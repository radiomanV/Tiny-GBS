/*
* GBS8200/8220 custom settings for 
* Progresive 288p/50Hz CGA RGB input
* Output is fixed at 1280x1024/60Hz (5:4)
* CPU: ATTINY25/45/85
* Created: October 10, 2021
*  Author: radiomanV
*/


#include <avr/pgmspace.h>
#include <string.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#include "i2c.h"
#include "reg.h"

#define I2C_ADDRESS 0x17

uint8_t write_bytes(uint8_t reg, uint8_t *data, int size) {
  return i2c_send(I2C_ADDRESS, reg, data, size);
}

uint8_t write_byte(uint8_t reg, uint8_t data) {
  return write_bytes(reg, &data, 1);
}

uint8_t read_bytes(uint8_t reg, uint8_t *data, int size) {
  return i2c_receive(I2C_ADDRESS, reg, data, size);
}


uint8_t write_gbs() {
  uint8_t b[16], i, seg, reg, banks;
  int idx = 0;
  while (idx + 3 < sizeof(reg_288p)) {
    seg = pgm_read_byte(reg_288p + idx++);   // segment
    reg = pgm_read_byte(reg_288p + idx++);   // start register
    banks = pgm_read_byte(reg_288p + idx++); // no. of banks to write
    if (banks * 16 + idx > sizeof(reg_288p))
      break;                   // break if not enough data
    if (write_byte(0xf0, seg)) // Select segment
      return 1;
    for (i = 0; i < banks; i++, idx += 16) { // write each bank
      memcpy_P(b, reg_288p + idx, 16);       // each bank is 16 bytes long
      if (write_bytes(i * 16 + reg, b, 16))
        return 1;
    }
  }
  // Set the S5_11 bit 7 to update PLLAD
  if (write_byte(0xf0, 0x05) || write_byte(0x11, 0x90))
    return 1;
  return 0;
}


int main(void) {
  int ret = 1;
  //uint8_t status;
  i2c_init();
  sei();
  for (;;) {
    if (ret) {
      ret = write_gbs();
    }
    _delay_ms(20);
  }
}
