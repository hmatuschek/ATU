/*
 * ATU Project -- Main tuner implementation.
 *
 * Published under CC-BY-SA 4.0 License. See LICENSE file for details.
 *
 * (c) 2020 Hannes Matuschek, DM3MAT, dm3mat (at) darc (dot) de.
 */

/** @defgroup tuner Automatic Antenna Tuner (ATU) logic
 * This module implements the actual tuning algorithm. That is, it measues the SWR and tries to
 * find a combination of Ls & Cs to get the Antenna to appear resonant to the tuner ans therfore
 * to the radio.
 *
 * \dot
 * digraph SM {
 *   rankdir=LR; size="10,7";
 *
 *   node [shape = doublecircle];
 *   idle [label="Idle"];
 *
 *   node [shape=circle];
 *   wait  [label="Wait"];
 *   tunec [label="Tune C"];
 *   tunel [label="Tune L"];
 *   testc [label="Test C"];
 *   testl [label="Test L"];
 *   tuned [label="Tuned"];
 *
 *   idle  -> wait  [label="Btn"];
 *   wait  -> tunec [label="PWR>0.5W"];
 *   wait  -> idle  [label="delay 5s"];
 *   testc -> tuned [label="PWR<0.5W"];
 *   testl -> tuned [label="PWR<0.5W"];
 *   tunec -> tuned [label="PWR<0.5W"];
 *   tunel -> tuned [label="PWR<0.5W"];
 *
 *   testc -> tunec [label="dec. SWR"];
 *   testc -> tuned [label="else"];
 *   testl -> tunel [label="dec. SWR"];
 *   testl -> tuned [label="else"];
 *   tunec -> tunec [label="dec. SWR"];
 *   tunec -> testl [label="else"];
 *   tunel -> tunel [label="dec. SWR"];
 *   tunel -> testc [label="else"];
 *   tuned -> idle  [label="delay 3s"];
 * }
 * \enddot
 */
#ifndef __TUNER_H__
#define __TUNER_H__

#include "config.h"
#include <inttypes.h>

/** Initializes the tuner state-machine and resets all relays.
 * @ingroup tuner */
void tuner_init();
/** Gets called in the main loop to perform the actual tuning algorithm.
 * @ingroup tuner */
void tuner_poll();

/** Start tuning.
 * @ingroup tuner */
void tuner_start();
/** Disables the tuner and disconnects all Ls & Cs from the antenna. This also stops any ongoing
 * tuning.
 * @ingroup tuner */
void tuner_off();

uint8_t tuner_is_sleeping();
void tuner_wakeup();
/** Returns @c 1 if tuner is waiting for power to rise.
 * @ingroup tuner */
uint8_t tuner_is_waiting();
/** Returns @c 1 if tuning.
 * @ingroup tuner */
uint8_t tuner_is_tuning();
/** Returns @c 1 if tuner is idle.
 * @ingroup tuner */
uint8_t tuner_is_idle();

uint8_t tuner_is_tuned();

void tuner_error();

#endif // __TUNER_H__
