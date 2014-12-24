/*
 * fifoBuffer.c
 *
 * Created: 13. 3. 2013 8:23:05
 *  Author: Stefan
 */ 

#include "fifo_buffer.h"

void fifo_init(fifo_t *f, char *buf, uint16_t size){
	f->head = 0;
	f->tail = 0;
	f->size = size;
	f->buf = buf;
}


uint16_t fifo_read(fifo_t *f, void *buf, uint16_t nbytes)
{
	uint16_t i;
	
	for(i = 0; i < nbytes; i ++)
	{
		if(f->tail != f->head)		
		{
			((char *)buf)[i] = f->buf[f->tail ++];
			if(f->tail == f->size)
				f->tail = 0;
		}
		else
			return i;
	}
	return nbytes;
}

uint16_t fifo_write(fifo_t *f, const void *buf, uint16_t nbytes)
{
	uint16_t i, next_head;
	
	for(i = 0; i < nbytes; i ++)
	{
		next_head = f->head + 1;
		if(next_head == f->size)
			next_head = 0;
			
		if(next_head != f->tail)
		{
			f->buf[f->head] = ((char *)buf)[i];
			f->head = next_head;
		}
		else
			return i;		
	}
	return nbytes;
}

char fifo_getc(fifo_t *f, char *c)
{
	if(f->tail != f->head)
	{
		*c = f->buf[f->tail ++];
		if(f->tail == f->size)
		f->tail = 0;
		return 1;
	}
	else
		return 0;	
}

char fifo_putc(fifo_t *f, char c)
{
	uint16_t next_head = f->head + 1;
	if(next_head == f->size)
	next_head = 0;
	
	if(next_head != f->tail)
	{
		f->buf[f->head] = c;
		f->head = next_head;
		return 1;
	}
	else
		return 0;
}