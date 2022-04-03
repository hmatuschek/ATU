/*
 * ATU Project -- SWR measurement
 *
 * Published under CC-BY-SA 4.0 License. See LICENSE file for details.
 *
 * (c) 2020 Hannes Matuschek, DM3MAT, dm3mat (at) darc (dot) de.
 */

/** @defgroup swr SWR/PWR bridge measurement
 * This module implements the power and VSWR measurement.
 */
#ifndef __SWR_H__
#define __SWR_H__

#include "config.h"

/** Initializes ADCs for SWR and PWR measurements. */
void swr_init();
void swr_start();
void swr_end();

/** Measures VSWR in 100th, that is 234 is VSWR 1:2.34. */
uint16_t swr_read();

/** Reads PWR in 0.01W that is, 100 = 1W. */
uint16_t pwr_read();

#endif // __SWR_H__
