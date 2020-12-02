#ifndef __LED_H__
#define __LED_H__

#include "config.h"
#include <inttypes.h>

typedef enum {
  LED_ON,
  LED_OFF,
  LED_BLINK
} LEDState;

void led_init();
void led_tick();

void led_set(LEDState state);
void led_set_swr(uint8_t swr);
void led_off();

#endif // __LED_H__
