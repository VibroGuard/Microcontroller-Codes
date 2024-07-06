/*
 * MIT License
 * 
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

#include "uart_communication.h"

// Function to initialize UART0
void UART_init(uint32_t baud_rate)
{
    uint16_t ubrr_value = round((F_CLK / (16.0 * baud_rate)) - 1);

    // Set baud rate
    UBRR0H = (ubrr_value >> 8);
    UBRR0L = ubrr_value;

    // Enable receiver and transmitter
    UCSR0B = (1 << RXEN0) | (1 << TXEN0);

    // Set frame format: 8 data bits, 1 stop bit, no parity
    UCSR0C = (1 << UCSZ01) | (3 << UCSZ00);
}

// Function to transmit a character
void UART_transmit(unsigned char data)
{
    // Wait for empty transmit buffer
    while (!(UCSR0A & (1 << UDRE0)))
        ;

    // Put data into buffer, sends the data
    UDR0 = data;
}

// Function to transmit a string
void UART_transmit_string(const char *str)
{
    while (*str)
    {
        UART_transmit(*str++);
    }

    // Write new line.
    UART_transmit('\n');
}

// Function to check if serial data is available to be read
bool UART_available(void)
{
    // Check if RXC0 flag is set (data available to be read)
    return (UCSR0A & (1 << RXC0));
}

// Function to receive a character
char UART_receive(void)
{
    // Wait for data to be received
    while (!(UCSR0A & (1 << RXC0)))
        ;

    // Get and return received data from buffer
    return UDR0;
}

// Function to receive a string
char *UART_receive_string(void)
{
    char *received_string = NULL;
    uint8_t index = 0;
    char received_char;

    // Allocate memory for received string
    received_string = (char *)malloc(1); // Start with a small allocation

    if (received_string == NULL)
    {
        return NULL; // Failed to allocate memory
    }

    // Read characters until newline or maximum length
    do
    {
        // Receive a character
        received_char = UART_receive();

        // Reallocate memory to accommodate more characters
        received_string = (char *)realloc(received_string, index + 2);

        if (received_string == NULL)
        {
            return NULL; // Failed to reallocate memory
        }

        // Store character in received string
        received_string[index++] = received_char;

    } while (received_char != '\n' && received_char != '\0');

    // Add null terminator to end of string
    received_string[index] = '\0';

    return received_string;
}