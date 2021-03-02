
char promptc(PGM_P str);
unsigned char prompti(PGM_P str);
reg16_t promptr(PGM_P str);

void read_fbdac(void);
void write_fbdac(unsigned char val);

void read_currmon(void);

#ifdef CARD_B02
void write_odac_config(void);
#endif

void read_vout(void);

void buck_enable(void);
void buck_disable(void);

// 74.3 ohms
