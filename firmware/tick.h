/*
 * ATU Project -- Timer interface
 *
 * Published under CC-BY-SA 4.0 License. See LICENSE file for details.
 *
 * (c) 2020 Hannes Matuschek, DM3MAT, dm3mat (at) darc (dot) de.
 */

/** @defgroup tick Tick event generation
 * This module uses a timer to generate regular ticks to execute time-dependent code.
 */
#ifndef __TICK_H__
#define __TICK_H__

#include "config.h"

/** Initializes the tick-timer.
 * @ingroup tick */
void tick_init();

/** Starts the tick-timer.
 * @ingroup tick */
void tick_start();
/** Stops the tick-timer.
 * @ingroup tick */
void tick_stop();

/** Adds a callback to the tick-timer.
 * @ingroup tick */
void tick_add_callback(void (*callback)());
/** Removes a callback from the tick-timer.
 * @ingroup tick */
void tick_rem_callback(void (*callback)());

#endif // __TICK_H__
