#include "i2c_communication.h"

// Initialize I2C (TWI)
void i2c_init()
{
    // Set bit rate to 100kHz
    TWBR = 72;
    TWSR &= ~(1 << TWPS0) & ~(1 << TWPS1); // Prescaler = 1
}

// Start I2C transmission
void i2c_start()
{
    TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);
    while (!(TWCR & (1 << TWINT)))
        ;
}

// Stop I2C transmission
void i2c_stop()
{
    TWCR = (1 << TWINT) | (1 << TWSTO) | (1 << TWEN);
    _delay_us(50); // Give time for stop condition to complete
}

// Write data to I2C
void i2c_write(uint8_t data)
{
    TWDR = data;
    TWCR = (1 << TWINT) | (1 << TWEN);
    while (!(TWCR & (1 << TWINT)))
        ; // Waits until writing is complete.
}

// Read data from I2C
uint8_t i2c_read(bool ack)
{
    TWCR = (1 << TWINT) | (ack ? (1 << TWEA) : 0) | (1 << TWEN);
    while (!(TWCR & (1 << TWINT)))
        ; // Waits until writing is complete.
    return TWDR;
}