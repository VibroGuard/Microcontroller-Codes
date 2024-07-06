/*
 * MIT License
 * 
 * Copyright (c) 2024 Linuka Ratnayake
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include "Accelerometer.h"
#include "auxiliary_functions.h"
// #include "i2c_communication.h"

#include "I2C.h"

// MPU6050 registers
#define MPU6050_REG_RESET 0x6B
#define MPU6050_REG_ACCEL_XOUT_H 0x3B

// Initializing I2C Connection and MPU6050
void Accelerometer::begin(int device_address)
{
    i2c_address = device_address;

    I2c.begin();
    I2c.write(i2c_address, MPU6050_REG_RESET, 0x00);
    I2c.end();
}

void Accelerometer::readAcceleration()
{
    int16_t rawAccX, rawAccY, rawAccZ;

    I2c.begin();
    I2c.read(i2c_address, MPU6050_REG_ACCEL_XOUT_H, 6); // read 6 bytes (x,y,z) from the device

    // Read raw data
    rawAccX = (I2c.receive() << 8) | (I2c.receive());
    rawAccY = (I2c.receive() << 8) | (I2c.receive());
    rawAccZ = (I2c.receive() << 8) | (I2c.receive());

    I2c.end();

    // Convert raw data to g-force (for ±2g range)
    const float accScale = 16384.0;
    accX = (float)rawAccX / accScale;
    accY = (float)rawAccY / accScale;
    accZ = (float)rawAccZ / accScale;
}

struct accComp Accelerometer::getAcceleration()
{
    readAcceleration();

    struct accComp readings;

    // Map accelerometer values to 0-255 range
    readings.AccX = (uint8_t)map_range(accX * 100, -200, 200, 0, 255);
    readings.AccY = (uint8_t)map_range(accY * 100, -200, 200, 0, 255);
    readings.AccZ = (uint8_t)map_range(accZ * 100, -200, 200, 0, 255);

    return readings;
}