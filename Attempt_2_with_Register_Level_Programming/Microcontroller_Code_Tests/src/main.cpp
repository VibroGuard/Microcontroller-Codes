/*
 * MIT License
 *
 * Copyright (c) 2024 Linuka Ratnayake
 * Copyright (c) 2024 Sanuja Rupasinghe
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

#include <avr/io.h>
#include <avr/interrupt.h>

#include "Accelerometer.h"
#include "auxiliary_functions.h"
#include "uart_communication.h"

// Definitions for clock frequency and limits
#define CLOCK_FREQUENCY 16000000
#define FREQUENCY_LOWER_LIMIT 1
#define FREQUENCY_UPPER_LIMIT 1000

#define BUFFER_SIZE 256
#define SAMPLING_FREQUENCY 200

using namespace std;

const int MPU = 0x68; // MPU6050 I2C address

// Global variables for accelerometer data and buffer management
volatile uint8_t AccX = 0, AccY = 0, AccZ = 0;
volatile uint8_t buffer[3][BUFFER_SIZE];
volatile bool bufferReady = true;
volatile int bufferIndex = 0;

int counterStartValue;

Accelerometer accelerometer;

// Function declarations
void setSamplingFrequency(int frequency);
void printBuffer();
void sendBuffer();
void setup();
void loop();

int main()
{
    setup();

    while (true)
        loop();

    return 0;
}

// Setup function to initialize peripherals and variables
void setup()
{
    setup_millis_counter(); // Initialize millisecond counter

    UART_init(115200); // Initialize UART with baud rate 115200

    // Pin type declaration
    DDRB = DDRB | (1 << PORTB0); // Set PORTB0 as output

    accelerometer.begin(MPU); // Initialize accelerometer

    // Set the sampling frequency for data collection
    setSamplingFrequency(SAMPLING_FREQUENCY); 
}

// Main loop function to continuously read accelerometer data and handle UART communication
void loop()
{
    // Read accelerometer data
    struct accComp readings;
    readings = accelerometer.getAcceleration();

    // Store accelerometer readings in global variables
    AccX = readings.AccX; // X-axis value
    AccY = readings.AccY; // Y-axis value
    AccZ = readings.AccZ; // Z-axis value

    // Check if buffer is full and ready to be sent
    if (!bufferReady)
    {
        // Buffer is full. Send data to the computer.
        sendBuffer();

        // Reset the buffer
        bufferIndex = 0;
        bufferReady = true;
    }

    // Check for incoming UART data
    if (UART_available())
    {
        char *inputBuffer = NULL;

        inputBuffer = UART_receive_string(); // Receive string from UART

        // Convert received string to std::string
        string inputSerial = string(inputBuffer); 

        // Handle different UART commands
        if (inputSerial == "ALERT")
        {
            PORTB = (1 << PORTB0); // Set PORTB0 to HIGH
        }
        else if (inputSerial == "NO_ALERT")
        {
            PORTB = (0 << PORTB0); // Set PORTB0 to LOW
        }
    }
}

// Function to set the sampling frequency using timer interrupts
void setSamplingFrequency(int frequency)
{
    // Ensure frequency is within the defined limits
    if (frequency < FREQUENCY_LOWER_LIMIT)
    {
        frequency = FREQUENCY_LOWER_LIMIT;
    }
    if (frequency > FREQUENCY_UPPER_LIMIT)
    {
        frequency = FREQUENCY_UPPER_LIMIT;
    }

    TCCR1A = 0;
    TCCR1B = 0;

    // Calculate suitable prescaler values based on the frequency
    int prescaler;
    if (frequency <= 10)
    {
        prescaler = 256;
        TCCR1B |= (1 << CS12);
    }
    else if (frequency <= 50)
    {
        prescaler = 64;
        TCCR1B |= (1 << CS10) | (1 << CS11);
    }
    else if (frequency <= 500)
    {
        prescaler = 8;
        TCCR1B |= (1 << CS11);
    }
    else
    {
        prescaler = 1;
        TCCR1B |= (1 << CS10);
    }

    // Calculate the counter start value based on the prescaler and frequency
    counterStartValue = 65536 - CLOCK_FREQUENCY / prescaler / frequency;
    TCNT1 = counterStartValue;

    // Enable timer interrupts
    TIMSK1 |= (1 << TOIE1);
}

// Timer1 overflow interrupt service routine
ISR(TIMER1_OVF_vect)
{
    // Check if the buffer is ready to be filled
    if (bufferReady)
    {
        buffer[0][bufferIndex] = AccX;
        buffer[1][bufferIndex] = AccY;
        buffer[2][bufferIndex] = AccZ;

        bufferIndex++;
        // Check if the buffer is full
        if (bufferIndex == BUFFER_SIZE)
        {
            bufferReady = false;
        }
    }

    // Reset the counter to the start value
    TCNT1 = counterStartValue;
}

// Function to send the buffered data over UART
void sendBuffer()
{
  UART_transmit_string_n("x");
  for (int i = 0; i < BUFFER_SIZE; i++)
  {
    const char *value_to_transmit = to_string((map_range(buffer[0][i], 0, 255, -200, 200) / 100.0));
    UART_transmit_string_n(value_to_transmit);
    delete[] value_to_transmit;
  }

  UART_transmit_string_n("y");
  for (int i = 0; i < BUFFER_SIZE; i++)
  {
    const char *value_to_transmit = to_string((map_range(buffer[1][i], 0, 255, -200, 200) / 100.0));
    UART_transmit_string_n(value_to_transmit);
    delete[] value_to_transmit;
  }

  UART_transmit_string_n("z");
  for (int i = 0; i < BUFFER_SIZE; i++)
  {
    const char *value_to_transmit = to_string((map_range(buffer[2][i], 0, 255, -200, 200) / 100.0));
    UART_transmit_string_n(value_to_transmit);
    delete[] value_to_transmit;
  }
}
