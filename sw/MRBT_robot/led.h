/*
 * led.h
 *
 * Created: 13. 3. 2013 21:39:17
 *  Author: Stefan
 */ 


#ifndef LED_H_
#define LED_H_
#include <avr/io.h>
#include "timer.h"


void led_init(void);
void led_blink(void);


#endif /* LED_H_ */