/*
 * protocol.c
 *
 * Created: 19. 3. 2013 20:31:31
 *  Author: Stefan
 */ 

#include "protocol.h"

uint8_t calc_ceck_sum(void *data, uint8_t length, uint8_t carry)
{	
	uint8_t i;
	for(i = 0; i < length; i ++)
		carry += ((uint8_t *)data)[i];
	
	return ~carry;
}

void protocol_init(void)
{
	protocol_data.packet_length = 0;
	protocol_data.received		= 0;
	protocol_data.state			= RCV_HDR_1;
}

void protocol_send_response(void)
{	
	protocol_data.buffer[0] = 0xFF;
	protocol_data.buffer[1] = 0xFF;
	protocol_data.buffer[2] = 1;
	protocol_data.buffer[3] = protocol_data.error_flags;
	protocol_data.buffer[4] = ~(1 + protocol_data.error_flags);
	uart_send(protocol_data.buffer, 5);	
}

void protocol_execute_packet(void)
{
	uint8_t pos = 0;
	
		
	
	if((protocol_data.buffer[pos ++] & 0x07) == 0)
	{
		if(pos == protocol_data.packet_length)
		{
			if(protocol_data.buffer[0] == 0)
			{
				protocol_send_response();
				
				while(1) {
					cli();
					if(TX_BUFFER_EMPTY())
						break;
					sei();
				}
									
				WDTCR = _BV(WDCE);
				WDTCR = _BV(WDCE) | _BV(WDE);				
				while(1){ ; };
			}
			else
				return;
		}				
		if(protocol_data.buffer[0] & _BV(PROT_SRVA))
		{
			motors_set_s_pwm(constrain(protocol_data.buffer[pos], calibration_data.servo_min_value, calibration_data.servo_max_value));
			pos ++;
			if(pos == protocol_data.packet_length)
				return;
		}
		if(protocol_data.buffer[0] & _BV(PROT_MOTA))
		{
			motors_set_a_pwm(protocol_data.buffer[pos ++]);
			if(pos == protocol_data.packet_length)
				return;
		}
		if(protocol_data.buffer[0] & _BV(PROT_MOTB))
		{
			motors_set_b_pwm(protocol_data.buffer[pos ++]);
			if(pos == protocol_data.packet_length)
				return;
		}
		if(protocol_data.buffer[0] & _BV(PROT_MOTF))
		{			
			if(protocol_data.buffer[pos] & _BV(PROT_MAEN))
				motors_enable_a();
			else
				motors_disable_a();
			
			if(protocol_data.buffer[pos] & _BV(PROT_MBEN))
				motors_enable_b();
			else
				motors_disable_b();
				
			if(protocol_data.buffer[pos] & _BV(PROT_AREV))
			{
				motors_reverse_a();		
			}							
			else
			{
				motors_forth_a();
			}				
			
			if(protocol_data.buffer[pos] & _BV(PROT_BREV))
			{
				motors_reverse_b();
			}				
			else
			{
				motors_forth_b();	
			}						
			
			pos ++;
			if(pos == protocol_data.packet_length)
				return;
		}
		if(protocol_data.buffer[0] & _BV(PROT_CAL))
		{
			uint8_t cal_flags = protocol_data.buffer[pos ++];
			
			if((cal_flags & 0x7f) == 2)
			{
				calibration_data.servo_min_value = protocol_data.buffer[pos ++];
				calibration_data.servo_max_value = protocol_data.buffer[pos ++];
			}
			
			if(cal_flags >> 7)
				calibration_write();			
			
			if(pos == protocol_data.packet_length)
				return;
		}		
	}
		
}


void protocol_main_loop_hook(void)
{
	switch(protocol_data.state)
	{				
		case RCV_HDR_1:
		switch(uart_receive(protocol_data.buffer, 2))
		{
			case 1:
			if(protocol_data.buffer[0] == 0xFF)
			{				
				protocol_data.state = RCV_HDR_2;
			}				
			break;
			case 2:
			if((protocol_data.buffer[0] == 0xFF) && (protocol_data.buffer[1] == 0xFF))
			{				
				protocol_data.state = RCV_LEN;
				protocol_data.error_flags = 0;
			}							
			break;
		}	
		break;
		case RCV_HDR_2:		
		if(uart_receive(protocol_data.buffer, 1))
		{
			if(protocol_data.buffer[0] == 0xFF)
			{				
				protocol_data.state = RCV_LEN;
				protocol_data.error_flags = 0;
			}				
			else
				protocol_data.state = RCV_HDR_1;
		}				
		break;
		case RCV_LEN:	
		if(uart_since_last_rxc() > PACKET_TIMEOUT)
		{
			protocol_data.error_flags |= _BV(PROT_ERR_TO);
			protocol_send_response();
			protocol_data.state = RCV_HDR_1;
		}
		if(uart_receive(&protocol_data.packet_length, 1))
		{			
			protocol_data.received = 0;
			if(protocol_data.packet_length > MAX_PACKET_LENGTH)
			{				
				protocol_data.error_flags |= _BV(PROT_ERR_UNSDL);
				protocol_send_response();
				protocol_data.state = RCV_HDR_1;
			}
			else if(protocol_data.packet_length == 0)
			{				
				protocol_send_response();
				protocol_data.state = RCV_HDR_1;				
			}
			else
			{				
				protocol_data.state = RCV_DATA;
			}				
		}
		break;
		case RCV_DATA:	
		if(uart_since_last_rxc() > PACKET_TIMEOUT)
		{
			protocol_data.error_flags |= _BV(PROT_ERR_TO);
			protocol_send_response();
			protocol_data.state = RCV_HDR_1;
		}	
		protocol_data.received += uart_receive((protocol_data.buffer + protocol_data.received),
			(protocol_data.packet_length - protocol_data.received));
		if(protocol_data.received == protocol_data.packet_length)
			protocol_data.state = RCV_CRC;
		break;
		case RCV_CRC:
		if(uart_since_last_rxc() > PACKET_TIMEOUT)
		{
			protocol_data.error_flags |= _BV(PROT_ERR_TO);
			protocol_send_response();
			protocol_data.state = RCV_HDR_1;
		}		
		if(uart_receive(&protocol_data.received, 1))
		{
			if(protocol_data.received == calc_ceck_sum(protocol_data.buffer, 
				protocol_data.packet_length, protocol_data.packet_length))
			{				
				protocol_execute_packet();
				protocol_send_response();				
				protocol_data.state = RCV_HDR_1;
			}				
			else
			{
				protocol_data.error_flags |= _BV(PROT_ERR_CRC);
				protocol_send_response();				
				protocol_data.state = RCV_HDR_1;
			}				
		}
		break;		
	}
}
