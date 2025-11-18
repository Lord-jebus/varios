/*
 * uart1.h
 *
 * Created: 11/17/2025 6:39:12 PM
 *  Author: Jebus
 */ 


#ifndef UART1_H_
#define UART1_H_

#include <avr/io.h>
#include <stdint.h>

void UART1_init(uint16_t ubrr);
void UART1_sendChar(char c);
void UART1_sendString(const char *s);

// RX funciones
char UART1_receiveChar(void);        // Bloqueante
uint8_t UART1_available(void);       // No bloqueante
char UART1_read(void);               // Lee si hay dato, si no devuelve 0

#endif

