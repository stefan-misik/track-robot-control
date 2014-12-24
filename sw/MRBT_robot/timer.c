/*
 * timer.c
 *
 * Created: 12. 3. 2013 16:49:53
 *  Author: Stefan
 */ 

#include "timer.h"


void timer_init(uint8_t unit, uint8_t prescale)
{
	timer_struct.init_value = 0xFF - unit;
	timer_struct.tmp_time = 0;
	timer_struct.first_alarm = (alarm_t *)0;
	timer_struct.disable_timer = 0;
	timer_struct.sys_time.t_long = 0;
	
	TCCR0 &= 0xF8;		/* Vynulovanie predelièky = zastavenie èasovaèa */
	TCNT0 = timer_struct.init_value;	/* Nastavenie poèiatoènej hodnoty */
	sei();								/* Globálne povolenie prerušení */
	TIMSK |= (_BV(TOIE0));	/* Povolenie prerušenia od preteèenia èasovaèa */
	TCCR0 |= (prescale & 0x07);	/* Nastavenie predelièky frekvencie */
}

void timer_set(alarm_t *alarm)
{
	alarm_t *p_alarm;
	short_time_t now;
	unsigned char b_sreg;
	
	DISABLE_TIMER();
	b_sreg = SREG;
	cli();
	now = timer_struct.tmp_time;
	if((b_sreg) & (1 << SREG_I))
		sei();
	
	if((alarm->alarm_time + now) < now)
	{
		p_alarm = (alarm_t *)&timer_struct;
		while(p_alarm->next_alarm != (alarm_t *)0)
		{
			p_alarm = p_alarm->next_alarm;
			if(p_alarm->alarm_time < now)
				p_alarm->alarm_time = 0;
			else
				p_alarm->alarm_time -= now;
		}
		now = 0;
		b_sreg = SREG;
		cli();
		timer_struct.tmp_time = 0;
		if((b_sreg) & (1 << SREG_I))
			sei();
	}	
	else
		alarm->alarm_time += now;
		
	p_alarm = (alarm_t *)&timer_struct;
	while(p_alarm->next_alarm != (alarm_t *)0)
	{
		if(p_alarm->next_alarm == alarm)
		{
			p_alarm->next_alarm = alarm->next_alarm;
			break;
		}
		p_alarm = p_alarm->next_alarm;
	}
	
	p_alarm = (alarm_t *)&timer_struct;	
	while(p_alarm->next_alarm != (alarm_t *)0)
	{		
		if(p_alarm->next_alarm->alarm_time > alarm->alarm_time)
			break;
					
		p_alarm = p_alarm->next_alarm;
	}
	
	alarm->next_alarm = p_alarm->next_alarm;
	p_alarm->next_alarm = alarm;	
	ENABLE_TIMER();	
}

void timer_unset(alarm_t *alarm)
{
	alarm_t *p_alarm;
	
	DISABLE_TIMER();
	p_alarm = (alarm_t *)&timer_struct;
	while(p_alarm->next_alarm != (alarm_t *)0)
	{
		if(p_alarm->next_alarm == alarm)
		{		
			p_alarm->next_alarm = alarm->next_alarm;
			break;
		}
		p_alarm = p_alarm->next_alarm;
	}					
	
	ENABLE_TIMER();
}


/* Obsluha prerusenia */
ISR(TIMER0_OVF_vect)
{
	alarm_t *p_alarm;	
	short_time_t now;
	AlarmCallback callback;
	
	timer_struct.sys_time.t_long ++;
	timer_struct.tmp_time ++;	
	if(timer_struct.disable_timer == 0)
	{			
		timer_struct.disable_timer = 2;
		now = timer_struct.tmp_time;
		sei();
		p_alarm = (alarm_t *)&timer_struct;
		while(p_alarm->next_alarm != (alarm_t *)0)
		{
			if(p_alarm->next_alarm->alarm_time <= now)
			{
				callback = p_alarm->next_alarm->callback;
				p_alarm->next_alarm = p_alarm->next_alarm->next_alarm;
				callback();				
			}
			p_alarm = p_alarm->next_alarm;
		}
		cli();
		if(timer_struct.disable_timer == 2)
			timer_struct.disable_timer = 0;		
	}		
			
	TCNT0 = timer_struct.init_value;	
}
