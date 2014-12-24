/*
 * uart.h
 *
 *  Created: 12. 3. 2013 7:33:25
 *  Author: Štefan Mišík
 *  
 *	
 */ 


#ifndef UART_H_
#define UART_H_

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdint.h>
#include "utils/fifo_buffer.h"
#include "led.h"

#define TX_BUFFER_LEN 64
#define RX_BUFFER_LEN 64

#define BAUD_CALC(baudrate)		((F_CPU / ((baudrate) * 16UL)) - 1)
#define TX_BUFFER_EMPTY()		(uart_struct.tx_buffer.head == uart_struct.tx_buffer.tail)


volatile struct
{
	fifo_t			rx_buffer;
	fifo_t			tx_buffer;	
	char			tx_data[TX_BUFFER_LEN];
	char			rx_data[RX_BUFFER_LEN];	
	short_time_t	last_rxc;
} uart_struct;


void uart_init(uint16_t ubrr);
uint16_t uart_send(const void *buf, uint16_t nbytes);
uint16_t uart_receive(void *buf, uint16_t nbytes);
short_time_t uart_since_last_rxc(void);

#endif /* UART_H_ */