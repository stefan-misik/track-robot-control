/*
 * CFile1.c
 *
 * Created: 20. 3. 2013 18:47:36
 *  Author: Stefan
 */ 

#include "calibration.h"

void eeprom_read(void* data, uint16_t eeprom_start, uint16_t length)
{
	uint16_t i;
	for(i = 0; i < length; i ++)
	{
		while(EECR & _BV(EEWE)) { };
		
		EEAR = (eeprom_start + i);
		EECR |= _BV(EERE);
		((char *) data)[i] = EEDR;
	}
}

void eeprom_write(void* data, uint16_t eeprom_start, uint16_t length)
{
	uint16_t i;
	unsigned char b_sreg;
	
	b_sreg = SREG;
	cli();
	
	for(i = 0; i < length; i ++)
	{
		while(EECR & _BV(EEWE)) { };
	
		EEAR = (eeprom_start + i);
		EEDR = ((char *) data)[i];
		EECR |= _BV(EEMWE);
		EECR |= _BV(EEWE);			
	}
	if((b_sreg) & (1 << SREG_I))
		sei();
}
