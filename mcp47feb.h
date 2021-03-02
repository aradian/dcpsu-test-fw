
#include <stdbool.h>

#include "common.h"

#define MCP47FEB_ADDR 0x60

#define MCP47FEB_CMD_WRITE             0b00
#define MCP47FEB_CMD_READ              0b11
#define MCP47FEB_CMD_CONF_BIT_ENABLE   0b10
#define MCP47FEB_CMD_CONF_BIT_DISABLE  0b01

#define MCP47FEB_REG_VOL_DAC0  0x00
#define MCP47FEB_REG_VOL_DAC1  0x01
#define MCP47FEB_REG_VOL_VREF  0x08
#define MCP47FEB_REG_VOL_PDOWN 0x09
#define MCP47FEB_REG_VOL_GSTAT 0x0a
#define MCP47FEB_REG_VOL_WLCK  0x0b

#define MCP47FEB_REG_NV_DAC0  0x10
#define MCP47FEB_REG_NV_DAC1  0x11
#define MCP47FEB_REG_NV_VREF  0x18
#define MCP47FEB_REG_NV_PDOWN 0x19
#define MCP47FEB_REG_NV_GSADR 0x1a

#define MCP47FEB_VREF_VDD        0b00
#define MCP47FEB_VREF_BANDGAP    0b01
#define MCP47FEB_VREF_VREF_UNBUF 0b10
#define MCP47FEB_VREF_VREF_BUF   0b11

#define MCP47FEB_POW_NORMAL     0b00
#define MCP47FEB_POW_DOWN_1K    0b01
#define MCP47FEB_POW_DOWN_100K  0b10
#define MCP47FEB_POW_DOWN_OPEN  0b11

#define MCP47FEB_GAIN_1X 0
#define MCP47FEB_GAIN_2X 1

#define MCP47FEB_GSTAT_DAC1_GAIN 0b0000001000000000
#define MCP47FEB_GSTAT_DAC0_GAIN 0b0000000100000000
#define MCP47FEB_GSTAT_POR       0b0000000010000000
#define MCP47FEB_GSTAT_EEWA      0b0000000001000000

#define MCP47FEB_GSADR_DAC1_GAIN 0b0000001000000000
#define MCP47FEB_GSADR_DAC0_GAIN 0b0000000100000000
#define MCP47FEB_GSADR_ADLCK     0b0000000010000000
#define MCP47FEB_GSADR_ADDR_MASK 0b0000000001111111

#define MCP47FEB_WLCK_CL 0b01
#define MCP47FEB_WLCK_DL 0b10

#define MCP47FEB_SYSCONFIG_BIT_CL0   0x00
#define MCP47FEB_SYSCONFIG_BIT_CL1   0x01
#define MCP47FEB_SYSCONFIG_BIT_DL0   0x10
#define MCP47FEB_SYSCONFIG_BIT_DL1   0x11
#define MCP47FEB_SYSCONFIG_BIT_SALCK 0x1a


bool _mcp47feb_write_reg(uint8_t, reg16_t);
reg16_t _mcp47feb_read_reg(uint8_t);
bool _mcp47feb_sysconfig_bit_set(uint8_t, bool);

//bool mcp47feb_set_addr();

bool mcp47feb_set_gain(bool, bool, bool);
bool mcp47feb_set_vref(uint8_t, uint8_t, bool);
bool mcp47feb_set_power(uint8_t, uint8_t, bool);
bool mcp47feb_set_output(uint8_t, reg16_t, bool);
reg16_t mcp47feb_get_output(uint8_t, bool);

