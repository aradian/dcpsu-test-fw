
#include "defines.h"

#include <ctype.h>
#include <stdint.h>
#include <stdio.h>

#include <avr/io.h>
#include <avr/pgmspace.h>
#include <util/delay.h>

#include "uart.h"
#include "USI_TWI_Master.h"

#include "catb.h"

//#define ADDR_DIGIPOT 0b0101111
#define ADDR_FBDAC   0b1100000
#define ADDR_CURRMON 0b1000000

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

#ifdef CARD_A03
const char p_build[] PROGMEM = "Card A v0.3\n";
#endif
#ifdef CARD_B02
const char p_build[] PROGMEM = "Card B v0.2\n";
#endif
const char p_start[] PROGMEM = "start v0.4\n";
const char p_conf_fbdac[] PROGMEM = "configured fbdac\n";
const char p_conf_currmon[] PROGMEM = "configured currmon\n";
const char p_cmd[] PROGMEM = "cmd: ";
const char p_read[] PROGMEM = "read: ";
const char p_cm_busv[] PROGMEM = "bus V: 0x%02x%02x %d * 0.004\n";
const char p_cm_shuntv[] PROGMEM = "shunt V: 0x%02x%02x %d * 0.00001\n";
const char p_cm_cur[] PROGMEM = "current: 0x%02x%02x %d * 0.00012207\n";
const char p_cm_pow[] PROGMEM = "power: 0x%02x%02x %d * 0.00012207 * 20\n";
const char p_write[] PROGMEM = "write: ";
const char p_prompt_fbdac[] PROGMEM = "fbdac val: ";
const char p_led[] PROGMEM = "led: ";
const char p_buck[] PROGMEM = "buck: ";
const char p_fpwm_disable[] PROGMEM = "Disable force-PWM";
const char p_fpwm_enable[] PROGMEM = "Enable force-PWM";
const char p_invalid[] PROGMEM = "invalid\n";
const char p_broken[] PROGMEM = "state broken\n";
const char p_end[] PROGMEM = "end";
const char p_usi_failed[] PROGMEM = "failed: %d\n";

#define PRINT_USI_ERROR printf_P(p_usi_failed, USI_TWI_Get_State_Info());

FILE uart_str = FDEV_SETUP_STREAM(uart_putchar, uart_getchar, _FDEV_SETUP_RW);

//                        R-RPPBBBBSSSSMMM
#define CURRMON_CONFIG  0b0011100000011111

//static void delay_1s(void) {
//  uint8_t i;
//
//  for (i = 0; i < 100; i++)
//    _delay_ms(10);
//}

int main(void) {
  LED_UNSET_READY();
  LED_UNSET_POWER();

  PORTB &= ~_BV(PIN_POWER_ENABLE);
  PORTB &= ~_BV(PIN_FORCE_PWM);
  DDRB |= _BV(DDB1) | _BV(DDB3) | _BV(DDB5);

  ADC_INIT();
  uart_init();
  USI_TWI_Master_Initialise();

  stderr = stdout = stdin = &uart_str;

  printf_P(p_build);
  printf_P(p_start);
  write_fbdac(0xff); // configure FBDAC
  printf_P(p_conf_fbdac);
  cal_currmon(); // configure currmon
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
            //read_digipot();
            read_fbdac();
            break;
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
            //write_digipot(prompti("digipot val: "));
            write_fbdac(prompti(p_prompt_fbdac));
            break;
          case 'c':
            break;
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

//void read_digipot() {
//  unsigned char usi_send[2] = {
//    TWI_READ(ADDR_DIGIPOT),
//    0x00,
//  };
//  fputs("read digipot: ", stdout);
//  if (USI_TWI_Start_Transceiver_With_Data(usi_send, 2))
//    printf("(addr 0x%x) 0x%x\n", usi_send[0], usi_send[1]);
//  else
//    printf("failed: %d\n",  USI_TWI_Get_State_Info());
//}
//
//void write_digipot(unsigned char val) {
//  unsigned char usi_send[3] = {
//    TWI_WRITE(ADDR_DIGIPOT),
//    val,
//    0x00,
//  };
//
//  printf("set digipot: 0x%x\n", usi_send[1]);
//  //usi_send[1] = 10; //prompti("Set value: ");
//  if (! USI_TWI_Start_Transceiver_With_Data(usi_send, 2))
//    printf("failed: %d\n", USI_TWI_Get_State_Info());
//  //  printf("after write: 0x%x 0x%x 0x%x\n", usi_send[0], usi_send[1], usi_send[2]);
//  //else
//  //  printf("failed: %d\n", USI_TWI_Get_State_Info());
//}

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

