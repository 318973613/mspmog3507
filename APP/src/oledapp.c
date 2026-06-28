#include "oledapp.h"
char buffer[50];

void oled_show(void)
{
    sprintf(buffer,"s_l:%.2f s_r:%.2f", encoder.left_MotorSpeed,encoder.right_MotorSpeed);
    OLED_ShowString(0,0,(uint8_t *)buffer,8);

    // sprintf(buffer,"A:%d-%d-%d-%d-%d-%d-%d-%d",Anolog[0],Anolog[1],Anolog[2],Anolog[3],Anolog[4],Anolog[5],Anolog[6],Anolog[7]);
    // OLED_ShowString(0,1,(uint8_t *)buffer,8);

    // sprintf(buffer,"W:%d-%d-%d-%d-%d-%d-%d-%d",white[0],white[1],white[2],white[3],white[4],white[5],white[6],white[7]);
    // OLED_ShowString(0,3,(uint8_t *)buffer,8);

    // sprintf(buffer,"B:%d-%d-%d-%d-%d-%d-%d-%d",black[0],black[1],black[2],black[3],black[4],black[5],black[6],black[7]);
    // OLED_ShowString(0,5,(uint8_t *)buffer,8);
}