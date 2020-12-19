#include "tuner.h"
#include "led.h"
#include "relay.h"
#include "swr.h"
#include "tick.h"
#include <util/delay.h>
#include <avr/eeprom.h>
#include <util/atomic.h>

#define ABS(a) ((a<0) ? -a : a)


typedef enum {
  TUNER_WAIT,
  TUNER_TUNED_WAIT,
  TUNER_TUNE_C_COARSE,
  TUNER_TUNE_L_COARSE,
  TUNER_TUNE_C_FINE,
  TUNER_TUNE_L_FINE,
  TUNER_IDLE,
  TUNER_TUNED_IDLE,
  TUNER_SLEEP,
  TUNER_TUNED_SLEEP
} State;

volatile State _tuner_state = TUNER_IDLE;


#define MAX_C  31
#define MIN_C -31
#define MAX_L 63
#define MIN_L 0

volatile int8_t  C = 0;
volatile uint8_t L = 0;

volatile uint16_t _tuner_count = 0;

void tuner_tick();
void tuner_set_state(State state);


typedef struct {
  uint8_t  L;
  int8_t   C;
  uint16_t swr;
} LogEntry;

LogEntry _log[256] EEMEM;
uint8_t _log_index = 0;
void add_log(uint16_t swr) {
#if LOG_TUING
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    LogEntry entry = {L, C, swr};
    eeprom_write_block(&entry, &(_log[_log_index++]), sizeof(LogEntry));
  }
#endif
}

void tuner_init() {
  led_set(LED_ON);
  tuner_off();
  tick_add_callback(tuner_tick);
}

void tuner_off() {
  relay_clear(RELAY_ALL);
  tuner_set_state(TUNER_IDLE);
  L = C = 0;
}

void tuner_set_state(State state) {
  if (state == _tuner_state)
    return;

  _tuner_state = state;
  _tuner_count = 0;

  switch (_tuner_state) {
  case TUNER_WAIT:
  case TUNER_TUNED_WAIT:
    led_off();
    led_set(LED_SLOW);
    break;
  case TUNER_TUNE_C_COARSE:
  case TUNER_TUNE_L_COARSE:
  case TUNER_TUNE_C_FINE:
  case TUNER_TUNE_L_FINE:
    led_off();
    led_set(LED_FAST);
    break;
  case TUNER_IDLE:
  case TUNER_TUNED_IDLE:
    led_set(LED_ON);
    swr_start();
    break;
  case TUNER_SLEEP:
  case TUNER_TUNED_SLEEP:
    led_set(LED_OFF);
    led_off();
    swr_end();
    break;
  }
}


uint8_t tuner_is_sleeping() {
  return ((TUNER_SLEEP == _tuner_state) || (TUNER_TUNED_SLEEP == _tuner_state));
}

void tuner_wakeup() {
  if (tuner_is_sleeping()) {
    if (tuner_is_tuned())
      tuner_set_state(TUNER_TUNED_IDLE);
    else
      tuner_set_state(TUNER_IDLE);
  }
}

uint8_t tuner_is_waiting() {
  return ((TUNER_WAIT == _tuner_state) || (TUNER_TUNED_WAIT == _tuner_state));
}

uint8_t tuner_is_tuning() {
  return ((TUNER_TUNE_C_COARSE == _tuner_state) || (TUNER_TUNE_L_COARSE == _tuner_state) ||
          (TUNER_TUNE_C_FINE == _tuner_state) || (TUNER_TUNE_L_FINE == _tuner_state));
}

uint8_t tuner_is_idle() {
  return ((TUNER_IDLE == _tuner_state) || (TUNER_TUNED_IDLE == _tuner_state));
}

uint8_t tuner_is_tuned() {
  return ((TUNER_TUNED_IDLE == _tuner_state) || (TUNER_TUNED_SLEEP == _tuner_state) ||
          (TUNER_TUNED_WAIT == _tuner_state));
}


void tuner_set(uint8_t l, int8_t c) {
  if (l<MIN_L) l = MIN_L;
  if (l>MAX_L) l = MAX_L;
  if (c<MIN_C) c = MIN_C;
  if (c>MAX_C) c = MAX_C;
  L = l; C = c;

  uint8_t io = (c<0) ? 1 : 0;
  c = ABS(c);

  relay_clear(RELAY_ALL);
  relay_set((((uint16_t)c)<<C_SHIFT) | (((uint16_t) l)<<L_SHIFT) | (((uint16_t) io)<<IO_SHIFT));
}


void tuner_start() {
  if (tuner_is_idle()) {
    if (tuner_is_tuned())
      tuner_set_state(TUNER_TUNED_WAIT);
    else
      tuner_set(0,0);
      tuner_set_state(TUNER_WAIT);
  }
}

inline int8_t l_step() {
  if (L < 10)
    return TUNER_STEP_L;
  if (L < 20)
    return 2*TUNER_STEP_L;
  return 3*TUNER_STEP_L;
}

inline int8_t c_step() {
  if (ABS(C) < 10)
    return TUNER_STEP_C;
  if (ABS(C) < 20)
    return 2*TUNER_STEP_C;
  return 3*TUNER_STEP_C;
}


void tuner_poll() {
  static uint16_t swr_min = 9999;
  static uint8_t l_min;
  static int8_t  c_min;
  static uint8_t _cont = 1;
  static int8_t  _dir  = 1;

  if (tuner_is_waiting() && (10 <= pwr_read())) {
    _delay_ms(TUNER_FINE_DELAY);
    // Get initial SWR reading
    swr_min = swr_read();
    add_log(swr_min);
    led_set_swr(swr_min);

    if (tuner_is_tuned()) {
      tuner_set_state(TUNER_TUNE_L_FINE);
    } else {
      l_min = L; c_min = C;
      tuner_set_state(TUNER_TUNE_C_COARSE);
    }
  } else if (TUNER_TUNE_C_COARSE == _tuner_state) {
    int8_t c = C;
    uint16_t swr;

    // search for C
    if ((c < MAX_C) && (c > MIN_C)) {
      tuner_set(L,c + _dir*c_step());
      _delay_ms(TUNER_COARSE_DELAY);
      swr = swr_read();
      add_log(swr);
      led_set_swr(swr);
      if (SWR_LOW_POWER == swr) {
        return;
      } else if (swr > SWR_HOPELESS) {
        tuner_set_state(TUNER_TUNE_L_COARSE);
        return;
      } else if (swr < swr_min) {
        swr_min = swr;
        l_min   = L;
        c_min   = C;
        if (swr < SWR_GOOD) {
          tuner_set_state(TUNER_TUNED_IDLE);
        } if (swr < SWR_FAIR) {
          _cont = 1;
          tuner_set_state(TUNER_TUNE_L_FINE);
        }
      }
    } else {
      tuner_set_state(TUNER_TUNE_L_COARSE);
    }
  } else if (TUNER_TUNE_L_COARSE == _tuner_state) {
    uint8_t l = L;
    uint16_t swr;

    // search fo L
    if (l < MAX_L) {
      tuner_set(l+l_step(), 0);
      _delay_ms(TUNER_COARSE_DELAY);
      swr = swr_read();
      add_log(swr);
      led_set_swr(swr);
      if (SWR_LOW_POWER == swr) {
        return;
      } else if (swr > SWR_HOPELESS) {
        l = MAX_L;
        return;
      } if (swr < swr_min) {
        swr_min = swr;
        l_min = L;
        c_min = C;
        if (swr < SWR_GOOD) {
          tuner_set_state(TUNER_TUNED_IDLE);
        } if (swr < SWR_FAIR) {
          _cont = 1;
          tuner_set_state(TUNER_TUNE_L_FINE);
        }
      }
      tuner_set_state(TUNER_TUNE_C_COARSE);
    } else if (_dir > 0) {
      _dir = -1;
      L = 0; C = 0;
      tuner_set_state(TUNER_TUNE_C_COARSE);
    } else {
      tuner_set(l_min, c_min);
      _cont = 1;
      tuner_set_state(TUNER_TUNE_L_FINE);
    }
  } else if (TUNER_TUNE_C_FINE == _tuner_state) {
    int8_t c = C;
    uint16_t swr;

    if (c < MAX_C) {
      tuner_set(L, c+1);
      _delay_ms(TUNER_FINE_DELAY);
      swr = swr_read();
      add_log(swr);
      led_set_swr(swr);
      if (swr < swr_min) {
        swr_min = swr;
        c_min = C;
        _cont = 1;
        if (swr < SWR_GOOD) {
          tuner_set_state(TUNER_TUNED_IDLE);
        }
        return;
      }
    }

    if (c > MIN_C) {
      tuner_set(L, c-1);
      _delay_ms(TUNER_FINE_DELAY);
      swr = swr_read();
      add_log(swr);
      led_set_swr(swr);
      if (swr < swr_min) {
        swr_min = swr;
        c_min = C;
        _cont = 1;
        if (swr < SWR_GOOD) {
          tuner_set_state(TUNER_TUNED_IDLE);
        }
        return;
      }
    }

    L = l_min; C = c_min;
    tuner_set_state(TUNER_TUNE_L_FINE);
  } else if (TUNER_TUNE_L_FINE == _tuner_state) {
    uint8_t l = L;
    uint16_t swr;

    if (l < MAX_L) {
      tuner_set(l+1, C);
      _delay_ms(TUNER_FINE_DELAY);
      swr = swr_read();
      add_log(swr);
      led_set_swr(swr);
      if (swr < swr_min) {
        swr_min = swr;
        l_min = L;
        if (swr < SWR_GOOD) {
          tuner_set_state(TUNER_TUNED_IDLE);
        }
        return;
      }
    }

    if (l > MIN_L) {
      tuner_set(l-1, C);
      _delay_ms(TUNER_FINE_DELAY);
      swr = swr_read();
      add_log(swr);
      led_set_swr(swr);
      if (swr < swr_min) {
        swr_min = swr;
        l_min = L;
        if (swr < SWR_GOOD) {
          tuner_set_state(TUNER_TUNED_IDLE);
        }
        return;
      }
    }

    L = l_min; C = c_min;
    if (_cont) {
      _cont = 0;
      tuner_set_state(TUNER_TUNE_C_FINE);
    } else {
      tuner_set(L, C);
      tuner_set_state(TUNER_TUNED_IDLE);
    }
  } else if (tuner_is_idle()) {
    uint16_t swr;
    swr = swr_read();
    led_set_swr(swr);
  }
}


void tuner_tick() {
  if (tuner_is_waiting()) {
    _tuner_count++;
    if (5000 == _tuner_count) {
      if (tuner_is_tuned())
        tuner_set_state(TUNER_TUNED_IDLE);
      else
        tuner_set_state(TUNER_IDLE);
    }
  }

#if POWER_DOWN_ON_IDLE
  if (tuner_is_idle()) {
    _tuner_count++;
    if (10000 == _tuner_count) {
      if (tuner_is_tuned())
        tuner_set_state(TUNER_TUNED_SLEEP);
      else
        tuner_set_state(TUNER_SLEEP);
    }
  }
#endif
}


