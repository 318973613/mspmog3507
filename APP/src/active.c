#include "active.h"



void question1(uint8_t number)
{
    static uint8_t star_flag=0;
    extern uint8_t black_turn;      //转弯标志（需要指定变量类型）
    uint8_t turn_around;     //转圈圈数（需要指定变量类型）
    static uint8_t turn_num = 0;    //转弯的个数
    static uint8_t stop_detected = 0; // 停止检测标记，确保只检测一次
    turn_around = number;
    
    // 只有在未检测过停止状态且满足停止条件时才执行
    if(black_turn == 4 && star_flag == 0 && stop_detected == 0)
    {
        stop_detected = 1;  // 标记为已检测，防止再次进入
        turn_num++;
        
        if(turn_around == 0)
        {
            car_come(0, 0);  // 停止小车
            return;
        }
        else if((turn_num % 4) == 0)  // 修复变量名拼写错误 turn_nuum -> turn_num
        {
            turn_around--;
        }
    }
    else if(star_flag == 1)  
    {
        // 当star_flag为1时的处理逻辑
        stop_detected = 0;  // 重置检测标记，允许下次检测
    }

    motor_trctive();    //循迹函数
}




