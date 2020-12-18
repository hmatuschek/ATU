/*
 * ATU Project -- Button interface
 *
 * Published under CC-BY-SA 4.0 License. See LICENSE file for details.
 *
 * (c) 2020 Hannes Matuschek, DM3MAT, dm3mat (at) darc (dot) de.
 */

/** @defgroup button Button interface
 * This module provides the interface to the buttons. */
#ifndef __BUTTON_H__
#define __BUTTON_H__

#include "config.h"
#include <inttypes.h>

/** Possible button states.
 * @ingroup button */
typedef enum {
  BUTTON_NONE,   ///< Button not pressed.
  BUTTON_CLICK,  ///< Button clicked (short press).
  BUTTON_HOLD    ///< Button long press.
} Button;

/** Initializes the button interface.
 * @ingroup button */
void button_init();

/** Reads and resets the current button state.
 * @ingroup button */
Button button_state();

#endif // __BUTTON_H__
