#include "button.h"
#include <avr/interrupt.h>

uint8_t  _last_button;
uint16_t _button_count;
Button   _button_state;

void button_init() {
  DDR_BUTTON &= ~(1<<BIT_BUTTON);
  PORT_BUTTON |= (1<<BIT_BUTTON);
  _last_button = ((PIN_BUTTON>>BIT_BUTTON)&1);
  _button_state = BUTTON_NONE;

  // Enable pin-change interrupt on PCIE22
  PCICR  |= (1<<PCIE2);
  PCMSK2 |= (1<<PCINT22);
}

void button_tick() {
  // read current button value (0=pressed, 1=released)
  uint8_t button = ((PIN_BUTTON>>BIT_BUTTON)&1);
  if ((1 == button) && (0 == _last_button) && (2000 > _button_count)) {
    _button_state = BUTTON_CLICK;
  } else if ((0 == button) && (1 == _last_button)) {
    _button_count = 0;
  } else if ((0 == button) && (2000 > _button_count)) {
    _button_count++;
  } else if (2000 == _button_count) {
    _button_state = BUTTON_HOLD;
    _button_count++;
  }
  _last_button = button;
}

Button button_state() {
  Button state = _button_state;
  _button_state = BUTTON_NONE;
  return state;
}


ISR(PCINT2_vect) {
  // pass...
}
