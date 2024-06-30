#include <Arduino.h>
#include <StandardCplusplus.h>
#include <vector>
#include <cmath>
#include <Wire.h>

#include "Accelerometer.h"

using namespace std;

void Accelerometer::begin(int addr) {
    i2c_address = addr;

    Wire.begin();                      // Initialize comunication
    Wire.beginTransmission(i2c_address);       // Start communication with MPU6050
    Wire.write(0x6B);                  // Talk to the register 6B
    Wire.write(0x00);                  // Make reset - place a 0 into the 6B register
    Wire.endTransmission(true);        //end the transmission
}

void Accelerometer::readAcceleration() {
    Wire.beginTransmission(i2c_address);
    Wire.write(0x3B); // Start with register 0x3B (ACCEL_XOUT_H)
    Wire.endTransmission(false);
    Wire.requestFrom(i2c_address, 6, true); // Read 6 registers total, each axis value is stored in 2 registers

    // //For a range of +-2g, we need to divide the raw values by 16384, according to the datasheet
    accX = (Wire.read() << 8 | Wire.read()) / 16384.0;
    accY = (Wire.read() << 8 | Wire.read()) / 16384.0;
    accZ = (Wire.read() << 8 | Wire.read()) / 16384.0;
}

struct accComp Accelerometer::getAcceleration() {
    readAcceleration();

    struct accComp readings;
    
    readings.AccX = (uint8_t) map(accX * 100, -200, 200, 0, 255);
    readings.AccY = (uint8_t) map(accY * 100, -200, 200, 0, 255);
    readings.AccZ = (uint8_t) map(accZ * 100, -200, 200, 0, 255);

    return readings;
}