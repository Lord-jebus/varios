#include "bme280.h"

// ------------------ Funciones BME280 ------------------

void BME280_writeRegister(uint8_t reg, uint8_t value) {
	I2C_start();
	I2C_write((BME280_ADDR << 1) | 0);
	I2C_write(reg);
	I2C_write(value);
	I2C_stop();
}

void BME280_readCalibrationData(void) {
	I2C_start();
	I2C_write((BME280_ADDR << 1) | 0);
	I2C_write(BME280_CALIB_DATA);
	I2C_start();
	I2C_write((BME280_ADDR << 1) | 1);

	dig_T1 = I2C_readAck() | (I2C_readAck() << 8);
	dig_T2 = I2C_readAck() | (I2C_readAck() << 8);
	dig_T3 = I2C_readAck() | (I2C_readAck() << 8);

	dig_P1 = I2C_readAck() | (I2C_readAck() << 8);
	dig_P2 = I2C_readAck() | (I2C_readAck() << 8);
	dig_P3 = I2C_readAck() | (I2C_readAck() << 8);
	dig_P4 = I2C_readAck() | (I2C_readAck() << 8);
	dig_P5 = I2C_readAck() | (I2C_readAck() << 8);
	dig_P6 = I2C_readAck() | (I2C_readAck() << 8);
	dig_P7 = I2C_readAck() | (I2C_readAck() << 8);
	dig_P8 = I2C_readAck() | (I2C_readAck() << 8);
	dig_P9 = I2C_readAck() | (I2C_readAck() << 8);

	I2C_stop();

	// Leer calibración de humedad
	I2C_start();
	I2C_write((BME280_ADDR << 1) | 0);
	I2C_write(BME280_CALIB_HUM);
	I2C_start();
	I2C_write((BME280_ADDR << 1) | 1);

	dig_H1 = I2C_readAck();

	I2C_stop();

	I2C_start();
	I2C_write((BME280_ADDR << 1) | 0);
	I2C_write(BME280_CALIB_HUM2);
	I2C_start();
	I2C_write((BME280_ADDR << 1) | 1);

	dig_H2 = I2C_readAck() | (I2C_readAck() << 8);
	dig_H3 = I2C_readAck();
	uint8_t e4 = I2C_readAck();
	uint8_t e5 = I2C_readAck();
	uint8_t e6 = I2C_readNack();

	I2C_stop();

	dig_H4 = (e4 << 4) | (e5 & 0x0F);
	dig_H5 = (e6 << 4) | (e5 >> 4);
	dig_H6 = e6;
}

void BME280_init(void) {
	BME280_writeRegister(BME280_REG_CTRL_HUM, 0x01);   // Humedad oversampling x1
	BME280_writeRegister(BME280_REG_CTRL_MEAS, 0x27);  // Temp/Pres oversampling x1, modo normal
	BME280_writeRegister(BME280_REG_CONFIG, 0xA0);     // Standby y filtros
	BME280_readCalibrationData();
}

float BME280_readTemperature(void) {
	int32_t adc_T;

	I2C_start();
	I2C_write((BME280_ADDR << 1) | 0);
	I2C_write(BME280_REG_TEMP_MSB);
	I2C_start();
	I2C_write((BME280_ADDR << 1) | 1);
	adc_T = ((int32_t)I2C_readAck() << 12) | ((int32_t)I2C_readAck() << 4) | (I2C_readNack() >> 4);
	I2C_stop();

	int32_t var1 = ((((adc_T >> 3) - ((int32_t)dig_T1 << 1))) * ((int32_t)dig_T2)) >> 11;
	int32_t var2 = (((((adc_T >> 4) - ((int32_t)dig_T1)) *
	((adc_T >> 4) - ((int32_t)dig_T1))) >> 12) *
	((int32_t)dig_T3)) >>
	14;
	t_fine = var1 + var2;

	return ((t_fine * 5 + 128) >> 8) / 100.0;
}

int32_t BME280_readPressure(void) {
	int32_t adc_P;

	I2C_start();
	I2C_write((BME280_ADDR << 1) | 0);
	I2C_write(BME280_REG_PRESS_MSB);
	I2C_start();
	I2C_write((BME280_ADDR << 1) | 1);
	adc_P = ((int32_t)I2C_readAck() << 12) | ((int32_t)I2C_readAck() << 4) | (I2C_readNack() >> 4);
	I2C_stop();

	int64_t var1, var2, p;
	var1 = ((int64_t)t_fine) - 128000;
	var2 = var1 * var1 * (int64_t)dig_P6;
	var2 = var2 + ((var1 * (int64_t)dig_P5) << 17);
	var2 = var2 + (((int64_t)dig_P4) << 35);
	var1 = ((var1 * var1 * (int64_t)dig_P3) >> 8) + ((var1 * (int64_t)dig_P2) << 12);
	var1 = (((((int64_t)1) << 47) + var1)) * ((int64_t)dig_P1) >> 33;

	if (var1 == 0)
	return 0;

	p = 1048576 - adc_P;
	p = (((p << 31) - var2) * 3125) / var1;
	var1 = (((int64_t)dig_P9) * (p >> 13) * (p >> 13)) >> 25;
	var2 = (((int64_t)dig_P8) * p) >> 19;
	p = ((p + var1 + var2) >> 8) + (((int64_t)dig_P7) << 4);

	return (int32_t)p / 256;
}

float BME280_readHumidity(void) {
	int32_t adc_H;

	I2C_start();
	I2C_write((BME280_ADDR << 1) | 0);
	I2C_write(BME280_REG_HUM_MSB);
	I2C_start();
	I2C_write((BME280_ADDR << 1) | 1);
	adc_H = ((int32_t)I2C_readAck() << 8) | I2C_readNack();
	I2C_stop();

	int32_t v_x1_u32r;

	v_x1_u32r = (t_fine - ((int32_t)76800));
	v_x1_u32r = (((((adc_H << 14) - (((int32_t)dig_H4) << 20) -
	(((int32_t)dig_H5) * v_x1_u32r)) +
	((int32_t)16384)) >>
	15) *
	(((((((v_x1_u32r * ((int32_t)dig_H6)) >> 10) *
	(((v_x1_u32r * ((int32_t)dig_H3)) >> 11) +
	((int32_t)32768))) >>
	10) +
	((int32_t)2097152)) *
	((int32_t)dig_H2) +
	8192) >>
	14));
	v_x1_u32r = v_x1_u32r -
	(((((v_x1_u32r >> 15) * (v_x1_u32r >> 15)) >> 7) *
	((int32_t)dig_H1)) >>
	4);
	v_x1_u32r = (v_x1_u32r < 0 ? 0 : v_x1_u32r);
	v_x1_u32r = (v_x1_u32r > 419430400 ? 419430400 : v_x1_u32r);
	float h = (v_x1_u32r >> 12);
	return h / 1024.0; // %HR
}
