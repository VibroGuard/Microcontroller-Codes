#ifndef ACCELEROMETER_H
#define ACCELEROMETER_H

#include <StandardCplusplus.h>
#include <vector>

using namespace std;

struct accComp {
  float AccX;
  float AccY;
  float AccZ;
};

class Accelerometer {
    private:
    int i2c_address;
    float accX, accY, accZ;
    float accErrorX = 0, accErrorY = 0;
    
    void readAcceleration();

    public:
    void begin(int);
    void calibrateError();  // This function is recommended to be run when the sensor is completely flat on a surface.
    struct accComp getAcceleration();
    float getRoll();
    float getPitch();
};

#endif