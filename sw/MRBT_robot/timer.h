/*
 * timer.h
 *
 * Created: 12. 3. 2013 16:49:37
 *  Author: Stefan
 */ 


#ifndef TIMER_H_
#define TIMER_H_

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdint.h>

#define TPS_1		0x01
#define TPS_8		0x02
#define TPS_64		0x03
#define TPS_256		0x04
#define TPS_1024	0x05

#define DISABLE_TIMER()		timer_struct.disable_timer = 1
#define ENABLE_TIMER()		timer_struct.disable_timer = 0

typedef uint16_t short_time_t;
typedef uint32_t long_time_t;
typedef void (*AlarmCallback)(void);
typedef struct _alarm_t
{	
	struct _alarm_t	   *next_alarm;
	AlarmCallback		callback;
	short_time_t		alarm_time;	
} alarm_t;
typedef union
{
	long_time_t		t_long;
	short_time_t	t_short[sizeof(long_time_t)/sizeof(short_time_t)];
} time_t;	

volatile struct 
{
	alarm_t	       *first_alarm;
	uint8_t			disable_timer;
	uint8_t			init_value;
	short_time_t	tmp_time;
	time_t			sys_time;
} timer_struct;



void timer_init(uint8_t unit, uint8_t prescale);
void timer_set(alarm_t *alarm);
void timer_unset(alarm_t *alarm);

#endif /* TIMER_H_ */