#ifndef __LOG_H__
#define __LOG_H__

#include <inttypes.h>

/** Stores the current VSWR, L and C in the log. */
void add_log(uint16_t swr, uint8_t L, int8_t C);

#endif // __LOG_H__
