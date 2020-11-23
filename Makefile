SHELL = /bin/sh

CC = avr-gcc
CFLAGS = -mmcu=attiny87 -Os -DF_CPU=8000000 #-Wl,-u,vfprintf -lprintf_flt -lm

.SUFFIXES:
.SUFFIXES: .c .o .hex

objects := $(patsubst %.c,%.o,$(wildcard *.c))


card_test_fw.hex: $(objects)
	avr-gcc ${CFLAGS} -o card_test_fw.elf $(objects)
	avr-objcopy -O ihex card_test_fw.elf card_test_fw.hex
#hex:
#	avr-gcc -mmcu=attiny87 -Os -DF_CPU=8000000 -c catb.c
#	avr-gcc -mmcu=attiny87 -DF_CPU=8000000 catb.o -o catb.elf
#	avr-objcopy -O ihex catb.elf catb.hex

hexa: CFLAGS += -DCARD_A03
hexa: card_test_fw.hex
hexb: CFLAGS += -DCARD_B02
hexb: card_test_fw.hex

flasha: CFLAGS += -DCARD_A03
flasha: card_test_fw.hex
	#sudo avrdude -c ft232r -P usb:ft0 -p attiny87 -Uflash:w:card_test_fw.hex:i
	sudo avrdude -c linuxgpio -p attiny87 -Uflash:w:card_test_fw.hex:i

flashb: CFLAGS += -DCARD_B02
flashb: card_test_fw.hex
	#sudo avrdude -c ft232r -P usb:ft0 -p attiny87 -Uflash:w:card_test_fw.hex:i
	sudo avrdude -c linuxgpio -p attiny87 -Uflash:w:card_test_fw.hex:i

.PHONY: clean
clean:
	rm -f $(objects) *.elf *.hex

