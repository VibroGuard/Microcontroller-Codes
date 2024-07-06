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
#include "i2c_communication.h"

// MPU6050 registers
#define MPU6050_REG_PWR_MGMT_1 0x6B
#define MPU6050_REG_ACCEL_XOUT_H 0x3B

// Initializing I2C Connection and MPU6050
void Accelerometer::begin(int i2c_address)
{
    i2c_init(); // Initialize I2C

    i2c_start();
    i2c_write(i2c_address << 1); // Write mode
    i2c_write(MPU6050_REG_PWR_MGMT_1);
    i2c_write(0); // Clear sleep mode bit
    i2c_stop();
}

void Accelerometer::readAcceleration()
{
    int16_t rawAccX, rawAccY, rawAccZ;

    i2c_start();
    i2c_write(i2c_address << 1); // Write mode
    i2c_write(MPU6050_REG_ACCEL_XOUT_H);
    i2c_stop();

    i2c_start();
    i2c_write((i2c_address << 1) | 1); // Read mode

    // Read raw data
    rawAccX = (i2c_read(true) << 8) | (i2c_read(true));
    rawAccY = (i2c_read(true) << 8) | (i2c_read(true));
    rawAccZ = (i2c_read(true) << 8) | (i2c_read(false));

    i2c_stop();

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