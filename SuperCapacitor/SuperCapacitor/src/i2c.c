#include <avr/io.h>
#include <util/twi.h>


void I2C_init() {
	// Prescaler = 1  (TWPS1:0 = 0)
	TWSR0 &= ~((1 << 1) | (1 << 0));

	// TWBR0 = 72 para 100 kHz con 16 MHz
	TWBR0 = 72;
}

void I2C_start() {
	TWCR0 = (1 << TWSTA) | (1 << TWEN) | (1 << TWINT);
	while (!(TWCR0 & (1 << TWINT)));
}

void I2C_stop() {
	TWCR0 = (1 << TWSTO) | (1 << TWEN) | (1 << TWINT);
}

void I2C_write(uint8_t data) {
	TWDR0 = data;
	TWCR0 = (1 << TWEN) | (1 << TWINT);
	while (!(TWCR0 & (1 << TWINT)));
}

uint8_t I2C_readAck() {
	TWCR0 = (1 << TWEN) | (1 << TWINT) | (1 << TWEA);
	while (!(TWCR0 & (1 << TWINT)));
	return TWDR0;
}

uint8_t I2C_readNack() {
	TWCR0 = (1 << TWEN) | (1 << TWINT);
	while (!(TWCR0 & (1 << TWINT)));
	return TWDR0;
}
