#include "led.h"

uint16_t _count = 0;
LEDState _led_state = LED_OFF;

void led_init() {
  DDR_LED_ACT |= (1<<BIT_LED_ACT); PORT_LED_ACT &= ~(1<<BIT_LED_ACT);

  DDR_LED0 |= (1<<BIT_LED0); PORT_LED0 &= ~(1<<BIT_LED0);
  DDR_LED1 |= (1<<BIT_LED1); PORT_LED1 &= ~(1<<BIT_LED1);
  DDR_LED2 |= (1<<BIT_LED2); PORT_LED2 &= ~(1<<BIT_LED2);
  DDR_LED3 |= (1<<BIT_LED3); PORT_LED3 &= ~(1<<BIT_LED3);

  _count = 0;
  _led_state = LED_OFF;
}

void led_off() {
  _led_state = LED_OFF;
  PORT_LED_ACT &= ~(1<<BIT_LED_ACT);
  PORT_LED0 &= ~(1<<BIT_LED0);
  PORT_LED1 &= ~(1<<BIT_LED1);
  PORT_LED2 &= ~(1<<BIT_LED2);
  PORT_LED3 &= ~(1<<BIT_LED3);
}

void led_set(LEDState state) {
  _led_state = state;
  if (LED_ON == _led_state)
    PORT_LED_ACT |= (1<<BIT_LED_ACT);
  if (LED_OFF == _led_state)
    PORT_LED_ACT &= ~(1<<BIT_LED_ACT);
}

void led_set_swr(uint8_t swr) {
  if (11 < swr)
    PORT_LED0 |=  (1<<BIT_LED0);
  else
    PORT_LED0 &= ~(1<<BIT_LED0);
  if (15 < swr)
    PORT_LED1 |= (1<<BIT_LED1);
  else
    PORT_LED1 &= ~(1<<BIT_LED1);
  if (20 < swr)
    PORT_LED2 |= (1<<BIT_LED2);
  else
    PORT_LED2 &= ~(1<<BIT_LED2);
  if (25 < swr)
    PORT_LED3 |= (1<<BIT_LED3);
  else
    PORT_LED3 &= ~(1<<BIT_LED3);
}

void led_tick() {
  _count++;
  if (500 == _count) {
    PIN_LED_ACT = (1<<BIT_LED_ACT);
    _count = 0;
  }
}
