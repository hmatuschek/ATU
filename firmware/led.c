#include "led.h"
#include "tick.h"
#include "config.h"
#include <util/delay.h>

uint16_t _count = 0;
LEDState _led_state = LED_OFF;

void led_tick();


void led_init() {
  DDR_LED_ACT |= (1<<BIT_LED_ACT); PORT_LED_ACT &= ~(1<<BIT_LED_ACT);

  DDR_LED0 |= (1<<BIT_LED0); PORT_LED0 &= ~(1<<BIT_LED0);
  DDR_LED1 |= (1<<BIT_LED1); PORT_LED1 &= ~(1<<BIT_LED1);
  DDR_LED2 |= (1<<BIT_LED2); PORT_LED2 &= ~(1<<BIT_LED2);
  DDR_LED3 |= (1<<BIT_LED3); PORT_LED3 &= ~(1<<BIT_LED3);

  _count = 0;
  _led_state = LED_OFF;

  tick_add_callback(led_tick);

#ifdef RESET_ANIMATION
  led_off();
  PORT_LED_ACT |= (1<<BIT_LED_ACT);
  _delay_ms(100);
  PORT_LED0 |= (1<<BIT_LED0);
  _delay_ms(100);
  PORT_LED1 |= (1<<BIT_LED1);
  _delay_ms(100);
  PORT_LED2 |= (1<<BIT_LED2);
  _delay_ms(100);
  PORT_LED3 |= (1<<BIT_LED3);
  _delay_ms(100);
  led_off();
#endif
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

void led_set_swr(uint16_t swr) {
  if (9999 == swr)
      swr = 0;
  if (120 < swr)
    PORT_LED0 |=  (1<<BIT_LED0);
  else
    PORT_LED0 &= ~(1<<BIT_LED0);
  if (150 < swr)
    PORT_LED1 |= (1<<BIT_LED1);
  else
    PORT_LED1 &= ~(1<<BIT_LED1);
  if (200 < swr)
    PORT_LED2 |= (1<<BIT_LED2);
  else
    PORT_LED2 &= ~(1<<BIT_LED2);
  if (250 < swr)
    PORT_LED3 |= (1<<BIT_LED3);
  else
    PORT_LED3 &= ~(1<<BIT_LED3);
}

void led_tick() {
  if (LED_SLOW == _led_state) {
    _count++;
    if (500 == _count) {
      PIN_LED_ACT |= (1<<BIT_LED_ACT);
      _count = 0;
    }
  } else if (LED_FAST == _led_state) {
    _count++;
    if (200 == _count) {
      PIN_LED_ACT |= (1<<BIT_LED_ACT);
      _count = 0;
    }
  }
}
