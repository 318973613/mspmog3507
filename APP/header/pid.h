#ifndef pid_h
#define pid_h
#include "main.h"



typedef struct
{
	float kp ; //比例系数
	float ki;  //积分项的系数
	float kd;  //微分项的系数
	float target; //目标值
	float errorint; //积分值
	float error2;  //上上次PID的差值
	float error1; //上次运行PID的误差	
	float error0; //目前误差
}PID_TypeDef;



float pid_angle(PID_TypeDef*PID,float actual);
void pid_init(PID_TypeDef *PID,float kp,float ki,float kd);
void pid_reset(PID_TypeDef *PID);
void pid_changeTarget(PID_TypeDef *PID,float target);
float pid_compute(PID_TypeDef*PID,float actual);
float pid_duoji(PID_TypeDef*PID,float actual);
void App_duoji_angle(PID_TypeDef * PID,float Omega); //多级目标值设置函数


#endif

