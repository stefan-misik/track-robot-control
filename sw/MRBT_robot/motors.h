/*
 * motors.h
 *
 * Created: 14. 3. 2013 12:29:04
 *  Author: Stefan
 */ 


#ifndef MOTORS_H_
#define MOTORS_H_

#include <avr/io.h>
#include <stdint.h>
#include "timer.h"

#define motors_set_a_pwm(duty_cycle)		OCR1BL = (~((uint8_t)(duty_cycle)))
#define motors_set_b_pwm(duty_cycle)		OCR1AL = (~((uint8_t)(duty_cycle)))
#define motors_set_s_pwm(duty_cycle)		OCR2 = (~((uint8_t)(duty_cycle)))

#define motors_enable_b()					PORTD |= _BV(7)
#define motors_disable_b()					PORTD &= ~_BV(7)
#define motors_enable_a()					PORTB |= _BV(5)
#define motors_disable_a()					PORTB &= ~_BV(5)

#define motors_reverse_b()				{							\
											PORTB |= _BV(0);		\
											TCCR1A &= ~_BV(COM1A0);	\
										}
											
#define motors_forth_b()				{							\
											PORTB &= ~_BV(0);		\
											TCCR1A |= _BV(COM1A0);	\
										}
											
#define motors_reverse_a()				{							\
											PORTB |= _BV(4);		\
											TCCR1A &= ~_BV(COM1B0);	\
										}
											
#define motors_forth_a()				{							\
											PORTB &= ~_BV(4);		\
											TCCR1A |= _BV(COM1B0);	\
										}

void motors_init(void);

#endif /* PWM_H_ */