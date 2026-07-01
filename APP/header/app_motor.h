#ifndef  app_motor_h
#define  app_motor_h

#include "main.h"



// extern PID_TypeDef pid_motor_l;  //左电机调速系统的PID控制器
// extern PID_TypeDef pid_motor_r;  //右电机调速系统的PID控制器
extern float omega_l;
extern float omega_r;
extern float out_l;
extern float out_r;
void App_Motor_init(void);  //初始化左右电机调速系统
void App_Motor_Proc(void);  //电机pid速度环调控
void App_Motor_Stop(void);
void angle_Motor_Proc(void); //电机pid角度环控制
void angle_Motor_LEFT(void);  //角度换左转
void App_motor_SetOmega_L(float Omega); //设置左电机速度
void App_motor_SetOmega_R(float Omega); //设置右电机速度
void App_motor_angle(float Omega);     //设置目标角度
//int Serial_Printf(UART_HandleTypeDef *huart, const char *format, ...); //串口打印函数
//void PID_PrintProc(void);   //PID参数打印
void car_turn(float arget);  //小车原地左右转（调速）
void car_come(float arget_l,float arget_r);//小车行动函数（直行，后退，右转，左转，停止);
void PID_PrintProc(void);
// void PID_Print(PID_TypeDef * pid);

#endif
