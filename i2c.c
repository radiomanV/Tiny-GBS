/*
* BitBannging I2C for ATTINy25/45/85 
* Created: October 10, 2021
*  Author: radiomanV
*/

#include <avr/io.h>
#include <inttypes.h>
#include <util/delay.h>
#include "i2c.h"

#define SCL 1
#define SDA 0

#define SDA_LOW (DDRB |= (1 << SDA))
#define SDA_HIGH (DDRB &= ~(1 << SDA))
#define SCL_LOW (DDRB |= (1 << SCL))
#define SCL_STATE (PINB & (1 << SCL))
#define SCL_HIGH {DDRB &= ~(1 << SCL); while (!SCL_STATE);}
#define SDA_STATE (PINB & (1 << SDA))


static void i2c_start() {
  SCL_HIGH; // needed for i2c restart
  _delay_us(5);
  SDA_LOW;
  _delay_us(4);
  SCL_LOW;
  _delay_us(5);
  return;
}

static void i2c_stop() {
  SDA_LOW;
  _delay_us(5);
  SCL_HIGH;
  _delay_us(4);
  SDA_HIGH;
  _delay_us(5);
  return;
}

static uint8_t i2c_write(uint8_t data) {
  for (uint8_t i = 0; i < 8; i++) {
    data & 0x80 ? SDA_HIGH : SDA_LOW;
    _delay_us(1);
    SCL_HIGH;
    _delay_us(4);
    SCL_LOW;
    _delay_us(5);
    data <<= 1;
  }

  SDA_HIGH;
  _delay_us(1);
  SCL_HIGH;
  _delay_us(3);
  if (SDA_STATE)
    return 1;
  SCL_LOW;
  _delay_us(15);
  return 0;
}

static uint8_t i2c_read(uint8_t ack) {
  uint8_t b = 0;
  for (uint8_t i = 0; i < 8; i++) {
    b <<= 1;
    if (SDA_STATE)
      b |= 0x01;
    SCL_HIGH;
    _delay_us(4);
    SCL_LOW;
    _delay_us(5);
  }

  if (ack)// ACK/NACK
    SDA_LOW; 
  _delay_us(1);
  SCL_HIGH;
  _delay_us(3);
  SCL_LOW;
  _delay_us(7);
  SDA_HIGH;
  _delay_us(1);
  return b;
}

void i2c_init() {
  SDA_HIGH;
  SCL_HIGH;
  PORTB &= ~(1 << SDA);
  PORTB &= ~(1 << SCL);
}

uint8_t i2c_send(uint8_t address, uint8_t reg, uint8_t *data, int size) {
  uint8_t ret = 0;
  address <<= 1; // write access
  i2c_start();
  if(i2c_write(address) || i2c_write(reg)){// Exit if NACK
    i2c_stop();
    return 1;
  }
  for (uint8_t i = 0; i < size; i++) {
    if (i2c_write(data[i])) { // Exit if NACK
      ret = 1;
      break;
    }
  }
  i2c_stop();
  return ret;
}

uint8_t i2c_receive(uint8_t address, uint8_t reg, uint8_t *data, int size) {
  uint8_t ret = 1;
  address <<= 1; // write access
  i2c_start();
  if(i2c_write(address) || i2c_write(reg)){ // exit if NACK
    goto err;
  }
  address |= 0x01; // read access
  i2c_start(); // repeated start
  if(i2c_write(address)){ // Exit if NACK
    goto err;
  }
  for (uint8_t i = 0; i < size; i++) {
    data[i] = i2c_read(i < size - 1);
  }
  ret = 0;
err:
  i2c_stop();
  return ret;
}

