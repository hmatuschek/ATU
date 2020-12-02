#ifndef __RELAY_H__
#define __RELAY_H__

#include "config.h"

#define C_SHIFT   0
#define L_SHIFT   5
#define IO_SHIFT 11

typedef enum {
  RELAY_C1 = (1<< 0),
  RELAY_C2 = (1<< 1),
  RELAY_C3 = (1<< 2),
  RELAY_C4 = (1<< 3),
  RELAY_C5 = (1<< 4),
  RELAY_L1 = (1<< 5),
  RELAY_L2 = (1<< 6),
  RELAY_L3 = (1<< 7),
  RELAY_L4 = (1<< 8),
  RELAY_L5 = (1<< 9),
  RELAY_L6 = (1<<10),
  RELAY_IO = (1<<11),
  RELAY_ALL = (RELAY_C1 | RELAY_C2 | RELAY_C3 | RELAY_C4 | RELAY_C5 | RELAY_L1 | RELAY_L1 |
               RELAY_L1 | RELAY_L1 | RELAY_L1 | RELAY_L1 | RELAY_IO)
} Relay;


void relay_init();
void relay_set(Relay relay);
void relay_clear(Relay relay);

#endif // __RELAY_H__
