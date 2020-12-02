#include "relay.h"
#include "button.h"
#include "swr.h"
#include "led.h"
#include "tick.h"
#include "tuner.h"

#include <avr/interrupt.h>
#include <avr/sleep.h>

int main()
{
  relay_init();
  button_init();
  swr_init();
  led_init();

  tuner_init();

  sei();
  tick_init();

  while (1) {
    Button button = button_state();
    if (BUTTON_CLICK == button) {
      tuner_start();
    } else if (tuner_is_idle()) {
      set_sleep_mode(SLEEP_MODE_PWR_DOWN);
      sleep_mode();
    } else {
      tuner_poll();
    }
  }
  return 0;
}
