#ifndef uart_H_
#define uart_H_

#include <avr/io.h>

void UART_init(unsigned int ubrr);
void UART_send(char data);
void UART_sendString(const char *str);
//char UART_receive(void);
//int UART_waitForOK(void);


#endif /* uart_H_ */