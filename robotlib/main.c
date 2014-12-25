extern char **environ;

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>

#include "robot.h"

int main (int argc, char *argv[])
{  
    int i;
            
    robot_init();
   
        if( 0 != robot_connect("/dev/ttyUSB0")) /* treba zistit aka seriovka to je */
        {
            puts(strerror(errno));
            exit(EXIT_FAILURE);            
        }     
        
        robot_reset();
        sleep(1);                       /* Po restarte  treba chvilu pockat 1s je mozno aj vela,
                                         * atmega8 v sucasnom nastaveni startuje asi 65ms,
                                         * takze prvy packet by nestihol */
        
        robot_set_calibration(85, 192);	/* nastavenie spodnej a hornej hranice serva */
        robot_compose_packet();         /* Najskor packet vytvorit */
        robot_send_packet();            /* Potom odoslat */
        
        robot_set_servo(85);
        robot_compose_n_send();         /* Alebo vsetko v jednom */
        
        robot_set_motor_a(120);
        robot_set_motor_b(100);
        robot_set_motor_flags(ROBOT_MAEN | ROBOT_MBEN);
        robot_compose_packet();         /* Vytvorit, a potom sa pokusit odoslat */
        for(i = 0; i < 5; i ++)
        {
            if(ROBOT_ERR_CRC != robot_send_packet())
                break;        
        }       
        
        robot_disconnect();
      
    
    return (0);
}

