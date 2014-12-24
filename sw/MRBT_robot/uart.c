/*
 * uart.c
 *
 * Created: 12. 3. 2013 7:33:13
 *  Author: Štefan Mišík
 */ 

#include "uart.h"



void uart_init(uint16_t ubrr)
{	
	fifo_init((fifo_t *)&uart_struct.rx_buffer, (char *)uart_struct.rx_data, RX_BUFFER_LEN);
	fifo_init((fifo_t *)&uart_struct.tx_buffer, (char *)uart_struct.tx_data, TX_BUFFER_LEN);
	uart_struct.last_rxc = 0;
	
	UBRRH = (char)(ubrr >> 8);
	UBRRL = (char)ubrr;
	
	/* 8 bit data, 1 stop bit, bez parity */
	UCSRB = _BV(RXEN) | _BV(TXEN);  
	UCSRC = _BV(URSEL) | _BV(UCSZ0) | _BV(UCSZ1);
	
	UCSRB |= (3 << TXCIE);
	sei();
}

/* Umoznuje poslat data na edrese pData, s dlzkou cLength*/
uint16_t uart_send(const void *buf, uint16_t nbytes)
{	
	char c;
	unsigned char b_sreg;
	b_sreg = SREG;
	cli();	
	nbytes = fifo_write((fifo_t *)&uart_struct.tx_buffer, buf, nbytes);	
	if(UCSRA & _BV(UDRE))
	{		
		if(fifo_getc((fifo_t *)&uart_struct.tx_buffer.buf, &c))
			UDR = c;
	}
	if((b_sreg) & (1 << SREG_I))	
		sei();
	return nbytes;	
}

uint16_t uart_receive(void *buf, uint16_t nbytes)
{	
	unsigned char b_sreg;
	b_sreg = SREG;
	cli();	
	nbytes = fifo_read((fifo_t *)&uart_struct.rx_buffer, buf, nbytes);
	if((b_sreg) & (1 << SREG_I))	
		sei();
	return nbytes;
}

short_time_t uart_since_last_rxc(void)
{
	unsigned char b_sreg;
	short_time_t t_result;
	b_sreg = SREG;
	cli();
	t_result = timer_struct.sys_time.t_short[0] - uart_struct.last_rxc;
	if((b_sreg) & (1 << SREG_I))
		sei();
	return t_result;	
}


/* Obsluzenie preruseni */
ISR(USART_RXC_vect)
{	
	fifo_putc((fifo_t *)&uart_struct.rx_buffer, UDR);
	uart_struct.last_rxc = timer_struct.sys_time.t_short[0];
	led_blink();
}

ISR(USART_TXC_vect)
{		
	char c;	
	while((UCSRA & _BV(UDRE)) == 0){} ;
	if(fifo_getc((fifo_t *)&uart_struct.tx_buffer, &c))
		UDR = c;
	led_blink();
}