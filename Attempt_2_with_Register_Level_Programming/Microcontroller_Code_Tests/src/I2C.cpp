/*
  I2C.h   - I2C library
  Copyright (c) 2011-2012 Wayne Truchsess.  All right reserved.

  Originally written by: Wayne Truchsess
  Modified by: Linuka Ratnayake

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

*/

#include <inttypes.h>
#include "I2C.h"
#include "auxiliary_functions.h"

uint8_t I2C::bytesAvailable = 0;
uint8_t I2C::bufferIndex = 0;
uint8_t I2C::totalBytes = 0;
uint16_t I2C::timeOutDelay = 0;

I2C::I2C()
{
}

////////////// Public Methods ////////////////////////////////////////

/*
 *  Description:
 *      Enables the I2C hardware
 *  Parameters:
 *      none
 *  Returns:
 *      none
 */
void I2C::begin()
{
  pullup(1);

  // initialize twi prescaler and bit rate
  cbi(TWSR, TWPS0);
  cbi(TWSR, TWPS1);
  TWBR = ((F_CPU / 100000) - 16) / 2;
  // enable twi module and acks
  TWCR = _BV(TWEN) | _BV(TWEA);
}

/*
 *  Description:
 *      Disables the I2C hardware
 *  Parameters:
 *      none
 *  Returns:
 *      none
 */
void I2C::end()
{
  TWCR = 0;
}

/*
 *  Description:
 *      Allows the user to program a time out limit to prevent and recover from
 *      I2C bus lockups. I2C bus lockups have a tendency to freeze a program
 *      which typically requires a power cycle to restart your program. This
 *      allows the user to define a time out in which the I2C will release
 *      itself and reinitialize and continue on with the next function. Setting
 *      the value to zero will disable the function. On a side note, be careful
 *      with setting too low a value because some devices support clock
 *      stretching which can increase the time before an acknowledgment is sent
 *      which could be misconstrued as a lockup.
 *
 *      If a lock up occurs the returned parameters from Read and/or Writes will
 *      contain a 1.
 *
 *  Parameters:
 *      timeOut - uint16_t
 *          The amount of time to wait before timing out. Can range from
 *          0 - 65535 milliseconds. If it's set to 0 it will be disabled.
 *  Returns:
 *      none
 */
void I2C::timeOut(uint16_t _timeOut)
{
  timeOutDelay = _timeOut;
}

/*
 *  Description:
 *      Enables/disables internal pullup resistors
 *  Parameters:
 *      activate - Boolean
 *          True: Enable internal pullup resistors (default)
 *          False: Disable internal pullup resistors
 *  Returns:
 *      none
 */
void I2C::pullup(uint8_t activate)
{
  if (activate)
  {
#if defined(__AVR_ATmega168__) || defined(__AVR_ATmega8__) || defined(__AVR_ATmega328P__) || defined(__AVR_ATmega328PB__)
    // activate internal pull-ups for twi
    // as per note from atmega8 manual pg167
    sbi(PORTC, 4);
    sbi(PORTC, 5);
#elif defined(__AVR_ATmega644__) || defined(__AVR_ATmega644P__)
    // activate internal pull-ups for twi
    // as per note from atmega644p manual pg108
    sbi(PORTC, 0);
    sbi(PORTC, 1);
#else
    // activate internal pull-ups for twi
    // as per note from atmega128 manual pg204
    sbi(PORTD, 0);
    sbi(PORTD, 1);
#endif
  }
  else
  {
#if defined(__AVR_ATmega168__) || defined(__AVR_ATmega8__) || defined(__AVR_ATmega328P__) || defined(__AVR_ATmega328PB__)
    // deactivate internal pull-ups for twi
    // as per note from atmega8 manual pg167
    cbi(PORTC, 4);
    cbi(PORTC, 5);
#elif defined(__AVR_ATmega644__) || defined(__AVR_ATmega644P__)
    // deactivate internal pull-ups for twi
    // as per note from atmega644p manual pg108
    cbi(PORTC, 0);
    cbi(PORTC, 1);
#else
    // deactivate internal pull-ups for twi
    // as per note from atmega128 manual pg204
    cbi(PORTD, 0);
    cbi(PORTD, 1);
#endif
  }
}

/*
 *  Description:
 *      Returns the first unread byte of the internal buffer.
 *  Parameters:
 *      none
 *  Returns:
 *      uint8_t
 *          First unread byte of the internal buffer
 */
uint8_t I2C::receive()
{
  bufferIndex = totalBytes - bytesAvailable;
  if (!bytesAvailable)
  {
    bufferIndex = 0;
    return (0);
  }
  bytesAvailable--;
  return (data[bufferIndex]);
}

/*
 *  Description:
 *      Initiate an I2C write operation, sending a single data byte. Typically
 *      used to send a single byte of data to a register address
 *
 *      NOTE: For devices with 16-bit register addresses use
 *      I2c.write16(address, registerAddress, data). It is identical except
 *      registerAddress is a uint16_t
 *  Parameters:
 *      address - uint8_t
 *          The 7 bit I2C slave address
 *      registerAddress - uint8_t
 *          Address of the register you wish to access (as per the datasheet)
 *      data - uint8_t
 *          A single byte of data to send
 *  Returns:
 *      uint8_t
 *          See "TRANSMISSION TIMEOUT RETURN VALUES" for return value meaning
 */
uint8_t I2C::write(uint8_t address, uint8_t registerAddress, uint8_t data)
{
  returnStatus = 0;
  returnStatus = _start();
  if (returnStatus)
  {
    return (returnStatus);
  }
  returnStatus = _sendAddress(SLA_W(address));
  if (returnStatus)
  {
    if (returnStatus == 1)
    {
      return (2);
    }
    return (returnStatus);
  }
  returnStatus = _sendByte(registerAddress);
  if (returnStatus)
  {
    if (returnStatus == 1)
    {
      return (3);
    }
    return (returnStatus);
  }
  returnStatus = _sendByte(data);
  if (returnStatus)
  {
    if (returnStatus == 1)
    {
      return (3);
    }
    return (returnStatus);
  }
  returnStatus = _stop();
  if (returnStatus)
  {
    if (returnStatus == 1)
    {
      return (7);
    }
    return (returnStatus);
  }
  return (returnStatus);
}

/*
 *  Description:
 *      Initiate a write operation to set the pointer to the registerAddress,
 *      then send a repeated start (not a stop then start) and store the number
 *      of bytes in an internal buffer. The 32 byte size restriction is imposed
 *      for this function. Data can be read out of the buffer using
 *      I2c.receive().
 *
 *      NOTE: For devices with 16-bit register addresses use
 *      I2c.read16(address, registerAddress, numberBytes). It is identical
 *      except registerAddress is a uint16_t
 *  Parameters:
 *      address - uint8_t
 *          The 7 bit I2C slave address
 *      registerAddress - uint8_t
 *          Starting register address to read data from
 *      numberBytes - uint8_t
 *          The number of bytes to be read
 *  Returns:
 *      uint8_t
 *          NOTE: Unlike the Wire library the read operation will not return the
 *          number of bytes read, instead it will return the error code which
 *          can be used for debugging.
 */
uint8_t I2C::read(uint8_t address, uint8_t registerAddress, uint8_t numberBytes)
{
  bytesAvailable = 0;
  bufferIndex = 0;
  numberBytes = numberBytes;
  if (numberBytes > MAX_BUFFER_SIZE)
  {
    numberBytes = MAX_BUFFER_SIZE;
  }
  if (numberBytes == 0)
  {
    numberBytes++;
  }
  nack = numberBytes - 1;
  returnStatus = 0;
  returnStatus = _start();
  if (returnStatus)
  {
    return (returnStatus);
  }
  returnStatus = _sendAddress(SLA_W(address));
  if (returnStatus)
  {
    if (returnStatus == 1)
    {
      return (2);
    }
    return (returnStatus);
  }
  returnStatus = _sendByte(registerAddress);
  if (returnStatus)
  {
    if (returnStatus == 1)
    {
      return (3);
    }
    return (returnStatus);
  }
  returnStatus = _start();
  if (returnStatus)
  {
    if (returnStatus == 1)
    {
      return (4);
    }
    return (returnStatus);
  }
  returnStatus = _sendAddress(SLA_R(address));
  if (returnStatus)
  {
    if (returnStatus == 1)
    {
      return (5);
    }
    return (returnStatus);
  }
  for (uint8_t i = 0; i < numberBytes; i++)
  {
    if (i == nack)
    {
      returnStatus = _receiveByte(0);
      if (returnStatus == 1)
      {
        return (6);
      }
      if (returnStatus != MR_DATA_NACK)
      {
        return (returnStatus);
      }
    }
    else
    {
      returnStatus = _receiveByte(1);
      if (returnStatus == 1)
      {
        return (6);
      }
      if (returnStatus != MR_DATA_ACK)
      {
        return (returnStatus);
      }
    }
    data[i] = TWDR;
    bytesAvailable = i + 1;
    totalBytes = i + 1;
  }
  returnStatus = _stop();
  if (returnStatus)
  {
    if (returnStatus == 1)
    {
      return (7);
    }
    return (returnStatus);
  }
  return (returnStatus);
}

//////////// LOW-LEVEL METHODS
//////////// (No need to use them if the device uses normal register protocol)

/*
 *  Description:
 *      Sends out a Start Condition. This puts all slave devices on notice that
 *      a transmission is about to start. This function incorporates the timeOut
 *      function.
 *  Parameters:
 *      none
 *  Returns:
 *      uint8_t
 *          0: The start condition was successfully sent
 *          1: The function timed out
 *          2 - 0xFF: See the datasheet
 */
uint8_t I2C::_start()
{
  unsigned long startingTime = millis_elapsed();
  TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);
  while (!(TWCR & (1 << TWINT)))
  {
    if (!timeOutDelay)
    {
      continue;
    }
    if ((millis_elapsed() - startingTime) >= timeOutDelay)
    {
      lockUp();
      return (1);
    }
  }
  if ((TWI_STATUS == START) || (TWI_STATUS == REPEATED_START))
  {
    return (0);
  }
  if (TWI_STATUS == LOST_ARBTRTN)
  {
    uint8_t bufferedStatus = TWI_STATUS;
    lockUp();
    return (bufferedStatus);
  }
  return (TWI_STATUS);
}

/*
 *  Description:
 *      Sends out the address byte. The address byte's first 7 bits are the
 *      7-bit address of the Slave you wish to communicate with, the last bit
 *      specifies if you wish to write or read to that slave,
 *      where 0 = write & 1 = read.
 *  Parameters:
 *      i2cAddress - uint8_t
 *          The address byte you wish to send
 *  Returns:
 *      uint8_t
 *          0: The address byte was successfully sent
 *          1: The function timed out
 *          2 - 0xFF: See the datasheet
 */
uint8_t I2C::_sendAddress(uint8_t i2cAddress)
{
  TWDR = i2cAddress;
  unsigned long startingTime = millis_elapsed();
  TWCR = (1 << TWINT) | (1 << TWEN);
  while (!(TWCR & (1 << TWINT)))
  {
    if (!timeOutDelay)
    {
      continue;
    }
    if ((millis_elapsed() - startingTime) >= timeOutDelay)
    {
      lockUp();
      return (1);
    }
  }
  if ((TWI_STATUS == MT_SLA_ACK) || (TWI_STATUS == MR_SLA_ACK))
  {
    return (0);
  }
  uint8_t bufferedStatus = TWI_STATUS;
  if ((TWI_STATUS == MT_SLA_NACK) || (TWI_STATUS == MR_SLA_NACK))
  {
    _stop();
    return (bufferedStatus);
  }
  else
  {
    lockUp();
    return (bufferedStatus);
  }
}

/*
 *  Description:
 *      Sends out a byte of data to the slave.
 *  Parameters:
 *      i2cData - uint8_t
 *          The data byte you wish to send
 *  Returns:
 *      uint8_t
 *          0: The data byte was successfully sent
 *          1: The function timed out
 *          2 - 0xFF: See the datasheet
 */
uint8_t I2C::_sendByte(uint8_t i2cData)
{
  TWDR = i2cData;
  unsigned long startingTime = millis_elapsed();
  TWCR = (1 << TWINT) | (1 << TWEN);
  while (!(TWCR & (1 << TWINT)))
  {
    if (!timeOutDelay)
    {
      continue;
    }
    if ((millis_elapsed() - startingTime) >= timeOutDelay)
    {
      lockUp();
      return (1);
    }
  }
  if (TWI_STATUS == MT_DATA_ACK)
  {
    return (0);
  }
  uint8_t bufferedStatus = TWI_STATUS;
  if (TWI_STATUS == MT_DATA_NACK)
  {
    _stop();
    return (bufferedStatus);
  }
  else
  {
    lockUp();
    return (bufferedStatus);
  }
}

/*
 *  Description:
 *      Receives a byte from the slave. The ack parameter specifies whether or
 *      not to send an acknowledge signal after receiving it. If it is the last
 *      byte you want to receive it must be 0, so that the slave will stop
 *      transmitting and allow the master to send a stop.
 *  Parameters:
 *      ack - uint8_t
 *          Whether or not you want the master to acknowledge the receipt of the
 *          data byte. If this is the last byte being received, do not
 *          acknowledge.
 *          0: Do not send an ACK signal on receipt
 *          1 - 0xFF: Send an ACK signal on receipt
 *  Returns:
 *      uint8_t
 *          0: The data byte was successfully received
 *          1: The function timed out
 *          2 - 0xFF: See the datasheet
 */
uint8_t I2C::_receiveByte(uint8_t ack)
{
  unsigned long startingTime = millis_elapsed();
  if (ack)
  {
    TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWEA);
  }
  else
  {
    TWCR = (1 << TWINT) | (1 << TWEN);
  }
  while (!(TWCR & (1 << TWINT)))
  {
    if (!timeOutDelay)
    {
      continue;
    }
    if ((millis_elapsed() - startingTime) >= timeOutDelay)
    {
      lockUp();
      return (1);
    }
  }
  if (TWI_STATUS == LOST_ARBTRTN)
  {
    uint8_t bufferedStatus = TWI_STATUS;
    lockUp();
    return (bufferedStatus);
  }
  return (TWI_STATUS);
}

/*
 *  Description:
 *      Receives a byte from the slave. The ack parameter specifies whether or
 *      not to send an acknowledge signal after receiving it. If it is the last
 *      byte you want to receive it must be 0, so that the slave will stop
 *      transmitting and allow the master to send a stop.
 *  Parameters:
 *      ack - uint8_t
 *          Whether or not you want the master to acknowledge the receipt of the
 *          data byte. If this is the last byte being received, do not
 *          acknowledge.
 *          0: Do not send an ACK signal on receipt
 *          1 - 0xFF: Send an ACK signal on receipt
 *      *target - uint8_t
 *          A byte to store the received data.
 *  Returns:
 *      uint8_t
 *          0: The data byte was successfully received
 *          6: The function timed out
 *          1 - 5 and 7 - 0xFF: See the datasheet
 */
uint8_t I2C::_receiveByte(uint8_t ack, uint8_t *target)
{
  uint8_t stat = I2C::_receiveByte(ack);
  if (stat == 1)
  {
    return (6);
  }
  if (ack)
  {
    if (stat != MR_DATA_ACK)
    {
      *target = 0x0;
      return (stat);
    }
  }
  else
  {
    if (stat != MR_DATA_NACK)
    {
      *target = 0x0;
      return (stat);
    }
  }
  *target = TWDR;
  // I suppose that if we get this far we're ok
  return 0;
}

/*
 *  Description:
 *      Send out a stop condition
 *  Parameters:
 *      none
 *  Returns:
 *      uint8_t
 *          0: The stop condition was successfully sent
 *          1: The function timed out
 *          2 - 0xFF: See the datasheet
 */
uint8_t I2C::_stop()
{
  unsigned long startingTime = millis_elapsed();
  TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWSTO);
  while ((TWCR & (1 << TWSTO)))
  {
    if (!timeOutDelay)
    {
      continue;
    }
    if ((millis_elapsed() - startingTime) >= timeOutDelay)
    {
      lockUp();
      return (1);
    }
  }
  return (0);
}

/////////////// Private Methods ////////////////////////////////////////

void I2C::lockUp()
{
  TWCR = 0;                     // releases SDA and SCL lines to high impedance
  TWCR = _BV(TWEN) | _BV(TWEA); // reinitialize TWI
}

I2C I2c = I2C();
