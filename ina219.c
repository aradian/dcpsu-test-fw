
#include "USI_TWI_Master.h"

#include "ina219.h"

char ina219_error = 0;

char ina219_set_config(ina219_config_t config) {
  unsigned char usi_send[4] = {
    TWI_WRITE(INA219_ADDR),
    INA219_REG_CONFIG,
    config.reg.bytes[1],
    config.reg.bytes[0],
  };

  //printf("Cal currmon: 0x%x\n", INA219_ADDR);
  //printf("set config: 0x%x%x\n", usi_send[2], usi_send[3]);
  if (!USI_TWI_Start_Transceiver_With_Data(usi_send, 4))
    return -1;

  return 0;
}

char ina219_set_cal() {
  reg16_t cal = {INA219_CAL};
  unsigned char usi_send[4] = {
    TWI_WRITE(INA219_ADDR),
    INA219_REG_CAL,
    cal.bytes[1],
    cal.bytes[0],
  };
  //printf("set cal: 0x%x%x\n", usi_send[2], usi_send[3]);
  if (!USI_TWI_Start_Transceiver_With_Data(usi_send, 4))
    return -1;

  return 0;
}

ina219_config_t ina219_get_config() {
  ina219_config_t config;
  config.reg.word = 0;
  unsigned char usi_send[3] = {
    TWI_WRITE(INA219_ADDR),
    INA219_REG_CONFIG,
    0x00,
  };

  if (!USI_TWI_Start_Transceiver_With_Data(usi_send, 2)) {
    //PRINT_USI_ERROR;
    return config;
  }
  usi_send[0] = TWI_READ(INA219_ADDR);
  if (!USI_TWI_Start_Transceiver_With_Data(usi_send, 3)) {
    //PRINT_USI_ERROR;
    return config;
  }
  config.reg.bytes[0] = usi_send[1];
  config.reg.bytes[1] = usi_send[2];
  return config;
}

reg16_t ina219_read_data_reg(uint8_t reg) {
  unsigned char usi_send[3] = {
    TWI_WRITE(INA219_ADDR),
    reg,
    0x00,
  };
  reg16_t value = {0};

  if (!USI_TWI_Start_Transceiver_With_Data(usi_send, 2)) {
    //PRINT_USI_ERROR;
    return value;
  }
  usi_send[0] = TWI_READ(INA219_ADDR);
  if (!USI_TWI_Start_Transceiver_With_Data(usi_send, 3)) {
    //PRINT_USI_ERROR;
    return value;
  }
  value.bytes[0] = usi_send[1];
  value.bytes[1] = usi_send[2];
  return value;
}

ina219_data_t ina219_read() {
  ina219_data_t data = {0, 0, 0, 0};
  reg16_t value = {0};

  // bus V
  value = ina219_read_data_reg(INA219_REG_BUSV);
  value.word >> 3;
  data.bus_v = value.word * 0.004f;

  // shunt V
  value = ina219_read_data_reg(INA219_REG_SHUNTV);
  data.shunt_v = value.word * 0.00001f;

  // current
  value = ina219_read_data_reg(INA219_REG_CURR);
  data.current = value.word * INA219_LSB_CUR;

  // power
  value = ina219_read_data_reg(INA219_REG_POWER);
  data.power = value.word * INA219_LSB_POW;

  return data;
}

