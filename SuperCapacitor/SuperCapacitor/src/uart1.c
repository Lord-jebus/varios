/*
 * uart1.c
 *
 * Created: 11/17/2025 6:39:30 PM
 *  Author: Jebus
 */ 

#include "uart1.h"

void UART1_init(uint16_t ubrr){
	// Baud rate
	UBRR1H = (ubrr >> 8);
	UBRR1L = ubrr;

	// Frame: 8N1
	UCSR1C = (1 << UCSZ11) | (1 << UCSZ10);

	// Enable RX y TX
	UCSR1B = (1 << RXEN1) | (1 << TXEN1);
}

void UART1_sendChar(char c){
	while (!(UCSR1A & (1 << UDRE1)));  // Espera buffer TX vacío
	UDR1 = c;
}

void UART1_sendString(const char *s){
	while (*s){
		UART1_sendChar(*s++);
	}
}

// Bloqueante — espera a que llegue un byte
char UART1_receiveChar(void){
	while (!(UCSR1A & (1 << RXC1)));  // Espera dato recibido
	return UDR1;
}

// No bloqueante — retorna 1 si hay dato
uint8_t UART1_available(void){
	return (UCSR1A & (1 << RXC1));
}

// No bloqueante — devuelve dato o 0 si no hay (útil para polling)
char UART1_read(void){
	if (UART1_available())
	return UDR1;
	return 0;
}
