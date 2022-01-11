#----------------------------------------------------------------------------
# AVR-GCC Makefile
# GBS8200/8220 custom settings for CGA-280p/50Hz to VGA-1280x1024
# MCU=ATTINY85 or any tiny device with at least 2K of flash
# ---------------------------------------------------------------------------


TARGET     = gbs_control
DEVICE     = attiny25
SOURCE     = main.c i2c.c

PROGRAMMER = usbasp
AVRDUDE = avrdude
CC = avr-gcc
OBJCOPY = avr-objcopy
SIZE = avr-size
RM = rm -f


CFLAGS = -DF_CPU=8000000UL -std=gnu99 -Wall -Os -mmcu=$(DEVICE) \
          -funsigned-char -funsigned-bitfields -fpack-struct -fshort-enums


all: $(TARGET).hex

flash:
	$(AVRDUDE) -c $(PROGRAMMER) -p $(DEVICE) -U flash:w:$(TARGET).hex:i

minipro:
	minipro -zp $(DEVICE)@dip8 -w $(TARGET).hex

fuse:
	$(AVRDUDE) -c $(PROGRAMMER) -p $(DEVICE) -U lfuse:w:0xe2:m -U hfuse:w:0x5f:m -U efuse:w:0xff:m

clean:
	$(RM) $(TARGET).hex $(TARGET).elf
	
.PHONY: flash fuse clean


$(TARGET).elf:
	$(CC) $(CFLAGS) -o $(TARGET).elf $(SOURCE)

$(TARGET).hex: $(TARGET).elf
	$(RM) $(TARGET).hex
	$(OBJCOPY) -j .text -j .data -O ihex $(TARGET).elf $(TARGET).hex
	$(SIZE) --format=avr --mcu=$(DEVICE) $(TARGET).elf
