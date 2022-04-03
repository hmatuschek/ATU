/*
 * ATU Project -- LED interface
 *
 * Published under CC-BY-SA 4.0 License. See LICENSE file for details.
 *
 * (c) 2020 Hannes Matuschek, DM3MAT, dm3mat (at) darc (dot) de.
 */

/** @defgroup led LED interface
 * This module implements all routines to controll the LEDs.
 */
#ifndef __LED_H__
#define __LED_H__

#include "config.h"
#include <inttypes.h>

/** Possible states of the status LED.
 * @ingroup led */
typedef enum {
  LED_ON,    ///< Constantly on.
  LED_OFF,   ///< Constantly off.
  LED_SLOW,  ///< Blinking slowly.
  LED_FAST,  ///< Blinking fast.
  LED_ERROR  ///< Blinking all LEDs.
} LEDState;

/** Initializes the LED interface.
 * @ingroup led */
void led_init();

/** Set the status LED.
 * @ingroup led */
void led_set(LEDState state);
/** Set the SWR LED bar-graph.
 * @ingroup led */
void led_set_swr(uint16_t swr);
/** Set the SWR LED bar-graph.
 * @ingroup led */
void led_set_power(uint16_t pwr);
/** Turns all LEDs off.
 * @ingroup led */
void led_off();

#endif // __LED_H__
