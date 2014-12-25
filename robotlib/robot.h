/*
 * robot.h
 *
 *  Created: 16. 3. 2013 15:23:42
 *  Author: Stefan Misik
 * 
 * Kniznica na komunikaciu s robotom.
 */ 


#ifndef ROBOT_H_
#define ROBOT_H_

#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <stdint.h>
#include <string.h>


#define ROBOT_MAX_PACKET_LENGTH         15

#define ROBOT_ERR_UNSDL	0x01		/* Unsuported data length */
#define ROBOT_ERR_CRC	0x02		/* Checksum error */
#define ROBOT_ERR_TO	0x04		/* Timeout */

#define ROBOT_MAEN	0x80            /* Povolit motor A */
#define ROBOT_MBEN	0x40            /* Povolit motor B */
#define ROBOT_AREV	0x20            /* Reverzovat motor A */
#define ROBOT_BREV	0x10            /* Reverzovat motor B */

/* ------------------------------------------------------------------------- */

#define ROBOT_SRVA	0x80
#define ROBOT_MOTA	0x40
#define ROBOT_MOTB	0x20
#define ROBOT_MOTF	0x10
#define ROBOT_CAL	0x08
 
struct robotctl
{
    int 		fd;
    struct termios 	tio_backup;
    struct
    {
        uint8_t packet_flags;
        uint8_t servo;
        uint8_t motor_a;
        uint8_t motor_b;
        uint8_t motor_f;
        uint8_t cal_f;
        uint8_t cal_min;
        uint8_t cal_max;
    } packet;
    char tx_buffer[ROBOT_MAX_PACKET_LENGTH + 4];
    char rx_buffer[ROBOT_MAX_PACKET_LENGTH + 4];
} robot_ctl;

void robot_init(void);
int robot_connect(const char *port);
int robot_disconnect(void);

/* nasledujuce funkcie nastavia hodnotu, potom treba odoslat */
void robot_set_motor_a(uint8_t speed);
void robot_set_motor_b(uint8_t speed);
void robot_set_servo(uint8_t angle);
void robot_set_motor_flags(uint8_t flags);
void robot_set_calibration(uint8_t min, uint8_t max);
void robot_write_calibration(void);

/* Vytvorenie packetu do vnútornej pamäte systému */
int robot_compose_packet(void);

/* odoslanie, navratova hodnota je chybova sprava prijata zo zariadenia ak je
 *              0 = vsetko ok
 *              0xff = nieco sa velmi pokazilo
 *              ostatne = treba testovat chybove priznaky, su zadefinovane vyssie */
uint8_t robot_reset(void);
uint8_t robot_send_packet(void);

/* vytvorenie a odoslanie */
static inline uint8_t robot_compose_n_send(void)
{
    robot_compose_packet();
    return robot_send_packet();
}


#endif /* ROBOT_H_ */

/* POZNAMKA: 
 * Dovod, preco je vytvorenie packetu a jeho odoslanie oddelene je kvoli
 * situacii, kedy by niekto, chcel pravidelne odosielat packety v inom vlakne v 
 * nejakom casovom intervale, v tom pripade staci proti sucastnemu pristupu do 
 * zdielanych dat osetrit funkciu robot_compose_packet() a pri samotnom 
 * odosielani nehrozi kolizia (za predpokladu, ze robot_compose_packet() je
 * volana len v tomto vlakne. ALE POZOR: pri takejto konstrukcii treba pred 
 * odoslanim kontrolovat ci je co poslat (ci nie je navratova hodnota 
 * robot_compose_packet()nulova), lebo prazdny packet indikuje prikaz na
 * restartovanie robota.
 */