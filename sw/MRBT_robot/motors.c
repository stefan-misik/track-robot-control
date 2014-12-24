/*
 * motors.c
 *
 * Created: 14. 3. 2013 12:29:15
 *  Author: Stefan
 */ 

#include "motors.h"

alarm_t servo_alarm;

void servo_run_alarm(void)
{
	TCCR2 |= ((6 << CS20));
}

void motors_init(void)
{	
	DDRB |= 0x3F;
	DDRD |= 0x80;
	
	servo_alarm.callback = servo_run_alarm;
	
	TCCR1B &= ~(7 << CS10);	/* Zastav citac */
	TCCR2 &= ~(7 << CS20);
	
	TCNT1 = 0;
	TCNT2 = 0;
		
	TCCR1A |= (3 << COM1A0) | (3 << COM1B0);
	TCCR2 |= (3 << COM20);
	
	TCCR1A |= _BV(WGM10);	/* Fast PWM, TOP = 0x00FF*/
	TCCR1A &= ~_BV(WGM11);
	TCCR1B |= _BV(WGM12);
	TCCR1B &= ~_BV(WGM13);
	TCCR2 |= (_BV(WGM21) | _BV(WGM20));
	
	OCR1A = 0x00FF;
	OCR1B = 0x00FF;	
	OCR2 = 0x74;
	
	TIMSK |= _BV(TOIE2);
	
	TCCR1B |= _BV(CS11);	/* Prescaler 1/8 */	
	
	servo_alarm.alarm_time=2;
	timer_set(&servo_alarm);	
}

/* Obsluha prerusenia */
ISR(TIMER2_OVF_vect)
{
	TCCR2 &= ~(7 << CS20);
	TCNT2 = 0;
	
	servo_alarm.alarm_time=2;
	timer_set(&servo_alarm);		
}	