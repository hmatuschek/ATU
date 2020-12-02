#ifndef __SWR_H__
#define __SWR_H__

#include "config.h"

void swr_init();

void swr_read(uint8_t *swr, uint8_t *pwr);
uint8_t pwr_read();

#endif // __SWR_H__
