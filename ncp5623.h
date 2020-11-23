#include <stdint.h>

#define NCP5623_ADDR 0b0111000

#define NCP5623_CMD_SHUTDOWN   0b000
#define NCP5623_CMD_SET_CUR    0b001
#define NCP5623_CMD_PWM_RED    0b010
#define NCP5623_CMD_PWM_GREEN  0b011
#define NCP5623_CMD_PWM_BLUE   0b100
#define NCP5623_CMD_DIM_UP_END 0b101
#define NCP5623_CMD_DIM_DN_END 0b110
#define NCP5623_CMD_DIM_TIME   0b111

struct ncp5623_msg_fields {
  uint8_t cmd : 3;
  uint8_t data : 5;
};
typedef union ncp5623_msg {
  uint8_t byte;
  struct ncp5623_msg_fields field;
} ncp5623_msg_t;

void ncp5623_cmd_shutdown();
void ncp5623_cmd_set_cur(uint8_t);
void ncp5623_cmd_set_color(uint8_t, uint8_t, uint8_t);
void ncp5623_cmd_config_dim();

