#ifndef ACCELEROMETER_H
#define ACCELEROMETER_H

#include <stdint.h>

using namespace std;

struct accComp
{
  uint8_t AccX;
  uint8_t AccY;
  uint8_t AccZ;
};

class Accelerometer
{
private:
  int i2c_address;
  float accX, accY, accZ;

  void readAcceleration();

public:
  void begin(int i2c_address);
  struct accComp getAcceleration();
};

#endif