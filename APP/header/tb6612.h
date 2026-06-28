#ifndef tb6612_h
#define tb6612_h


#ifdef __cplusplus
extern "C" {
#endif


#include "main.h"
// 电机编号定义
typedef enum {
    MOTOR_A = 0,
    MOTOR_B = 1
} Motor_TypeDef;

// 电机旋转方向定义
typedef enum {
    MOTOR_STOP = 0,
    MOTOR_CW = 1,    // 顺时针
    MOTOR_CCW = 2    // 逆时针
} Motor_Direction;

// TB6612配置结构体
typedef struct {
    // 控制引脚 (使用DriverLib兼容类型)
    GPIO_Regs *  AIN1_Port;
    uint32_t AIN1_Pin;      // 修改为uint32_t
    GPIO_Regs * AIN2_Port;
    uint32_t AIN2_Pin;      // 修改为uint32_t
    GPIO_Regs *BIN1_Port;
    uint32_t BIN1_Pin;      // 修改为uint32_t
    GPIO_Regs *BIN2_Port;
    uint32_t BIN2_Pin;      // 修改为uint32_t
     
    // PWM相关 (使用DriverLib定时器类型)
   GPTIMER_Regs  * PWM_Timer;  // MSPM0G3507的定时器类型
    uint32_t PWM_A_Channel; // 定时器通道 (如DL_TIMER_CH_0)
    uint32_t PWM_B_Channel; // 定时器通道 (如DL_TIMER_CH_1)
} TB6612_HandleTypeDef;
extern TB6612_HandleTypeDef htb6612;

// 初始化函数
void TB6612_Init(void);
// void motor_init(void);
// 基础控制函数
void TB6612_SetDirection(Motor_TypeDef motor, Motor_Direction dir);
void TB6612_SetSpeed( Motor_TypeDef motor, uint16_t speed);
//void TB6612_Standby(TB6612_HandleTypeDef* htb6612, FunctionalState state);

// 高级控制函数
void TB6612_MotorControl( Motor_TypeDef motor, Motor_Direction dir, uint16_t speed);
void TB6612_Brake( Motor_TypeDef motor);
void TB6612_MotorStraight(uint16_t speed);  //小车直行
void TB6612_MotorBack(uint16_t speed);	  //小车后退
void TB6612_MotorTurnRight(uint16_t speed); //小车右转
void TB6612_MotorTurnLeft(uint16_t speed);  //小车左转
void TB6612_MotorBrake();     //小车刹车
void APP_PWM_Set_L(float duty);   //设置左电机占空比  用于pid
void APP_PWM_Set_R(float duty);	  //设置右电机占空比  用于pid
void car_turnright(float duty);     //小车原地右转
void car_action(float duty_l,float duty_r); //小车行动（包括左转，右转，直行，后退）
#ifdef __cplusplus
}
#endif






#endif



