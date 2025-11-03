#ifndef adc_H_
#define adc_H_

#include <avr/io.h>

void ADC_init(void);
uint16_t ADC_read(uint8_t channel);



#endif /* adc_H_ */