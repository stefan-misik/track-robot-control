/*
 * led.c
 *
 * Created: 13. 3. 2013 21:39:33
 *  Author: Stefan
 */ 

#include "led.h"

alarm_t led_alarm;

void led_alarm_callback(void)
{
	PORTC |= _BV(5);
}

void led_init(void)
{
	DDRC |= _BV(5);
	PORTC |= _BV(5);
	led_alarm.callback = led_alarm_callback;
}

void led_blink(void)
{
	PORTC &= ~_BV(5);
	led_alarm.alarm_time = 3;
	timer_set(&led_alarm);
}

