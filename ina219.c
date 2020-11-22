
#include "USI_TWI_Master.h"

#include "ina219.h"

char ina219_config(ina219_config_t config) {
  unsigned char usi_send[4] = {
    TWI_WRITE(ADDR_CURRMON),
    CURRMON_REG_CONFIG,
    config.reg.bytes[1],
    config.reg.bytes[0],
  };

  //printf("Cal currmon: 0x%x\n", ADDR_CURRMON);
  //printf("set config: 0x%x%x\n", usi_send[2], usi_send[3]);
  if (!USI_TWI_Start_Transceiver_With_Data(usi_send, 4))
    return -1;

  usi_send[1] = CURRMON_REG_CAL;
  config.reg.word = CURRMON_CAL;
  usi_send[2] = config.reg.bytes[1];
  usi_send[3] = config.reg.bytes[0];
  //printf("set cal: 0x%x%x\n", usi_send[2], usi_send[3]);
  if (!USI_TWI_Start_Transceiver_With_Data(usi_send, 4))
    return -1;
}

void read_currmon() {
  unsigned char usi_send[3] = {
    TWI_WRITE(ADDR_CURRMON),
    CURRMON_REG_CONFIG,
    0x00,
  };
  uint16_t value_reg = 0;

  printf("Read currmon: 0x%x\n", ADDR_CURRMON);

  // config
  if (!USI_TWI_Start_Transceiver_With_Data(usi_send, 2))
    PRINT_USI_ERROR;
  usi_send[0] = TWI_READ(ADDR_CURRMON);
  if (USI_TWI_Start_Transceiver_With_Data(usi_send, 3))
    printf("config: 0x%x%x\n", usi_send[1], usi_send[2]);
  else
    PRINT_USI_ERROR;

  // bus V
  usi_send[0] = TWI_WRITE(ADDR_CURRMON);
  usi_send[1] = CURRMON_REG_BUSV;
  if (!USI_TWI_Start_Transceiver_With_Data(usi_send, 2))
    PRINT_USI_ERROR;
  usi_send[0] = TWI_READ(ADDR_CURRMON);
  if (USI_TWI_Start_Transceiver_With_Data(usi_send, 3)) {
    value_reg = usi_send[1] * 0x100 + usi_send[2];
    value_reg >> 3;
    printf_P(p_cm_busv, usi_send[1], usi_send[2], value_reg);
  } else {
    PRINT_USI_ERROR;
  }

  // shunt V
  usi_send[0] = TWI_WRITE(ADDR_CURRMON);
  usi_send[1] = CURRMON_REG_SHUNTV;
  if (!USI_TWI_Start_Transceiver_With_Data(usi_send, 2))
    PRINT_USI_ERROR;
  usi_send[0] = TWI_READ(ADDR_CURRMON);
  if (USI_TWI_Start_Transceiver_With_Data(usi_send, 3)) {
    value_reg = usi_send[1] * 0x100 + usi_send[2];
    printf_P(p_cm_shuntv, usi_send[1], usi_send[2], value_reg);
  } else
    PRINT_USI_ERROR;

  // current
  usi_send[0] = TWI_WRITE(ADDR_CURRMON);
  usi_send[1] = CURRMON_REG_CURR;
  if (!USI_TWI_Start_Transceiver_With_Data(usi_send, 2))
    PRINT_USI_ERROR;
  usi_send[0] = TWI_READ(ADDR_CURRMON);
  if (USI_TWI_Start_Transceiver_With_Data(usi_send, 3)) {
    value_reg = usi_send[1] * 0x100 + usi_send[2];
    printf_P(p_cm_cur, usi_send[1], usi_send[2], value_reg);
  } else
    PRINT_USI_ERROR;

  // power
  usi_send[0] = TWI_WRITE(ADDR_CURRMON);
  usi_send[1] = CURRMON_REG_POWER;
  if (!USI_TWI_Start_Transceiver_With_Data(usi_send, 2))
    PRINT_USI_ERROR;
  usi_send[0] = TWI_READ(ADDR_CURRMON);
  if (USI_TWI_Start_Transceiver_With_Data(usi_send, 3)) {
    value_reg = usi_send[1] * 0x100 + usi_send[2];
    printf_P(p_cm_pow, usi_send[1], usi_send[2], value_reg);
  } else
    PRINT_USI_ERROR;
}

