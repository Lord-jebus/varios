/*
 * main.c
 *
 * Created: 10/29/2025 5:30:48 PM
 *  Author: ADMIN
 */ 
#define F_CPU 1000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <avr/wdt.h>
#include <util/twi.h>


#include "src/i2c.h"
#include "src/bme280.h"
#include "src/uart.h"

#define BAUD 9600
#define MYUBRR F_CPU/16/BAUD-1
#define Sleep_PIN PD1
#define Battery_PIN PD2

unsigned char bytes_SF[12];
uint8_t temp_entera = 0;
uint8_t temp_mantisa = 0;
uint8_t temp_signo = 0;
uint16_t presion_atmosferica = 0;


void tomar_medidas(float *temp, float *hum, int32_t *presion) {
	*temp = BME280_readTemperature();
	*hum = BME280_readHumidity();
	*presion = BME280_readPressure();
}

void comparadorA_init(void) {
	ADCSRA &= ~(1 << ADEN);
	DIDR0 |= (1 << ADC0D); 
	DIDR1 |= (1 << AIN0D) | (1 << AIN1D);
	// interrupción en flanco descendente (VAREF > VA0)
	//ACSR = (1 << ACIE) | (1 << ACIS1);
	// interrupcion por flanco ascendente (VA0 > VAREF)
	//ACSR = (1 << ACIE) | (1 << ACIS1) | (1 << ACIS0);
	// ACIS1=0, ACIS0=0 ? interrupción en cualquier cambio
	ACSR = (1 << ACIE); 
	
	ACSR |= (1 << ACI);
}

ISR(ANALOG_COMP_vect) {
	if (ACSR & (1 << ACO)) {
		PORTD |= (1 << Battery_PIN);   // AIN0 > AIN1
		} 
	else {
		PORTD &= ~(1 << Battery_PIN);  // AIN0 < AIN1
	}
}


int main(void)
{
	cli();	
	float temp;
	float hum;
	int32_t presion;

	DDRD |= (1 << Battery_PIN);
	//inicia prendido
	//PORTD |= (1 << Battery_PIN); 
	//inicia apagado
	PORTD &= ~(1 << Battery_PIN); 
	
	//Habilitamos TX1 y RX1 para hacer el debug correcto	
	DDRE |= (1 << PE1);   
	DDRE &= ~(1 << PE0);  

	
	UART_init(MYUBRR);
	
	UART1_init(MYUBRR);
	UART1_sendString("USART1 DEBUG OK\n\r");
	
	I2C_init();
	comparadorA_init();
	
	char buffer[64];
	BME280_init();
	_delay_ms(500);
	
	sei();

	while (1) {		
		//reset al modulo sigfox
		snprintf(buffer, sizeof(buffer), "AT$RC\n\r");
		UART_sendString(buffer);
		UART1_sendString("AT$RC\n\r");
		
		tomar_medidas(&temp, &hum, &presion);
		_delay_ms(100);
		
		//conversion para ajuste de sigfox
		if (temp >= 0){
			temp_signo = 0;
			temp_entera = (int)temp;
			temp_mantisa = (temp - temp_entera) * 10;
		}
		else{
			temp_signo = 1;
			temp_entera = -temp;
			temp_mantisa = (-temp - temp_entera) * 10;
		}
		
		if (presion < 30000) presion = 30000;
		if (presion > 110000) presion = 110000;
		presion_atmosferica = ((presion - 30000) * 1023 / 80000);
		
		uint16_t humedad_10bit = (uint16_t)(hum * 10);
		if (humedad_10bit > 1000) humedad_10bit = 1000;
		
		bytes_SF [0] |= (temp_entera & 0b11111111);
		
		bytes_SF[1] = (temp_mantisa & 0x0F) << 4;
		bytes_SF[1] |= (temp_signo & 0x01) << 4;
		
		bytes_SF[2] = (presion_atmosferica >> 2) & 0xFF;
		bytes_SF[3] = ((presion_atmosferica & 0x03) << 6)| ((humedad_10bit >> 4) & 0x3F);
		bytes_SF[4] = ((humedad_10bit & 0x0F) << 4);
		
		snprintf(buffer, sizeof(buffer), "AT$SF=%02x%02x%02x%02x%02x\n\r", bytes_SF [0], bytes_SF [1], bytes_SF [2], bytes_SF [3], bytes_SF [4]);
		UART_sendString(buffer);
		
		//Enviamos para debug
		UART1_sendString("AT$SF=%02x%02x%02x%02x%02x\n\r", bytes_SF [0], bytes_SF [1], bytes_SF [2], bytes_SF [3], bytes_SF [4]);
							
		//Parte de debug para el sistema en puerto serial 1
		if (UART1_available()) {
			char cmd = UART1_read();

			if (cmd == 'r') {
				UART1_sendString("Reset solicitado.\r\n");
			}
			else if (cmd == 's') {
				UART1_sendString("Estado OK.\r\n");
			}
			else {
				UART1_sendString("Comando desconocido: ");
				UART1_sendChar(cmd);
				UART1_sendString("\r\n");
			}
		}
		
		//Borramos los dato spara una siguiente muestra
		memset(bytes_SF, 0, sizeof(bytes_SF));
		_delay_ms(1000);
		

	}
}