/*
 * adc.h
 *
 * Created: 16. 3. 2013 15:23:42
 *  Author: Stefan
 */ 


#ifndef ADC_H_
#define ADC_H_


#include <avr/io.h>
#include <avr/interrupt.h>

struct 
{
	uint16_t sense_a;
	uint16_t sense_b;	
}adc_data;

void adc_init(void);



#endif /* ADC_H_ */