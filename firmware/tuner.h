#ifndef __TUNER_H__
#define __TUNER_H__

#include "config.h"
#include <inttypes.h>


void tuner_init();
void tuner_tick();

void tuner_poll();

void tuner_start();
void tuner_off();

uint8_t tuner_is_disabled();
uint8_t tuner_is_waiting();
uint8_t tuner_is_tuning();
uint8_t tuner_is_idle();

#endif // __TUNER_H__
