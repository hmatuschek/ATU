#include "tuner.h"
#include "led.h"
#include "relay.h"
#include "swr.h"
#include <util/delay.h>

#define ABS(a) ((a<0) ? -a : a)


typedef enum {
  TUNER_OFF,
  TUNER_WAIT,
  TUNER_TUNE_C,
  TUNER_TUNE_L,
  TUNER_TUNED,
  TUNER_IDLE
} State;

volatile State _tuner_state = TUNER_OFF;


#define MAX_C  31
#define MIN_C -31
#define MAX_L 63
#define MIN_L 0

volatile int8_t  C = 0;
volatile uint8_t L = 0;
volatile uint8_t _last_swr = 0;
volatile uint8_t _continue = 0;


volatile uint16_t _tuner_count = 0;

void tuner_init() {
  tuner_off();
}

void tuner_off() {
  relay_clear(RELAY_ALL);
  _tuner_state = TUNER_OFF;
  L = C = 0;
}

uint8_t tuner_is_disabled() {
  return TUNER_OFF == _tuner_state;
}

uint8_t tuner_is_waiting() {
  return TUNER_WAIT == _tuner_state;
}

uint8_t tuner_is_tuning() {
  return ((TUNER_TUNE_C == _tuner_state) || (TUNER_TUNE_L == _tuner_state) ||
          (TUNER_TUNED == _tuner_state));
}

uint8_t tuner_is_idle() {
  return ((TUNER_OFF == _tuner_state) || (TUNER_IDLE == _tuner_state));
}

void tuner_set(uint8_t l, int8_t c) {
  if (l<MIN_L) l = MIN_L;
  if (l>MAX_L) l = MAX_L;
  if (c<MIN_C) c = MIN_C;
  if (c>MAX_C) c = MAX_C;
  L = l; C = c;

  uint8_t io = (c<0) ? 1 : 0;
  c = ABS(c);

  relay_set((((uint16_t)c)<<C_SHIFT) | (((uint16_t) l)<<L_SHIFT) | (((uint16_t) io)<<IO_SHIFT));
}


void tuner_start() {
  if (tuner_is_idle()) {
    _last_swr = 0;
    _tuner_count = 0;
    _tuner_state = TUNER_WAIT;
    led_set(LED_BLINK);
  }
}

void tuner_poll() {
  if ((TUNER_WAIT == _tuner_state) && (10 <= pwr_read())) {
    uint8_t swr, pwr;
    _delay_ms(TUNER_DELAY);
    // Get initial SWR reading
    swr_read(&swr, &pwr);
    _last_swr = swr;
    _continue = 0;
    _tuner_state = TUNER_TUNE_C;
  } else if (((TUNER_TUNE_C == _tuner_state) || (TUNER_TUNE_L == _tuner_state)) && (5 > pwr_read())) {
    // If not enough power -> stop tuning
    _continue = 0;
    _tuner_state = TUNER_TUNED;
  } else if (TUNER_TUNE_C == _tuner_state) {
    uint8_t c = C, swr, pwr;

    // try larger C
    if (c < MAX_C) {
      tuner_set(L,c+1);
      _delay_ms(TUNER_WAIT);
      swr_read(&swr, &pwr);
      if (swr < _last_swr) {
        _continue = 1;
        _last_swr = swr;
        return;
      }
    }

    // try smaller C
    if (c > MIN_C) {
      tuner_set(L,c-1);
      _delay_ms(TUNER_WAIT);
      swr_read(&swr, &pwr);
      if (swr < _last_swr) {
        _continue = 1;
        _last_swr = swr;
        return;
      }
    }

    // Tune L anyway
    _tuner_state = TUNER_TUNE_L;
  } else if (TUNER_TUNE_L == _tuner_state) {
    uint8_t l = L, swr, pwr;

    // try larger L
    if (l < MAX_L) {
      tuner_set(l+1,C);
      _delay_ms(TUNER_WAIT);
      swr_read(&swr, &pwr);
      if (swr < _last_swr) {
        _continue = 1;
        _last_swr = swr;
        return;
      }
    }

    // try smaller L
    if (l > MIN_L) {
      tuner_set(l-1, C);
      _delay_ms(TUNER_WAIT);
      swr_read(&swr, &pwr);
      if (swr < _last_swr) {
        _continue = 1;
        _last_swr = swr;
        return;
      }
    }

    // If L is optimal but C or L has been changed -> repeat
    if (_continue) {
      _continue = 0;
      _tuner_state = TUNER_TUNE_C;
    } else {
      // other wise, if L & C is optimal -> done.
      _tuner_state = TUNER_TUNED;
      _tuner_count = 0;
      led_set(LED_OFF);
    }
  }
}


void tuner_tick() {
  if (TUNER_TUNED == _tuner_state) {
    _tuner_count++;
    if (_tuner_count >= 3000) {
      led_off();
      _tuner_state = TUNER_IDLE;
    }
  }

  if (TUNER_WAIT == _tuner_state){
    _tuner_count++;
    if (_tuner_count >= 5000) {
      led_off();
      _tuner_count = TUNER_IDLE;
    }
  }
}

