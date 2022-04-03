#include "relay.h"
#include "util/delay.h"
#include "config.h"

void
relay_init() {
  DDR_C1 |= (1<<BIT_C1); PORT_C1 &= ~(1<<BIT_C1);
  DDR_C2 |= (1<<BIT_C2); PORT_C2 &= ~(1<<BIT_C2);
  DDR_C3 |= (1<<BIT_C3); PORT_C3 &= ~(1<<BIT_C3);
  DDR_C4 |= (1<<BIT_C4); PORT_C4 &= ~(1<<BIT_C4);
  DDR_C5 |= (1<<BIT_C5); PORT_C5 &= ~(1<<BIT_C5);

  DDR_L1 |= (1<<BIT_L1); PORT_L1 &= ~(1<<BIT_L1);
  DDR_L2 |= (1<<BIT_L2); PORT_L2 &= ~(1<<BIT_L2);
  DDR_L3 |= (1<<BIT_L3); PORT_L3 &= ~(1<<BIT_L3);
  DDR_L4 |= (1<<BIT_L4); PORT_L4 &= ~(1<<BIT_L4);
  DDR_L5 |= (1<<BIT_L5); PORT_L5 &= ~(1<<BIT_L5);
  DDR_L6 |= (1<<BIT_L6); PORT_L6 &= ~(1<<BIT_L6);

  DDR_IO |= (1<<BIT_IO); PORT_IO &= ~(1<<BIT_IO);

#ifdef RESET_ANIMATION
  for (uint8_t i=0; i<12; i++) {
    relay_clear(RELAY_ALL);
    relay_set((1<<i));
    _delay_ms(100);
  }
#endif
}

void
relay_set(Relay relay) {
  if (relay & RELAY_C1) PORT_C1 |= (1<<BIT_C1);
  else PORT_C1 &= ~(1<<BIT_C1);
  if (relay & RELAY_C2) PORT_C2 |= (1<<BIT_C2);
  else PORT_C2 &= ~(1<<BIT_C2);
  if (relay & RELAY_C3) PORT_C3 |= (1<<BIT_C3);
  else PORT_C3 &= ~(1<<BIT_C3);
  if (relay & RELAY_C4) PORT_C4 |= (1<<BIT_C4);
  else PORT_C4 &= ~(1<<BIT_C4);
  if (relay & RELAY_C5) PORT_C5 |= (1<<BIT_C5);
  else PORT_C5 &= ~(1<<BIT_C5);

  if (relay & RELAY_L1) PORT_L1 |= (1<<BIT_L1);
  else PORT_L1 &= ~(1<<BIT_L1);
  if (relay & RELAY_L2) PORT_L2 |= (1<<BIT_L2);
  else PORT_L2 &= ~(1<<BIT_L2);
  if (relay & RELAY_L3) PORT_L3 |= (1<<BIT_L3);
  else PORT_L3 &= ~(1<<BIT_L3);
  if (relay & RELAY_L4) PORT_L4 |= (1<<BIT_L4);
  else PORT_L4 &= ~(1<<BIT_L4);
  if (relay & RELAY_L5) PORT_L5 |= (1<<BIT_L5);
  else PORT_L5 &= ~(1<<BIT_L5);
  if (relay & RELAY_L6) PORT_L6 |= (1<<BIT_L6);
  else PORT_L6 &= ~(1<<BIT_L6);

  if (relay & RELAY_IO) PORT_IO |= (1<<BIT_IO);
  else PORT_IO &= ~(1<<BIT_IO);
}


void
relay_clear(Relay relay) {
  if (relay & RELAY_C1) PORT_C1 &= ~(1<<BIT_C1);
  if (relay & RELAY_C2) PORT_C2 &= ~(1<<BIT_C2);
  if (relay & RELAY_C3) PORT_C3 &= ~(1<<BIT_C3);
  if (relay & RELAY_C4) PORT_C4 &= ~(1<<BIT_C4);
  if (relay & RELAY_C5) PORT_C5 &= ~(1<<BIT_C5);

  if (relay & RELAY_L1) PORT_L1 &= ~(1<<BIT_L1);
  if (relay & RELAY_L2) PORT_L2 &= ~(1<<BIT_L2);
  if (relay & RELAY_L3) PORT_L3 &= ~(1<<BIT_L3);
  if (relay & RELAY_L4) PORT_L4 &= ~(1<<BIT_L4);
  if (relay & RELAY_L5) PORT_L5 &= ~(1<<BIT_L5);
  if (relay & RELAY_L6) PORT_L6 &= ~(1<<BIT_L6);

  if (relay & RELAY_IO) PORT_IO &= ~(1<<BIT_IO);
}
