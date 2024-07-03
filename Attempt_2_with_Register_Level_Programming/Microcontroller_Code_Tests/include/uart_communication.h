#ifndef UART_COMMUNICATION_H
#define UART_COMMUNICATION_H

#include <avr/io.h>
#include <stdint.h>

void UART0_init(uint32_t baud_rate);
void UART0_transmit(unsigned char data);
void UART0_transmit_string(const char* str);

#endif