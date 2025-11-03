#include <avr/io.h>

void ADC_init() {
	ADMUX = (1 << REFS0);  // Referencia AVcc, selecciona ADC0 (PC0)
	ADCSRA = (1 << ADEN)  | // Habilita el ADC
	(1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);  // Prescaler de 128 (16MHz/128 = 125kHz)
}

uint16_t ADC_read(uint8_t channel) {
	channel &= 0x07;  // Asegurar que el canal esté en el rango 0-7
	ADMUX = (ADMUX & 0xF8) | channel;  // Seleccionar el canal
	ADCSRA |= (1 << ADSC);  // Inicia la conversión
	while (ADCSRA & (1 << ADSC));  // Espera a que termine la conversión
	return ADC;
}
