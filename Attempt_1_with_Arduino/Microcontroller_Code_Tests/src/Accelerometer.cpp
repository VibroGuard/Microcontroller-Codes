#include <Arduino.h>
#include <StandardCplusplus.h>
#include <vector>
#include <cmath>
#include <Wire.h>

#include "Accelerometer.h"

using namespace std;

void Accelerometer::begin(int addr) {
    i2c_address = addr;

    // Initialize comunication
    Wire.begin();             

    // Start communication with MPU6050
    Wire.beginTransmission(i2c_address);     
    
    // Talk to the register 6B
    Wire.write(0x6B);                  
    
    // Make reset - place a 0 into the 6B register
    Wire.write(0x00);                  
    
    //end the transmission
    Wire.endTransmission(true);        
}

void Accelerometer::readAcceleration() {
    Wire.beginTransmission(i2c_address);
    // Start with register 0x3B (ACCEL_XOUT_H)
    Wire.write(0x3B); 
    Wire.endTransmission(false);
    // Read 6 registers total, each axis value is stored in 2 registers
    Wire.requestFrom(i2c_address, 6, true); 

    //For a range of +-2g, we need to divide the 
    //raw values by 16384, according to the datasheet
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