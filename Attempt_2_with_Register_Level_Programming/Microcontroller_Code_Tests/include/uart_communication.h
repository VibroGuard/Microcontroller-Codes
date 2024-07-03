#ifndef UART_COMMUNICATION_H
#define UART_COMMUNICATION_H

#include <avr/io.h>
#include <stdint.h>
#include <stdlib.h>

void UART_init(uint32_t baud_rate);
void UART_transmit(unsigned char data);
void UART_transmit_string(const char *str);
bool UART_available(void);
char UART_receive(void);
char *UART_receive_string(void);

#endif