
#include "USI_TWI_Master.h"
#include "ncp5623.h"

char ncp5623_msg_send(ncp5623_msg_t msg) {
  unsigned char usi_send[2] = {
    TWI_WRITE(NCP5623_ADDR),
    msg.byte,
  };
  if (!USI_TWI_Start_Transceiver_With_Data(usi_send, 2))
    return -1;

  return 0;
}

void ncp5623_cmd_shutdown() {
  ncp5623_msg_t msg;
  msg.field.cmd = NCP5623_CMD_SHUTDOWN;
  ncp5623_msg_send(msg);
}

void ncp5623_cmd_set_cur(uint8_t val) {
  ncp5623_msg_t msg;
  msg.field.cmd = NCP5623_CMD_SET_CUR;
  msg.field.data = val;
  ncp5623_msg_send(msg);
}

void ncp5623_cmd_set_color(uint8_t red, uint8_t green, uint8_t blue) {
  ncp5623_msg_t msg;
  msg.field.cmd = NCP5623_CMD_PWM_RED;
  msg.field.data = red;
  ncp5623_msg_send(msg);
  msg.field.data = green;
  ncp5623_msg_send(msg);
  msg.field.data = blue;
  ncp5623_msg_send(msg);
}

void ncp5623_cmd_config_dim() {
}

