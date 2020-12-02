#ifndef __BUTTON_H__
#define __BUTTON_H__

#include "config.h"
#include <inttypes.h>

typedef enum {
  BUTTON_NONE,
  BUTTON_CLICK,
  BUTTON_HOLD
} Button;

void button_init();
void button_tick();

Button button_state();

#endif // __BUTTON_H__
