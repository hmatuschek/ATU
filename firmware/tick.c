#include "tick.h"
#include <avr/interrupt.h>

#include "button.h"
#include "led.h"
#include "tuner.h"

void tick_init() {
  TCCR0A |= ((1<<WGM01) | (0<<WGM00));
  TCCR0B |= ((0<<WGM02));
  OCR0A = 125 - 1;
  TIMSK0 |= (1<<OCIE0A);
  TCCR0B |= (0<<CS02)|(1<<CS01)|(1<<CS00);
}

ISR(TIMER0_COMPA_vect) {
  led_tick();
  button_tick();
  tuner_tick();
}
