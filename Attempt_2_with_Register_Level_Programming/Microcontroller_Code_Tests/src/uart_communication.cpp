#include "uart_communication.h"

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
