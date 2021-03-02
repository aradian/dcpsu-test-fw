
#ifndef _COMMON_H
#define _COMMON_H 1

#include <stdint.h>

typedef union u_reg_16 {
  uint16_t word;
  uint8_t bytes[2];
} reg16_t;

#endif

