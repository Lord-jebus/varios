#include "uart.h"
#include <string.h>

// ------------------ Funciones UART ------------------
void UART_init(unsigned int ubrr) {
	UBRR0H = (unsigned char)(ubrr >> 8);
	UBRR0L = (unsigned char)ubrr;
	UCSR0B = (1 << TXEN0);
	UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
}

void UART_send(char data) {
	while (!(UCSR0A & (1 << UDRE0)));
	UDR0 = data;
}

void UART_sendString(const char *str) {
	while (*str) {
		UART_send(*str++);
	}
}

/*
char UART_receive() {
	while (!(UCSR0A & (1 << RXC0))); // Esperar a recibir un dato
	return UDR0;
}

int UART_waitForOK() {
	char buffer[10] = {0}; // Buffer para la respuesta
	uint8_t i = 0;
	
	while (i < 9) {  // Leer hasta 9 caracteres
		char received = UART_receive();  // Leer un carácter
		
		if (received == '\n' || received == '\r') {
			continue;  // Ignorar saltos de línea
		}
		
		buffer[i++] = received;  // Guardar el carácter
		buffer[i] = '\0';  // Asegurar terminación de string
		
		if (strstr(buffer, "OK") != NULL) {  // Buscar "OK"
			snprintf(buffer, sizeof(buffer), "yes2\r");
			UART_sendString(buffer);
			return 1;  // Éxito
		}
	}
	snprintf(buffer, sizeof(buffer), "yes2\r");
	UART_sendString(buffer);
	return 0;  // No se recibió "OK"
}
*/