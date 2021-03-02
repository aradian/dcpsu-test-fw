
#include "defines.h"

#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <math.h>

#include <avr/io.h>
#include <avr/pgmspace.h>
#include <util/delay.h>

#include "uart.h"
#include "USI_TWI_Master.h"

#include "ina219.h"
#include "card_test_fw.h"

#define ADDR_FBDAC   0b1100000

#define S_CMD 0
#define S_RD  1
#define S_WR  2
#define S_BK  3


#ifdef CARD_A03
#define PIN_VOUT PIN_B6
#define PIN_POWER_ENABLE PORTB1
#define PIN_FORCE_PWM PORTB3
#define PIN_LED1 PORTB4
#define PIN_LED2 PORTB5

#define LED_SET_READY()  \
        PORTB |= _BV(PIN_LED1); \
        DDRB |= _BV(PIN_LED1);
#define LED_UNSET_READY()  \
        DDRB &= ~_BV(PIN_LED1);
#define LED_SET_POWER()  \
        PORTB |= _BV(PIN_LED2);
#define LED_UNSET_POWER()  \
        PORTB &= ~_BV(PIN_LED2);

// configure ADC for pin PB6/A9
#define ADC_INIT() \
	ADMUX = 0b00001001; \
	DIDR1 |= _BV(ADC9D); \
	ADCSRA |= _BV(ADPS2) | _BV(ADPS1); \
	ADCSRA |= _BV(ADEN);
#endif


#ifdef CARD_B02
#define PIN_VOUT PIN_B5
#define PIN_POWER_ENABLE PORTB1
#define PIN_FORCE_PWM PORTB3
#define PIN_CLIM PIN_B6
// TODO: configure aref on pin A7

// configure ADC for pin PB5/A8
#define ADC_INIT() \
	ADMUX = 0b00001000; \
	DIDR1 |= _BV(ADC8D); \
	ADCSRA |= _BV(ADPS2) | _BV(ADPS1); \
	ADCSRA |= _BV(ADEN);

#include "cardb_utils.h"
#endif

#define PRINT_USI_ERROR printf_P(p_usi_failed, USI_TWI_Get_State_Info());

#ifdef CARD_A03
const char p_build[] PROGMEM = "Card A v0.3\n";
#endif
#ifdef CARD_B02
const char p_build[] PROGMEM = "Card B v0.2\n";
#endif
const char p_start[] PROGMEM = "start v0.6\n";
const char p_conf_fbdac[] PROGMEM = "configured fbdac\n";
const char p_conf_currmon[] PROGMEM = "configured currmon\n";
const char p_cmd[] PROGMEM = "cmd: ";
const char p_read[] PROGMEM = "read: ";
const char p_cm_busv[] PROGMEM = "bus V: 0x%04x %d * 4mV = %dmV\n";
const char p_cm_shuntv[] PROGMEM = "shunt V: 0x%04x %d * 10uV = %dmV\n";
const char p_cm_cur[] PROGMEM = "current: 0x%04x %d * 0.00012207 = %dmA\n";
const char p_cm_pow[] PROGMEM = "power: 0x%04x %d * 0.00012207 * 20 = %dmW\n";
const char p_write[] PROGMEM = "write: ";
const char p_prompt_fbdac[] PROGMEM = "fbdac val: ";
const char p_led[] PROGMEM = "led: ";
const char p_buck[] PROGMEM = "buck: ";
const char p_fpwm_disable[] PROGMEM = "Disable force-PWM";
const char p_fpwm_enable[] PROGMEM = "Enable force-PWM";
const char p_success[] PROGMEM = "success\n";
const char p_failure[] PROGMEM = "failed\n";
const char p_invalid[] PROGMEM = "invalid\n";
const char p_broken[] PROGMEM = "state broken\n";
const char p_end[] PROGMEM = "end";
const char p_usi_failed[] PROGMEM = "failed: %d\n";
const char p_odac_choose[] PROGMEM = "odac channel: ";
const char p_odac_param[] PROGMEM = "param: ";
const char p_odac_value[] PROGMEM = "value: ";

FILE uart_str = FDEV_SETUP_STREAM(uart_putchar, uart_getchar, _FDEV_SETUP_RW);

int main(void) {
  LED_UNSET_READY();
  LED_UNSET_POWER();

  PORTB &= ~_BV(PIN_POWER_ENABLE);
  PORTB &= ~_BV(PIN_FORCE_PWM);
  DDRB |= _BV(DDB1) | _BV(DDB3) | _BV(DDB5);
  DDRB &= ~_BV(DDB6); // CLIM input

  ADC_INIT();
  uart_init();
  USI_TWI_Master_Initialise();

  stderr = stdout = stdin = &uart_str;

  printf_P(p_build);
  printf_P(p_start);

#ifdef CARD_A03
  // configure FBDAC
  write_fbdac(0xff);
  printf_P(p_conf_fbdac);
#endif

  // configure currmon
  ina219_set_config((ina219_config_t)(reg16_t)(uint16_t)INA219_CONFIG);
  ina219_set_cal();
  printf_P(p_conf_currmon);

  LED_SET_READY();

  unsigned char state = S_CMD;

  while (1) {

    switch (state) {
      case S_CMD:
        switch (promptc(p_cmd)) {
          case 'r':
            state = S_RD;
            break;
          case 'w':
            state = S_WR;
            break;
          case 'b':
            state = S_BK;
            break;
          default:
            fputs_P(p_invalid, stderr);
        }
        break;

      case S_RD:
        switch (promptc(p_read)) {
          case 'd':
            read_fbdac();
            break;
#ifdef CARD_B02
          case 'D':
            switch (promptc(p_odac_choose)) {
              case 'v':
                printf("0x%x\n", mcp47feb_get_output(0, false).word);
                break;
              case 'V':
                printf("0x%x\n", mcp47feb_get_output(0, true).word);
                break;
              case 'i':
                printf("0x%x\n", mcp47feb_get_output(1, false).word);
                break;
              case 'I':
                printf("0x%x\n", mcp47feb_get_output(1, true).word);
                break;
              default:
                fputs_P(p_invalid, stderr);
            }
            break;
#endif
          case 'c':
            read_currmon();
            break;
          case 'v':
            read_vout();
            break;
          case '-':
            state = S_CMD;
            break;
          default:
            fputs_P(p_invalid, stderr);
        }
        break;

      case S_WR:
        switch (promptc(p_write)) {
          case 'd':
            write_fbdac(prompti(p_prompt_fbdac));
            break;
#ifdef CARD_B02
          case 'D':
            switch (promptc(p_odac_choose)) {
              case 'v':
                if (mcp47feb_set_output(0, promptr(p_odac_value), false))
                  fputs_P(p_success, stdout);
                else
                  fputs_P(p_failure, stdout);
                break;
              case 'V':
                if (mcp47feb_set_output(0, promptr(p_odac_value), true))
                  fputs_P(p_success, stdout);
                else
                  fputs_P(p_failure, stdout);
                break;
              case 'i':
                if (mcp47feb_set_output(1, promptr(p_odac_value), false))
                  fputs_P(p_success, stdout);
                else
                  fputs_P(p_failure, stdout);
                break;
              case 'I':
                if (mcp47feb_set_output(1, promptr(p_odac_value), true))
                  fputs_P(p_success, stdout);
                else
                  fputs_P(p_failure, stdout);
                break;
              default:
                fputs_P(p_invalid, stderr);
            }
            break;
          case 'C':
            write_odac_config();
            break;
#endif
          case 'l':
            switch (promptc(p_led)) {
              case '1':
                LED_SET_POWER();
                break;
              case '0':
                LED_UNSET_POWER();
                break;
            }
            break;
#ifdef CARD_B02
          //case 'A':
          //  if (mcp47feb_set_addr())
          //    puts("success\n");
          //  else
          //    puts("failed\n");
          //  break;
#endif
          case '-':
            state = S_CMD;
            break;
          default:
            fputs_P(p_invalid, stderr);
        }
        break;

      case S_BK:
        switch (promptc(p_buck)) {
          case 'e':
            buck_enable();
            break;
          case 'd':
            buck_disable();
            break;
          case 'f':
            if (PORTB & _BV(PIN_FORCE_PWM)) {
              puts_P(p_fpwm_disable);
              PORTB &= ~_BV(PIN_FORCE_PWM);
            } else {
              puts_P(p_fpwm_enable);
              PORTB |= _BV(PIN_FORCE_PWM);
            }
            break;
          case '-':
            state = S_CMD;
            break;
          default:
            fputs_P(p_invalid, stderr);
        }
        break;

      default:
        fputs_P(p_broken, stderr);
    }
  }

  puts_P(p_end);

  return 0;
}

char promptc(PGM_P str) {
  char buf[3] = "\0\0\0";
  char *ret = NULL;
  fputs_P(str, stdout);
  ret = fgets(buf, 3, stdin);
  if (ret == NULL) {
    printf("got null; ferror: %d\n", ferror(stdin));
    clearerr(stdin);
  }
  return buf[0];
}

unsigned char prompti(PGM_P str) {
  char res;
  unsigned char resp = 0;
  fputs_P(str, stdout);
  res = scanf("%hhi", &resp);
  if (!res)
    puts_P(p_invalid);
  else
    fgetc(stdin);
  return resp;
}

reg16_t promptr(PGM_P str) {
  char res;
  reg16_t resp = {0};
  fputs_P(str, stdout);
  res = scanf("%i", &resp.word);
  if (!res)
    puts_P(p_invalid);
  else
    fgetc(stdin);
  return resp;
}

void read_fbdac() {
  unsigned char usi_send[3] = {
    TWI_READ(ADDR_FBDAC),
    0x00,
    0x00,
  };
  fputs("read fbdac: ", stdout);
  if (USI_TWI_Start_Transceiver_With_Data(usi_send, 3))
    printf("conf:0x%x val:0x%x\n", usi_send[1], usi_send[2]);
  else
    PRINT_USI_ERROR;
}

void write_fbdac(unsigned char val) {
  unsigned char usi_send[4] = {
    TWI_WRITE(ADDR_FBDAC),
    (0b011 << 5) | // write all memory
    (0b00 << 3)  | // ref voltage select: VDD
    (0b00 << 1)  | // power mode: normal
    (0b0),         // gain: 1
    val,
    0x00,
  };

  //printf("set fbdac: 0x%x\n", usi_send[2]);
  if (! USI_TWI_Start_Transceiver_With_Data(usi_send, 4))
    PRINT_USI_ERROR;
}

void read_currmon() {
  ina219_config_t config;
  //ina219_data_t values;
  reg16_t value = {0};
  float temp_f = 0;
  int16_t temp_i = 0;
  //uint16_t temp_ui = 0;
  printf("Read currmon: 0x%x\n", INA219_ADDR);

  // config
  config = ina219_get_config();
  printf("config: 0x%x\n", config.reg.word);

  // measurement values
  //values = ina219_read();

  // bus V
  value = ina219_read_data_reg(INA219_REG_BUSV);
  value.word = value.word >> 3;
  temp_f = value.word * 0.004f;
  temp_i = temp_f * 1000;
  printf_P(p_cm_busv, value.word, value.word, temp_i);

  // shunt V
  value = ina219_read_data_reg(INA219_REG_SHUNTV);
  temp_f = value.word * 0.00001f;
  temp_i = temp_f * 1000;
  printf_P(p_cm_shuntv, value.word, value.word, temp_i);

  // current
  value = ina219_read_data_reg(INA219_REG_CURR);
  temp_f = value.word * INA219_LSB_CUR;
  temp_i = temp_f * 1000;
  printf_P(p_cm_cur, value.word, value.word, temp_i);

  // power
  value = ina219_read_data_reg(INA219_REG_POWER);
  temp_f = value.word * INA219_LSB_POW;
  temp_i = temp_f * 1000;
  printf_P(p_cm_pow, value.word, value.word, temp_i);
}

#ifdef CARD_B02
void write_odac_config() {
  unsigned char param;
  uint8_t val0, val1;
  bool result;

  param = promptc(p_odac_param);
  val0 = prompti(p_odac_value);
  val1 = prompti(p_odac_value);

  switch (param) {
    case 'v':
      result = mcp47feb_set_vref(val0, val1, false);
      break;
    case 'V':
      result = mcp47feb_set_vref(val0, val1, true);
      break;
    case 'g':
      result = mcp47feb_set_gain(val0, val1, false);
      break;
    case 'G':
      result = mcp47feb_set_gain(val0, val1, true);
      break;
    case 'p':
      result = mcp47feb_set_power(val0, val1, false);
      break;
    case 'P':
      result = mcp47feb_set_power(val0, val1, true);
      break;
    default:
      fputs_P(p_invalid, stderr);
  }

  if (result)
    fputs_P(p_success, stdout);
  else
    fputs_P(p_failure, stdout);
}
#endif

void read_vout() {
  uint16_t res = 0;
  uint16_t count = 0;
  fputs("vout: ", stdout);
  ADCSRA |= _BV(ADSC);
  while (ADCSRA & _BV(ADSC)) {
    count++;
    //_delay_ms(1);
  }
  res = ADCL;
  res += ADCH * 0x100;
  printf("(loop %d) %d/1023\n", count, res);
}

void buck_enable() {
  puts("Enable buck");
  PORTB |= _BV(PIN_POWER_ENABLE);
  LED_SET_POWER();
}

void buck_disable() {
  puts("Disable buck");
  PORTB &= ~_BV(PIN_POWER_ENABLE);
  LED_UNSET_POWER();
}

