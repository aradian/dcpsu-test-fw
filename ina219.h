#include <stdint.h>

#define INA219_ADDR 0b1000000

#define INA219_REG_CONFIG 0
#define INA219_REG_SHUNTV 1
#define INA219_REG_BUSV   2
#define INA219_REG_POWER  3
#define INA219_REG_CURR   4
#define INA219_REG_CAL    5

//                       R-RPPBBBBSSSSMMM
#define INA219_CONFIG  0b0011100000011111
#define INA219_LSB_CUR 0.00012207f
#define INA219_LSB_POW (INA219_LSB_CUR * 20.0f)
#define INA219_CAL     (0.04096f / (INA219_LSB_CUR * 0.080f))

struct ina219_config_fields {
  uint8_t reset : 1;
  uint8_t unused : 1;
  uint8_t bus_v_range : 1;
  uint8_t shunt_v_range : 2;
  uint8_t bus_adc : 4;
  uint8_t shunt_adc : 4;
  uint8_t mode : 3;
};
typedef union u_reg_16 {
  uint16_t word;
  uint8_t bytes[2];
} reg16_t;
typedef union ina219_config {
  reg16_t reg;
  struct ina219_config_fields fields;
} ina219_config_t;

typedef struct ina219_data {
  float bus_v;
  float shunt_v;
  float current;
  float power;
} ina219_data_t;

char ina219_set_config(ina219_config_t);
char ina219_set_cal();
ina219_config_t ina219_get_config();
reg16_t ina219_read_data_reg(uint8_t);
ina219_data_t ina219_read();

