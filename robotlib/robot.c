#include "robot.h"

/*void * robot_com_thread(void *data)
{
    struct robotctl *rcs = (struct robotctl *)data;

    pthread_exit((void *)0);
}*/

void robot_init(void)
{
    robot_ctl.fd = -1;
    memset(&robot_ctl.packet, 0, sizeof(robot_ctl.packet));
}

int robot_connect(const char *comport_name)
{
    int fd;
    struct termios newtio;
    if(robot_ctl.fd == -1)
    {
        fd = open(comport_name, O_RDWR | O_NOCTTY);
        if(fd < 0)
        {
            robot_ctl.fd = fd;
            return fd;
        }

        tcgetattr(fd, &robot_ctl.tio_backup);

        memset(&newtio, 0, sizeof(newtio));
        newtio.c_cflag = B9600 | CS8 | CLOCAL | CREAD;
        newtio.c_iflag = IGNPAR;
        newtio.c_oflag = 0;
        newtio.c_lflag = 0;

        newtio.c_cc[VTIME] = 1;
        newtio.c_cc[VMIN]  = 0;

        tcflush(fd, TCIFLUSH);
        if(-1 == tcsetattr(fd,TCSANOW,&newtio))
        {
            close(fd);
            robot_ctl.fd = -1;
            return -1;
        }

        robot_ctl.fd = fd;        
        return 0;
    }
    else
        return -1;    
}

int robot_disconnect(void)
{
    if(robot_ctl.fd >= 0)
    {
        tcflush(robot_ctl.fd, TCIFLUSH);
        tcsetattr(robot_ctl.fd,TCSANOW,&robot_ctl.tio_backup);
        close(robot_ctl.fd);
        robot_ctl.fd = -1;
        return 0;
    }
    else
        return -1;
}

void robot_set_motor_a(uint8_t speed)
{
    robot_ctl.packet.packet_flags |= ROBOT_MOTA;
    robot_ctl.packet.motor_a = speed;    
}

void robot_set_motor_b(uint8_t speed)
{
    robot_ctl.packet.packet_flags |= ROBOT_MOTB;
    robot_ctl.packet.motor_b = speed;
}

void robot_set_servo(uint8_t angle)
{
    robot_ctl.packet.packet_flags |= ROBOT_SRVA;
    robot_ctl.packet.servo = angle;
}

void robot_set_motor_flags(uint8_t flags)
{
    robot_ctl.packet.packet_flags |= ROBOT_MOTF;
    robot_ctl.packet.motor_f = flags;
}

void robot_set_calibration(uint8_t min, uint8_t max)
{
    robot_ctl.packet.packet_flags |= ROBOT_CAL;
    robot_ctl.packet.cal_max = max;
    robot_ctl.packet.cal_min = min;
    robot_ctl.packet.cal_f &= 0x80;
    robot_ctl.packet.cal_f |=  2;
}

void robot_write_calibration(void)
{
    robot_ctl.packet.cal_f |= 0x80;
}

uint8_t robot_reset(void)
{
    robot_ctl.packet.packet_flags = 0;    
    return robot_compose_n_send();
}

uint8_t _robot_calc_crc(char *packet)
{
    int i;
    uint8_t result = packet[2];
    
    if(result > ROBOT_MAX_PACKET_LENGTH)
        return 0;
    
    for(i = 0; i < packet[2]; i ++)
        result += packet[3 + i];
    
    return ~result;        
}

int robot_compose_packet(void)
{
     int pos = 0;    
    
    robot_ctl.tx_buffer[pos ++] = 0xff;
    robot_ctl.tx_buffer[pos ++] = 0xff;
    robot_ctl.tx_buffer[pos ++] = 0;
    
    
    robot_ctl.tx_buffer[pos ++] = robot_ctl.packet.packet_flags;

    if(robot_ctl.packet.packet_flags & ROBOT_SRVA)
        robot_ctl.tx_buffer[pos ++] = robot_ctl.packet.servo;
    if(robot_ctl.packet.packet_flags & ROBOT_MOTA)
        robot_ctl.tx_buffer[pos ++] = robot_ctl.packet.motor_a;
    if(robot_ctl.packet.packet_flags & ROBOT_MOTB)
        robot_ctl.tx_buffer[pos ++] = robot_ctl.packet.motor_b;
    if(robot_ctl.packet.packet_flags & ROBOT_MOTF)
        robot_ctl.tx_buffer[pos ++] = robot_ctl.packet.motor_f;
    if(robot_ctl.packet.packet_flags & ROBOT_CAL)    {
        robot_ctl.tx_buffer[pos ++] = robot_ctl.packet.cal_f;
        if((robot_ctl.packet.cal_f & 0x7f) == 2)
        {
           robot_ctl.tx_buffer[pos ++] = robot_ctl.packet.cal_min;
           robot_ctl.tx_buffer[pos ++] = robot_ctl.packet.cal_max;
        }
    }   
    
    robot_ctl.tx_buffer[2] = (uint8_t)(pos-3);
    robot_ctl.tx_buffer[pos] = _robot_calc_crc(robot_ctl.tx_buffer);
    
    robot_ctl.packet.cal_f = 0;
    robot_ctl.packet.packet_flags = 0;
    return (pos-3);
}

uint8_t _robot_get_response(void)
{
    unsigned int offset = 0;
    int i;
    
    for(i = 0; i < 5; i ++)
    {
        offset += read(robot_ctl.fd, (void *)((unsigned int)(robot_ctl.rx_buffer) + offset),
                (size_t)(5 - offset));
        if(offset == 5)
            break;
        else if(offset > 5)
            return 0xff;
    }
    if(robot_ctl.rx_buffer[0] == 0xff && robot_ctl.rx_buffer[1] == 0xff 
            && robot_ctl.rx_buffer[2] == 1)
    {
        if(_robot_calc_crc(robot_ctl.rx_buffer) == robot_ctl.rx_buffer[4])
            return robot_ctl.rx_buffer[3];       
    }   
    return 0xff;   
}
 
uint8_t robot_send_packet(void)
{
    uint8_t len = ((uint8_t)robot_ctl.tx_buffer[2] + 4);
    
    if(len > ROBOT_MAX_PACKET_LENGTH || len <= 4)
        return 0xff;
    
    if(len != write(robot_ctl.fd, robot_ctl.tx_buffer, len))
        return 0xff;
    
    return (int)_robot_get_response();
}

