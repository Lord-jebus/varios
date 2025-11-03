#ifndef I2C_328PB_H
#define I2C_328PB_H
//SOLO PARA EL 328PB

#include <stdint.h>

void I2C0_init();
void I2C_start();
void I2C_stop();
void I2C_write(uint8_t data);
uint8_t I2C_readAck();
uint8_t I2C_readNack();

#endif
