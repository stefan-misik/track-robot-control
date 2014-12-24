/*
 * calibration.h
 *
 * Created: 20. 3. 2013 18:47:18
 *  Author: Stefan
 */ 


#ifndef CALIBRATION_H_
#define CALIBRATION_H_

#include <avr/io.h>
#include <stdint.h>
#include <avr/interrupt.h>

#define EEPROM_ADDR 0x0000

#define calibration_read()	eeprom_read(&calibration_data, EEPROM_ADDR, sizeof(calibration_data))
#define calibration_write()	eeprom_write(&calibration_data, EEPROM_ADDR, sizeof(calibration_data))

struct  
{
	uint8_t servo_min_value;
	uint8_t servo_max_value;
}calibration_data;

void eeprom_read(void* data, uint16_t eeprom_start, uint16_t length);
void eeprom_write(void* data, uint16_t eeprom_start, uint16_t length);


#endif /* CALIBRATION_H_ */