// #include <Arduino.h>
// #include <Wire.h>
// #include "Accelerometer.h"

// #define CLOCK_FREQUENCY 16000000
// #define FREQUENCY_LOWER_LIMIT 1
// #define FREQUENCY_UPPER_LIMIT 1000

// #define SAMPLING_FREQUENCY 100

// const int MPU = 0x68; // MPU6050 I2C address
// volatile float AccX = 0, AccY = 0, AccZ = 0;
// volatile bool ready = false;
// volatile float buffer[3];

// int counterStartValue;

// Accelerometer accelerometer;

// void setSamplingFrequency(int frequency);
// void sendData();

// void setup() {
//   Serial.begin(115200);

//   accelerometer.begin(MPU);
  
//   setSamplingFrequency(SAMPLING_FREQUENCY);
// }

// void loop() {
//   // === Read acceleromter data === //
//   struct accComp readings;
//   readings = accelerometer.getAcceleration();

//   // //For a range of +-2g, we need to divide the raw values by 16384, according to the datasheet
//   AccX = readings.AccX; // X-axis value
//   AccY = readings.AccY; // Y-axis value
//   AccZ = readings.AccZ; // Z-axis value

//   if (ready) {
//     // Do something to the collected data.
//     sendData();
//     ready = false;
//   }
// }

// void setSamplingFrequency(int frequency)
// {
//   // The following are internal registers in ATmega328.
//   // Internal interrupts (Timers) are used to keep track of time precisely.

//   // Adding lower and upper limits to frequency
//   if (frequency < FREQUENCY_LOWER_LIMIT) {
//     frequency = FREQUENCY_LOWER_LIMIT;
//   }
//   if (frequency > FREQUENCY_UPPER_LIMIT) {
//     frequency = FREQUENCY_UPPER_LIMIT;
//   }

//   TCCR1A = 0;
//   TCCR1B = 0;

//   // Calculating suitable prescalar values.
//   int prescaler;
//   if (frequency <= 10) {
//     prescaler = 256;
//     TCCR1B |= (1 << CS12);
//   }
//   else if (frequency <= 50) {
//     prescaler = 64;
//     TCCR1B |= (1 << CS10) | (1 << CS11);
//   }
//   else if (frequency <= 500) {
//     prescaler = 8;
//     TCCR1B |= (1 << CS11);
//   }
//   else {
//     prescaler = 1;
//     TCCR1B |= (1 << CS10);
//   }

//   // Calculating the value the counter should start counting from.
//   counterStartValue = 65536 - CLOCK_FREQUENCY / prescaler / frequency;
//   TCNT1 = counterStartValue;

//   // Enabling timer interrupts.
//   TIMSK1 |= (1 << TOIE1);
// }

// ISR(TIMER1_OVF_vect)
// {
//   buffer[0] = AccX;
//   buffer[1] = AccY;
//   buffer[2] = AccZ;

//   ready = true;
//   TCNT1 = counterStartValue;
// }

// void sendData() {
//   // Serial.println("x");
//   // Serial.println(buffer[0]);

//   // Serial.println("y");
//   // Serial.println(buffer[1]);

//   // Serial.println("z");
//   // Serial.println(buffer[2]);

//   Serial.println(String(buffer[0]) + " " + String(buffer[1]) + " " + String(buffer[2]));
// }