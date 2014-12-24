/*
 * MRBT_robot.c
 *
 * Created: 13. 3. 2013 22:19:04
 *  Author: Stefan
 */ 
#define F_CPU 20000000UL

#include "timer.h"
#include "led.h"
#include "calibration.h"
#include "uart.h"
#include "motors.h"
#include "protocol.h"

/*#include <avr/sleep.h>*/


int main (void)
{
	/*set_sleep_mode(SLEEP_MODE_IDLE);	*/
	
	calibration_read();
	timer_init(195, TPS_1024);
	led_init();
	uart_init(BAUD_CALC(9600));
	motors_init();	
	protocol_init();
	
				
	while (1)
	{	

		protocol_main_loop_hook();
		/*sleep_mode();*/
	}
}