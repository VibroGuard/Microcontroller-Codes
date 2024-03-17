#include <Arduino.h>
#include <Wire.h>

#define BUFFER_SIZE 32

const int MPU = 0x68; // MPU6050 I2C address
volatile float AccX = 0, AccY = 0, AccZ = 0;
volatile float buffer[3][BUFFER_SIZE];
volatile bool bufferReady = true;
volatile int bufferIndex = 0;

void setSamplingFrequency();
void printBuffer();

void setup() {
  Serial.begin(19200);

  Wire.begin();                      // Initialize comunication
  Wire.beginTransmission(MPU);       // Start communication with MPU6050 // MPU=0x68
  Wire.write(0x6B);                  // Talk to the register 6B
  Wire.write(0x00);                  // Make reset - place a 0 into the 6B register
  Wire.endTransmission(true);        //end the transmission
  
  setSamplingFrequency();
}

void loop() {
// === Read acceleromter data === //
  Wire.beginTransmission(MPU);
  Wire.write(0x3B); // Start with register 0x3B (ACCEL_XOUT_H)
  Wire.endTransmission(false);
  Wire.requestFrom(MPU, 6, true); // Read 6 registers total, each axis value is stored in 2 registers

  // //For a range of +-2g, we need to divide the raw values by 16384, according to the datasheet
  AccX = (Wire.read() << 8 | Wire.read()) / 16384.0; // X-axis value
  AccY = (Wire.read() << 8 | Wire.read()) / 16384.0; // Y-axis value
  AccZ = (Wire.read() << 8 | Wire.read()) / 16384.0; // Z-axis value

  if (!bufferReady) { // Buffer is full.
    // Do something to the collected data.
    printBuffer();
    delay(10000);

    // Reset the buffer
    bufferIndex = 0;
    bufferReady = true;
  }
}

void setSamplingFrequency()
{
  // The following are internal registers in ATmega328.
  // Internal interrupts (Timers) are used to keep track of time precisely.

  // // Sampling frequency - 100Hz
  TCCR1A = 0;
  TCCR1B = 0;
  TCNT1 = 45536;
  TCCR1B |= (1 << CS11);
  TIMSK1 |= (1 << TOIE1);

  // // Sampling frequency - 1Hz
  // TCCR1A = 0;
  // TCCR1B = 0;
  // TCNT1 = 3036;
  // TCCR1B |= (1 << CS12);
  // TIMSK1 |= (1 << TOIE1);
}

ISR(TIMER1_OVF_vect)
{
  if (bufferReady) {
    buffer[0][bufferIndex] = AccX;
    buffer[1][bufferIndex] = AccY;
    buffer[2][bufferIndex] = AccZ;

    bufferIndex++;
    if (bufferIndex == BUFFER_SIZE) {
      bufferReady = false;
    }
  }

  TCNT1 = 45536;
  // TCNT1 = 3036;
}

void printBuffer() {
  for (int i = 0; i < BUFFER_SIZE; i++) {
    Serial.print(buffer[0][i]);
    Serial.print(" ");
  }
  Serial.println();

  for (int i = 0; i < BUFFER_SIZE; i++) {
    Serial.print(buffer[1][i]);
    Serial.print(" ");
  }
  Serial.println();

  for (int i = 0; i < BUFFER_SIZE; i++) {
    Serial.print(buffer[1][i]);
    Serial.print(" ");
  }
  Serial.println();
}