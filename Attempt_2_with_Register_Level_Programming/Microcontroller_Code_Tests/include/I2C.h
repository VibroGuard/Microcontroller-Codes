/*
  I2C.h   - I2C library
  Copyright (c) 2011-2012 Wayne Truchsess.  All right reserved.

  Originally written by: Wayne Truchsess
  Modified by: 2024 Linuka Ratnayake 

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

#ifndef I2C_H
#define I2C_H

#define START 0x08
#define REPEATED_START 0x10
#define MT_SLA_ACK 0x18
#define MT_SLA_NACK 0x20
#define MT_DATA_ACK 0x28
#define MT_DATA_NACK 0x30
#define MR_SLA_ACK 0x40
#define MR_SLA_NACK 0x48
#define MR_DATA_ACK 0x50
#define MR_DATA_NACK 0x58
#define LOST_ARBTRTN 0x38
#define TWI_STATUS (TWSR & 0xF8)
#define SLA_W(address) (address << 1)
#define SLA_R(address) ((address << 1) + 0x01)
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))

#define MAX_BUFFER_SIZE 32

class I2C
{
public:
  I2C();
  void begin();
  void end();
  void timeOut(uint16_t);
  void pullup(uint8_t);
  uint8_t receive();
  uint8_t write(uint8_t, uint8_t, uint8_t);
  uint8_t read(uint8_t, uint8_t, uint8_t);

  // Low-level methods
  uint8_t _start();
  uint8_t _sendAddress(uint8_t);
  uint8_t _sendByte(uint8_t);
  uint8_t _receiveByte(uint8_t);
  uint8_t _receiveByte(uint8_t, uint8_t *target);
  uint8_t _stop();

private:
  void lockUp();
  uint8_t returnStatus;
  uint8_t nack;
  uint8_t data[MAX_BUFFER_SIZE];
  static uint8_t bytesAvailable;
  static uint8_t bufferIndex;
  static uint8_t totalBytes;
  static uint16_t timeOutDelay;
};

extern I2C I2c;

#endif
