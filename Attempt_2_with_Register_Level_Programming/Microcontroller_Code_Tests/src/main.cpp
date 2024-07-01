#include <Arduino.h>
#include <Wire.h>
#include "Accelerometer.h"
#include "auxiliary_functions.h"

#define CLOCK_FREQUENCY 16000000
#define FREQUENCY_LOWER_LIMIT 1
#define FREQUENCY_UPPER_LIMIT 1000

#define BUFFER_SIZE 256
#define SAMPLING_FREQUENCY 200

#define ALERT_PIN_1 8
#define ALERT_PIN_2 9

const int MPU = 0x68; // MPU6050 I2C address
volatile uint8_t AccX = 0, AccY = 0, AccZ = 0;
volatile uint8_t buffer[3][BUFFER_SIZE];
volatile bool bufferReady = true;
volatile int bufferIndex = 0;

int counterStartValue;

Accelerometer accelerometer;

void setSamplingFrequency(int frequency);
void printBuffer();
void sendBuffer();


// Function to initialize UART0
void UART0_init(uint32_t baud_rate) {
    uint16_t ubrr_value = (F_CPU / (16 * baud_rate)) - 1;
    
    // Set baud rate
    UBRR0H = (ubrr_value >> 8);
    UBRR0L = ubrr_value;

    // Enable receiver and transmitter
    UCSR0B = (1 << RXEN0) | (1 << TXEN0);
    
    // Set frame format: 8 data bits, 1 stop bit, no parity
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
}


// Function to transmit a character
void UART0_transmit(unsigned char data) {
    // Wait for empty transmit buffer
    while (!(UCSR0A & (1 << UDRE0)));
    
    // Put data into buffer, sends the data
    UDR0 = data;
}


// Function to transmit a string
void UART0_transmit_string(const char* str) {
    while (*str) {
        UART0_transmit(*str++);
    }
}




void setup() {
  UART0_init(115200);

  pinMode(ALERT_PIN_1, OUTPUT);
  pinMode(ALERT_PIN_2, OUTPUT);

  accelerometer.begin(MPU);
  
  setSamplingFrequency(SAMPLING_FREQUENCY);
}

void loop() {
  // === Read acceleromter data === //
  struct accComp readings;
  readings = accelerometer.getAcceleration();

  // //For a range of +-2g, we need to divide the raw values by 16384, according to the datasheet
  AccX = readings.AccX; // X-axis value
  AccY = readings.AccY; // Y-axis value
  AccZ = readings.AccZ; // Z-axis value

  if (!bufferReady) { // Buffer is full.
    // Do something to the collected data.
    // printBuffer();
    sendBuffer();
    // delay(5000);

    // Reset the buffer
    bufferIndex = 0;
    bufferReady = true;
  }

  if (Serial.available())
  {
    const int buffSize = 10;
    char inputBuffer[buffSize];
    Serial.readStringUntil('\n').toCharArray(inputBuffer, buffSize);

    String inputSerial = String(inputBuffer);

    if (inputSerial == "ALERT") {
      // giveAlert();
      digitalWrite(ALERT_PIN_1, HIGH);
    }
    else if (inputSerial == "NO_ALERT") {
      digitalWrite(ALERT_PIN_1, LOW);
    }
  }
}



void setSamplingFrequency(int frequency)
{
  // The following are internal registers in ATmega328.
  // Internal interrupts (Timers) are used to keep track of time precisely.

  // Adding lower and upper limits to frequency
  if (frequency < FREQUENCY_LOWER_LIMIT) {
    frequency = FREQUENCY_LOWER_LIMIT;
  }
  if (frequency > FREQUENCY_UPPER_LIMIT) {
    frequency = FREQUENCY_UPPER_LIMIT;
  }

  TCCR1A = 0;
  TCCR1B = 0;

  // Calculating suitable prescalar values.
  int prescaler;
  if (frequency <= 10) {
    prescaler = 256;
    TCCR1B |= (1 << CS12);
  }
  else if (frequency <= 50) {
    prescaler = 64;
    TCCR1B |= (1 << CS10) | (1 << CS11);
  }
  else if (frequency <= 500) {
    prescaler = 8;
    TCCR1B |= (1 << CS11);
  }
  else {
    prescaler = 1;
    TCCR1B |= (1 << CS10);
  }

  // Calculating the value the counter should start counting from.
  counterStartValue = 65536 - CLOCK_FREQUENCY / prescaler / frequency;
  TCNT1 = counterStartValue;

  // Enabling timer interrupts.
  TIMSK1 |= (1 << TOIE1);
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

  TCNT1 = counterStartValue;
}

void sendBuffer() {
  Serial.println("x");
  for (int i = 0; i < BUFFER_SIZE; i++) {
    Serial.println(map_range(buffer[0][i], 0, 255, -200, 200)/ 100.0);
  }

  Serial.println("y");
  for (int i = 0; i < BUFFER_SIZE; i++) {
    Serial.println(map_range(buffer[1][i], 0, 255, -200, 200)/ 100.0);
  }

  Serial.println("z");
  for (int i = 0; i < BUFFER_SIZE; i++) {
    Serial.println(map_range(buffer[2][i], 0, 255, -200, 200)/ 100.0);
  }
}


