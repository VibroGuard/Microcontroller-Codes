#ifndef I2C_COMMUNICATION_H
#define I2C_COMMUNICATION_H

#include <avr/io.h>
#include <util/delay.h>
#include <stdint.h>

// Function declarations related I2C protocol.
void i2c_init();
void i2c_start();
void i2c_stop();
void i2c_write(uint8_t data);
uint8_t i2c_read(bool ack);

#endif