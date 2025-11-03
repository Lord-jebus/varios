#ifndef BME280_H_
#define BME280_H_

#include <avr/io.h>
#include "i2c.h"

// Dirección I2C del BME280
#define BME280_ADDR 0x76

// Registros principales
#define BME280_REG_TEMP_MSB   0xFA
#define BME280_REG_PRESS_MSB  0xF7
#define BME280_REG_HUM_MSB    0xFD
#define BME280_REG_CTRL_HUM   0xF2
#define BME280_REG_CTRL_MEAS  0xF4
#define BME280_REG_CONFIG     0xF5

// Calibración
#define BME280_CALIB_DATA     0x88
#define BME280_CALIB_HUM      0xA1
#define BME280_CALIB_HUM2     0xE1

// Variables globales
uint16_t dig_T1, dig_P1;
int16_t dig_T2, dig_T3;
int16_t dig_P2, dig_P3, dig_P4, dig_P5, dig_P6, dig_P7, dig_P8, dig_P9;
uint8_t dig_H1, dig_H3;
int16_t dig_H2, dig_H4, dig_H5, dig_H6;
int32_t t_fine;

// Funciones
void BME280_writeRegister(uint8_t reg, uint8_t value);
void BME280_readCalibrationData(void);
void BME280_init(void);
float BME280_readTemperature(void);
int32_t BME280_readPressure(void);
float BME280_readHumidity(void);

#endif
