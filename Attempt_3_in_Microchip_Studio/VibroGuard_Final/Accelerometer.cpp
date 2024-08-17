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
#include "I2C.h"

// MPU6050 registers
#define MPU6050_REG_RESET 0x6B
#define MPU6050_REG_ACCEL_XOUT_H 0x3B

// Function to initialize the I2C connection and MPU6050
void Accelerometer::begin(int device_address)
{
    i2c_address = device_address; // Set the device address for I2C communication

    I2c.timeOut(1000); // Set I2C timeout period to 1000ms, to automatically recover from lockups

    I2c.begin(); // Initialize the I2C communication
    I2c.write(i2c_address, MPU6050_REG_RESET, 0x00); // Write 0x00 to the MPU6050_REG_RESET register to reset the MPU6050
    I2c.end(); // End the I2C communication
}

// Function to read acceleration values from the MPU6050
void Accelerometer::readAcceleration()
{
    int16_t rawAccX, rawAccY, rawAccZ; // Variables to store raw acceleration data

    I2c.begin(); // Initialize the I2C communication
    I2c.read(i2c_address, MPU6050_REG_ACCEL_XOUT_H, 6); // Read 6 bytes (x, y, z) from the MPU6050

    // Read raw acceleration data from the I2C buffer
    rawAccX = (I2c.receive() << 8) | (I2c.receive());
    rawAccY = (I2c.receive() << 8) | (I2c.receive());
    rawAccZ = (I2c.receive() << 8) | (I2c.receive());

    I2c.end(); // End the I2C communication

    // Convert raw data to g-force (for ±2g range)
    const float accScale = 16384.0; // Scaling factor for accelerometer
    accX = (float)rawAccX / accScale; // Convert raw X data to g-force
    accY = (float)rawAccY / accScale; // Convert raw Y data to g-force
    accZ = (float)rawAccZ / accScale; // Convert raw Z data to g-force
}

// Function to get acceleration values and map them to a 0-255 range
struct accComp Accelerometer::getAcceleration()
{
    readAcceleration(); // Read the current acceleration values

    struct accComp readings; // Structure to store the mapped acceleration values

    // Map accelerometer values to 0-255 range
    readings.AccX = (uint8_t)map_range(accX * 100, -200, 200, 0, 255);
    readings.AccY = (uint8_t)map_range(accY * 100, -200, 200, 0, 255);
    readings.AccZ = (uint8_t)map_range(accZ * 100, -200, 200, 0, 255);

    return readings; // Return the mapped acceleration values
}
