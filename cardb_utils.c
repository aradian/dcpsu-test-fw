
#include "cardb_utils.h"
#include "ncp5623.h"

void LED_SET_READY() {
  ncp5623_cmd_set_cur(31);
  ncp5623_cmd_set_color(0, 0xff, 0);
}

void LED_UNSET_READY() {
  ncp5623_cmd_shutdown();
}

void LED_SET_POWER() {
  ncp5623_cmd_set_color(0, 0, 0xff);
}

void LED_UNSET_POWER() {
  ncp5623_cmd_set_color(0xff, 0xff, 0);
}

