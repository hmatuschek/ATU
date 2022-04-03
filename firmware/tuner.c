#include "tuner.h"
#include "led.h"
#include "relay.h"
#include "swr.h"
#include "tick.h"
#include "log.h"
#include <util/delay.h>

#define ABS(a) ((a<0) ? -a : a)

/** Stages of tuning. Starting with IDLE after reset. */
typedef enum {
  STAGE_IDLE  = 0,      ///< Initial state.
  STAGE_WAIT  = 1,      ///< Waits for the input power to raise above MIN_POWER.
  STAGE_TUNE  = 2,      ///< Tuning
  STAGE_SLEEP = 3       ///< After tuning, reenters IDLE, may enter SLEEP after some time in idle.
} Stage;

/** Possible elements to tune. */
typedef enum {
  TUNE_C = 0,           ///< Tune the Cs
  TUNE_L = 1            ///< Tune the Ls
} Element;

/** Possible tuning modes. */
typedef enum {
  COARSE = 0,           ///< In a fist run, the Cs and Ls are tuned coarse.
  FINE   = 1            ///< In a second step, they are tuned fine. Also once tuned, only a fine
                        ///  tune is performed.
} Grain;

/** High/Low Z tune. */
typedef enum {
  POSITIVE = 0,         ///< Cs at the input.
  NEGATIVE = 1          ///< Cs at the output.
} Direction;

/** Combined state of the tuner. */
typedef struct {
  uint8_t stage : 3,    ///< State of tuning, IDLE, Wait, ...
    tuned       : 1,    ///< If 1, the network is tuned already.
    element     : 1,    ///< Which elements to tune.
    grain       : 1,    ///< Coarse or fine?
    final       : 1,    ///< Final round of tuning?
    direction   : 1;    ///< Cs at input or output?

  uint8_t L;            ///< The currently selected Ls.
  int8_t  C;            ///< The currently selected Cs.

  // Optimum found so far.
  uint16_t swr_min;     ///< Minimum VSWR yet.
  uint8_t l_min;        ///< Best L.
  int8_t  c_min;        ///< Best C.
} State;


// Tuner state variable
volatile State _tuner_state;


#define MAX_C  31
#define MIN_C -31
#define MAX_L 63
#define MIN_L 0

volatile uint16_t _tuner_count = 0;

void tuner_tick();
void tuner_set_tuned();
void tuner_tune(uint8_t coarse, uint8_t tune_l);
void tuner_set(uint8_t l, int8_t c);
uint8_t tuner_is_fine_tuning_L();


void tuner_init() {
  led_set(LED_ON);
  tuner_reset();
  tick_add_callback(tuner_tick);
}

void tuner_reset() {
  relay_clear(RELAY_ALL);

  _tuner_state.stage = STAGE_IDLE;
  _tuner_state.tuned = 0;
  _tuner_state.element = TUNE_C;
  _tuner_state.grain = COARSE;
  _tuner_state.final = 0;
  _tuner_state.direction = POSITIVE;

  _tuner_state.L = 0;
  _tuner_state.C = 0;

  _tuner_state.swr_min = 9999;
  _tuner_state.l_min = 0;
  _tuner_state.c_min = 0;

  _tuner_count = 0;
  swr_end();
}

void tuner_start() {
  if (tuner_is_idle()) {
    _tuner_state.stage = STAGE_WAIT;
    _tuner_count = 0;

    if (tuner_is_tuned()) {
      tuner_tune(FINE, TUNE_C);
    } else {
      tuner_set(0, 0);
      _tuner_state.direction = POSITIVE;
      tuner_tune(COARSE, TUNE_C);
    }

    led_off();
    led_set(LED_SLOW);
    swr_start();
  }
}

void tuner_tune(Grain grain, Element element) {
  led_off();
  led_set(LED_FAST);

  if ((STAGE_TUNE == _tuner_state.stage) && (grain == _tuner_state.grain) && (element == _tuner_state.element))
    return;

  _tuner_count = 0;

  _tuner_state.stage = STAGE_TUNE;
  _tuner_state.grain = grain;
  _tuner_state.element = element;
  _tuner_state.final = tuner_is_fine_tuning_L();
}

void tuner_set_tuned() {
  _tuner_state.stage = STAGE_IDLE;
  _tuner_state.tuned = 1;
  _tuner_count = 0;
  led_off();
  led_set(LED_ON);
}

void tuner_pause() {
  _tuner_state.stage = STAGE_WAIT;
  led_off();
  led_set(LED_SLOW);
}

void tuner_resume() {
  _tuner_state.stage = STAGE_TUNE;
  led_off();
  led_set(LED_FAST);
}

void tuner_sleep() {
  _tuner_state.stage = STAGE_SLEEP;
  led_set(LED_OFF);
  led_off();
  swr_end();
}

void tuner_wakeup() {
  if (tuner_is_sleeping())
    _tuner_state.stage = STAGE_IDLE;
  _tuner_count = 0;
  led_set(LED_ON);
  swr_start();
}


uint8_t tuner_is_sleeping() {
  return STAGE_SLEEP == _tuner_state.stage;
}

uint8_t tuner_is_waiting() {
  return STAGE_WAIT == _tuner_state.stage;
}

uint8_t tuner_is_tuning() {
  return STAGE_TUNE == _tuner_state.stage;
}

uint8_t tuner_is_coarse_tuning_C() {
  return (STAGE_TUNE == _tuner_state.stage) && (TUNE_C == _tuner_state.element) &&
      (COARSE == _tuner_state.grain);
}

uint8_t tuner_is_coarse_tuning_L() {
  return (STAGE_TUNE == _tuner_state.stage) && (TUNE_L == _tuner_state.element) &&
      (COARSE == _tuner_state.grain);
}

uint8_t tuner_is_fine_tuning_C() {
  return (STAGE_TUNE == _tuner_state.stage) && (TUNE_C == _tuner_state.element) &&
      (FINE == _tuner_state.grain);
}

uint8_t tuner_is_fine_tuning_L() {
  return (STAGE_TUNE == _tuner_state.stage) && (TUNE_L == _tuner_state.element) &&
      (FINE == _tuner_state.grain);
}

uint8_t tuner_is_final_tuning() {
  return _tuner_state.final;
}

uint8_t tuner_is_idle() {
  return STAGE_IDLE == _tuner_state.stage;
}

uint8_t tuner_is_tuned() {
  return _tuner_state.tuned;
}


Direction tuner_direction() {
  return _tuner_state.direction;
}

void tuner_set_direction(Direction dir) {
  _tuner_state.direction = dir;
}

int8_t sign() {
  return (POSITIVE == _tuner_state.direction) ? 1 : -1;
}


uint8_t tuner_current_L() {
  return _tuner_state.L;
}

uint8_t tuner_min_L() {
  return _tuner_state.l_min;
}

int8_t tuner_current_C() {
  return _tuner_state.C;
}

int8_t tuner_min_C() {
  return _tuner_state.c_min;
}

uint16_t tuner_min_swr() {
  return _tuner_state.swr_min;
}

void tuner_set_min_swr(uint16_t swr, uint8_t L, int8_t C) {
  _tuner_state.swr_min = swr;
  _tuner_state.l_min = L;
  _tuner_state.c_min = C;
  _tuner_state.final = 0;
}

void tuner_set(uint8_t l, int8_t c) {
  if (l<MIN_L) l = MIN_L;
  if (l>MAX_L) l = MAX_L;
  if (c<MIN_C) c = MIN_C;
  if (c>MAX_C) c = MAX_C;
  _tuner_state.L = l;
  _tuner_state.C = c;

  uint8_t io = (c<0) ? 1 : 0;
  c = ABS(c);

  relay_clear(RELAY_ALL);
  relay_set((((uint16_t)c)<<C_SHIFT) | (((uint16_t) l)<<L_SHIFT) | (((uint16_t) io)<<IO_SHIFT));
}


inline int8_t l_step() {
  if (_tuner_state.L < 10)
    return TUNER_STEP_L;
  if (_tuner_state.L < 20)
    return 2*TUNER_STEP_L;
  return 3*TUNER_STEP_L;
}

inline int8_t c_step() {
  if (ABS(_tuner_state.C) < 10)
    return TUNER_STEP_C;
  if (ABS(_tuner_state.C) < 20)
    return 2*TUNER_STEP_C;
  return 3*TUNER_STEP_C;
}


void tuner_poll() {
  if (tuner_is_waiting() && (MIN_POWER <= pwr_read())) {
    _delay_ms(TUNER_FINE_DELAY);

    uint16_t swr = swr_read();
    if ((SWR_LOW_POWER == swr) || (SWR_HOPELESS == swr))
      return;

    // Get and store SWR reading
    tuner_set_min_swr(swr, tuner_current_L(), tuner_current_C());
    add_log(swr, tuner_current_L(), tuner_current_C());
    led_set_swr(swr);
    tuner_resume();
  } else if (tuner_is_coarse_tuning_C()) {
    int8_t c = tuner_current_C();
    uint16_t swr;

    // search for C
    if ((c < MAX_C) && (c > MIN_C)) {
      tuner_set(tuner_current_L(), c + sign()*c_step());
      _delay_ms(TUNER_COARSE_DELAY);

      swr = swr_read();
      if (SWR_LOW_POWER == swr) {
        tuner_pause();
        return;
      }

      add_log(swr, tuner_current_L(), tuner_current_C());
      led_set_swr(swr);

      if (swr > SWR_HOPELESS) {
        // Continue tuning L coarse
        tuner_tune(COARSE, TUNE_L);
      } else if (swr < tuner_min_swr()) {
        // If better SWR store
        tuner_set_min_swr(swr, tuner_current_L(), tuner_current_C());
        if (swr < SWR_GOOD) {
          // Quick exit -> done
          tuner_set_tuned();
        } else if (swr < SWR_FAIR) {
          // Try fine-tuning C
          tuner_tune(FINE, TUNE_C);
        }
      }
    } else {
      // Continue coarse-tuning L
      tuner_tune(COARSE, TUNE_L);
    }
  } else if (tuner_is_coarse_tuning_L()) {
    uint8_t l = tuner_current_L();
    uint16_t swr;

    // search fo L
    if (l < MAX_L) {
      tuner_set(l+l_step(), 0);
      _delay_ms(TUNER_COARSE_DELAY);

      swr = swr_read();
      if (SWR_LOW_POWER == swr) {
        tuner_pause();
        return;
      }

      add_log(swr, tuner_current_L(), tuner_current_C());
      led_set_swr(swr);

      if (swr > SWR_HOPELESS) {
        // pass...
      } else if (swr < tuner_min_swr()) {
        // If better SWR is found
        tuner_set_min_swr(swr, tuner_current_L(), tuner_current_C());
        if (swr < SWR_GOOD) {
          // Quick exit -> done
          tuner_set_tuned();
        } if (swr < SWR_FAIR) {
          // Good enough -> fine tune C
          tuner_tune(FINE, TUNE_C);
        }
        return;
      }
      // Continue coarse tuning C
      tuner_tune(COARSE, TUNE_C);
    } else if (POSITIVE == tuner_direction()) {
      // If L = MAX_L and Cs are at input, try Cs at output
      tuner_set_direction(NEGATIVE);
      tuner_set(0, 0);
      tuner_tune(COARSE, TUNE_C);
    } else {
      // This branch basically means failed tune.
      // So set to best match yet and enter fine-tune C (this will end the tuneing).
      tuner_set(tuner_min_L(), tuner_min_C());
      tuner_tune(FINE, TUNE_L);
    }
  } else if (tuner_is_fine_tuning_C()) {
    int8_t c = tuner_current_C();
    uint16_t swr;

    if (c < MAX_C) {
      tuner_set(tuner_current_L(), c+1);
      _delay_ms(TUNER_FINE_DELAY);

      swr = swr_read();
      if (SWR_LOW_POWER == swr) {
        tuner_pause();
        return;
      }

      add_log(swr, tuner_current_L(), tuner_current_C());
      led_set_swr(swr);

      if (swr < tuner_min_swr()) {
        tuner_set_min_swr(swr, tuner_current_L(), tuner_current_C());
        if (swr < SWR_GOOD) {
          tuner_set_tuned();
        }
        return;
      }
    }

    // If increasing C does not find better VSWR -> try decreasing C
    if (c > MIN_C) {
      tuner_set(tuner_current_L(), c-1);
      _delay_ms(TUNER_FINE_DELAY);

      swr = swr_read();
      if (SWR_LOW_POWER == swr) {
        tuner_pause();
        return;
      }

      add_log(swr, tuner_current_L(), tuner_current_C());
      led_set_swr(swr);

      if (swr < tuner_min_swr()) {
        tuner_set_min_swr(swr, tuner_current_L(), tuner_current_C());
        if (swr < SWR_GOOD) {
          tuner_set_tuned();
        }
        return;
      }
    }

    tuner_set(tuner_min_L(), tuner_min_C());
    tuner_tune(FINE, TUNE_L);
  } else if (tuner_is_fine_tuning_L()) {
    uint8_t l = tuner_current_L();
    uint16_t swr;

    if (l < MAX_L) {
      tuner_set(l+1, tuner_current_C());
      _delay_ms(TUNER_FINE_DELAY);

      swr = swr_read();
      if (SWR_LOW_POWER == swr) {
        tuner_pause();
        return;
      }

      add_log(swr, tuner_current_L(), tuner_current_C());
      led_set_swr(swr);

      if (swr < tuner_min_swr()) {
        tuner_set_min_swr(swr, tuner_current_L(), tuner_current_C());
        if (swr < SWR_GOOD) {
          tuner_set_tuned();
        }
        return;
      }
    }

    if (l > MIN_L) {
      tuner_set(l-1, tuner_current_C());
      _delay_ms(TUNER_FINE_DELAY);

      swr = swr_read();
      if (SWR_LOW_POWER == swr) {
        tuner_pause();
        return;
      }

      add_log(swr, tuner_current_L(), tuner_current_C());
      led_set_swr(swr);

      if (swr < tuner_min_swr()) {
        tuner_set_min_swr(swr, tuner_current_L(), tuner_current_C());
        if (swr < SWR_GOOD) {
          tuner_set_tuned();
        }
        return;
      }
    }

    // If nothing helps, take current minimum
    tuner_set(tuner_min_L(), tuner_min_C());
    if (tuner_is_final_tuning())
      tuner_set_tuned();
    else
      tuner_tune(FINE, TUNE_C);

  } else if (tuner_is_idle()) {
    uint16_t swr;
    swr = swr_read();
    led_set_swr(swr);
  }
}


void tuner_tick() {
  if (tuner_is_waiting()) {
    _tuner_count++;
    if (5000 <= _tuner_count) {
      _tuner_state.stage = STAGE_IDLE;
      _tuner_count = 0;
      led_set(LED_ON);
    }
  }

#ifdef POWER_DOWN_ON_IDLE
  if (tuner_is_idle()) {
    _tuner_count++;
    if (10000 == _tuner_count) {
      tuner_sleep();
    }
  }
#endif
}


