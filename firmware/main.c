/** @mainpage Automatic Antenna Tuner
 * This page documents the firmware of the automatic antenna tuner.
 *
 * @author Hannes Matuschek, DM3MAT, dm3mat@darc.de
 * @version 0.1.0, HW rev. 0
 * @date 2020
 * @copyright GNU Public License (Version 2.0 or later)
 */

#include "relay.h"
#include "button.h"
#include "swr.h"
#include "led.h"
#include "tick.h"
#include "tuner.h"

#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <util/delay.h>

int main()
{
  tick_init();

  relay_init();
  button_init();
  swr_init();

  led_init();
  tuner_init();

  sei();
  tick_start();

  while (1) {
    if (tuner_is_idle() && (BUTTON_CLICK == button_state())) {
      tuner_start();
    } else if (tuner_is_sleeping()) {
      /*set_sleep_mode(SLEEP_MODE_PWR_DOWN);
      sleep_mode();
      tuner_wakeup();*/
    } else {
      tuner_poll();
    }
  }

  return 0;
}
