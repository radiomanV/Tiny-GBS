#ifndef I2C_H
#define I2C_H

#include <inttypes.h>

void i2c_init();
uint8_t i2c_send(uint8_t, uint8_t, uint8_t*, int);
uint8_t i2c_receive(uint8_t, uint8_t, uint8_t*, int);
#endif

