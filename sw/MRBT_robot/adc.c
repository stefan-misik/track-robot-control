/*
 * adc.c
 *
 * Created: 16. 3. 2013 15:23:55
 *  Author: Stefan
 */ 

#include "adc.h"


void adc_init(void)
{
	
	ADMUX = (3 << REFS0);
	ADMUX |= (0 << MUX0);
	ADCSRA = (4 << ADPS0);
	ADCSRA |= _BV(ADEN) | _BV(ADIE);	
	
	ADCSRA |= _BV(ADSC);
}

ISR(ADC_vect)
{
	if(ADMUX & _BV(MUX0))
		adc_data.sense_b = ADC;
	else
		adc_data.sense_a = ADC;	
	ADMUX ^= _BV(MUX0);	
	
	ADCSRA |= _BV(ADSC);	
}