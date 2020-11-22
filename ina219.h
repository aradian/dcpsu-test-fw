
#define INA219_REG_CONFIG 0
#define INA219_REG_SHUNTV 1
#define INA219_REG_BUSV   2
#define INA219_REG_POWER  3
#define INA219_REG_CURR   4
#define INA219_REG_CAL    5

#define CURRMON_LSB_CUR 0.00012207f
#define CURRMON_LSB_POW (CURRMON_LSB_CUR * 20.0f)
#define CURRMON_CAL     (0.04096f / (CURRMON_LSB_CUR * 0.080f))

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
  // TODO
} ina219_data_t;

char ina219_config(ina219_config_t);
ina219_data_t ina219_read();

