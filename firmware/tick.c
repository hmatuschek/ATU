#include "tick.h"
#include <avr/interrupt.h>

#include "button.h"
#include "led.h"
#include "tuner.h"
#include <string.h>

void (* volatile _callbacks[TICK_NUM_CALLBACKS])();

void tick_init() {
  TCCR0A |= ((1<<WGM01) | (0<<WGM00));
  TCCR0B |= ((0<<WGM02));
  OCR0A = 125 - 1;
  TIMSK0 |= (1<<OCIE0A);
  TCCR0B &= ~((1<<CS02)|(1<<CS01)|(1<<CS00));

  for (uint8_t i=0; i<TICK_NUM_CALLBACKS; i++)
    _callbacks[i] = NULL;
}

void tick_start() {
  TCCR0B |= (0<<CS02)|(1<<CS01)|(1<<CS00);
}

void tick_stop() {
  TCCR0B &= ~((1<<CS02)|(1<<CS01)|(1<<CS00));
}


void tick_add_callback(void (*callback)()) {
  uint8_t i=0;
  for (; i<TICK_NUM_CALLBACKS; i++){
    if ((NULL == _callbacks[i]) || (callback == _callbacks[i])) {
      _callbacks[i] = callback;
      return;
    }
  }
}

void tick_rem_callback(void (*callback)()) {
  uint8_t i=0;
  for (; i<TICK_NUM_CALLBACKS; i++){
    if (callback == _callbacks[i]) {
      _callbacks[i] = NULL;
      return;
    }
  }
}

ISR(TIMER0_COMPA_vect) {
  for (uint8_t i=0; i<TICK_NUM_CALLBACKS; i++) {
    if (NULL != _callbacks[i])
      _callbacks[i]();
  }
}
