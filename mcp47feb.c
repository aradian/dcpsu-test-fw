
#include "USI_TWI_Master.h"
#include "mcp47feb.h"

bool _mcp47feb_write_reg(uint8_t addr, reg16_t value) {
  unsigned char usi_send[4] = {
    TWI_WRITE(MCP47FEB_ADDR),
    (addr << 3) | MCP47FEB_CMD_WRITE,
    value.bytes[1],
    value.bytes[0],
  };
  if (!USI_TWI_Start_Transceiver_With_Data(usi_send, 4))
    return false;

  return true;
}

reg16_t _mcp47feb_read_reg(uint8_t addr) {
  unsigned char usi_send[3] = {
    TWI_WRITE(MCP47FEB_ADDR),
    (addr << 3) | MCP47FEB_CMD_READ,
    0x0,
  };
  reg16_t value = {0};

  if (!USI_TWI_Start_Transceiver_With_Data(usi_send, 2))
    return value;

  usi_send[0] = TWI_READ(MCP47FEB_ADDR);
  USI_TWI_Start_Transceiver_With_Data(usi_send, 3);
  value.bytes[1] = usi_send[1];
  value.bytes[0] = usi_send[2];

  return value;
}

bool _mcp47feb_sysconfig_bit_set(uint8_t addr, bool set) {
  unsigned char usi_send[2] = {
    TWI_WRITE(MCP47FEB_ADDR),
    (addr << 3),
  };

  if (set)
    usi_send[1] |= MCP47FEB_CMD_CONF_BIT_ENABLE;
  else
    usi_send[1] |= MCP47FEB_CMD_CONF_BIT_DISABLE;

  if (!USI_TWI_Start_Transceiver_With_Data(usi_send, 2))
    return false;

  return true;
}

//bool mcp47feb_set_addr() {
//  unsigned char usi_send[6] = {
//    TWI_WRITE(0x60),
//    (MCP47FEB_SYSCONFIG_BIT_SALCK << 3) | MCP47FEB_CMD_CONF_BIT_DISABLE,
//    (MCP47FEB_REG_NV_GSADR << 3) | MCP47FEB_CMD_WRITE,
//    0x00,
//    0x61,
//    (MCP47FEB_SYSCONFIG_BIT_SALCK << 3) | MCP47FEB_CMD_CONF_BIT_ENABLE,
//  };
//  if (!USI_TWI_Start_Transceiver_With_Data(usi_send, 6))
//    return false;
//
//  return true;
//  //return _mcp47feb_sysconfig_bit_set(MCP47FEB_SYSCONFIG_BIT_SALCK, false) &&
//  //       _mcp47feb_write_reg(MCP47FEB_REG_NV_GSADR, 0x61) &&
//  //       _mcp47feb_sysconfig_bit_set(MCP47FEB_SYSCONFIG_BIT_SALCK, true);
//}

bool mcp47feb_set_gain(bool val0, bool val1, bool nv) {
  reg16_t val = {0};
  val.word = (val0 << 8) | (val1 << 9);
  return _mcp47feb_write_reg(MCP47FEB_REG_VOL_GSTAT | (nv << 1), val);
}

bool mcp47feb_set_vref(uint8_t val0, uint8_t val1, bool nv) {
  reg16_t val = {0};
  val.word = val0 | (val1 << 2);
  return _mcp47feb_write_reg(MCP47FEB_REG_VOL_VREF | (nv << 1), val);
}

bool mcp47feb_set_power(uint8_t val0, uint8_t val1, bool nv) {
  reg16_t val = {0};
  val.word = val0 | (val1 << 2);
  return _mcp47feb_write_reg(MCP47FEB_REG_VOL_PDOWN | (nv << 1), val);
}

bool mcp47feb_set_output(uint8_t channel, reg16_t val, bool nv) {
  return _mcp47feb_write_reg(channel | (nv << 1), val);
}

reg16_t mcp47feb_get_output(uint8_t channel, bool nv) {
  return _mcp47feb_read_reg(channel | (nv << 1));
}

