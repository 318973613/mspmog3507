#ifndef encoder_h
#define encoder_h

#include "main.h"


#define MOTOR_SPEED_RERATIO 28u    //电机减速比
#define PULSE_PRE_ROUND 13.0f //一圈多少个脉冲

#define ENCODER_SAMPLE_PERIOD_S 0.02f

typedef struct {
uint8_t left_Dierction;         //方向
int32_t left_count;          //左电机总计数值
float left_MotorSpeed;		 //左电机速度(度/s)
float left_distance;	     //左电机位移(m)
int32_t lastcount_l;         //上一次计数值

uint8_t right_Dierction;     //方向
int32_t right_count;         //右电机总计数值
float right_MotorSpeed;		 //右电机速度(度/s)
float right_distance;	     //右电机位移(m)
int32_t lastcount_r;         //上一次计数值

}encoderData_t;
extern encoderData_t encoder;
void encoder_init(void);
void encoder_proc(void);
void encoder_clear_counts(void);
void motor_getspeed(encoderData_t * encoderdat);

#endif



