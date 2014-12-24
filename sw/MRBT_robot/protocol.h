/*
 * protocol.h
 *
 * Created: 19. 3. 2013 20:31:16
 *  Author: Stefan
 */ 


#ifndef PROTOCOL_H_
#define PROTOCOL_H_


#include <stdint.h>

#include "uart.h"
#include "led.h"
#include "timer.h"
#include "motors.h"
#include "calibration.h"

#define MAX_PACKET_LENGTH	15
#define PACKET_TIMEOUT		10	/* 10 * 10 = 100 ms */

#define PROT_ERR_UNSDL	0		/* Unsuported data length */
#define PROT_ERR_CRC	1		/* Checksum error */
#define PROT_ERR_TO		2		/* Timeout */

#define PROT_SRVA		7
#define PROT_MOTA		6
#define PROT_MOTB		5
#define PROT_MOTF		4
#define PROT_CAL		3

#define PROT_MAEN		7
#define PROT_MBEN		6
#define PROT_AREV		5
#define PROT_BREV		4

#define constrain(amt,low,high) ((amt)<(low)?(low):((amt)>(high)?(high):(amt)))

typedef enum
{
	RCV_HDR_1, RCV_HDR_2, RCV_LEN, RCV_DATA, RCV_CRC
}protocol_state;

struct 
{
	protocol_state	state;
	uint8_t			packet_length;
	uint8_t			received;
	char			buffer[MAX_PACKET_LENGTH];
	uint8_t			error_flags;
}protocol_data;

uint8_t calc_ceck_sum(void *data, uint8_t length, uint8_t carry);
void protocol_init(void);
void protocol_main_loop_hook(void);






#endif /* PROTOCOL_H_ */