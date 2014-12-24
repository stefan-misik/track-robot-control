/*
 * fifoBuffer.h
 *
 * Created: 13. 3. 2013 8:22:45
 *  Author: Stefan
 */ 


#ifndef FIFOBUFFER_H_
#define FIFOBUFFER_H_
#include <stdint.h>

typedef struct
{
	char	   *buf;
	uint16_t	head;
	uint16_t	tail;
	uint16_t	size;	
}fifo_t;

void fifo_init(fifo_t *f, char *buf, uint16_t size);
uint16_t fifo_read(fifo_t *f, void *buf, uint16_t nbytes);
uint16_t fifo_write(fifo_t *f, const void *buf, uint16_t nbytes);
char fifo_getc(fifo_t *f, char *c);
char fifo_putc(fifo_t *f, char c);


#endif /* FIFOBUFFER_H_ */