#include "tb6612.h"
TB6612_HandleTypeDef htb6612;

#define MOTOR_PWM_LIMIT_ABS    (1000.0f)

static float Motor_SanitizeDuty(float duty)
{
    if (duty != duty) {
        return 0.0f;
    }
    if (duty > MOTOR_PWM_LIMIT_ABS) {
        return MOTOR_PWM_LIMIT_ABS;
    }
    if (duty < -MOTOR_PWM_LIMIT_ABS) {
        return -MOTOR_PWM_LIMIT_ABS;
    }
    return duty;
}

void GPIO_WritePin(uint8_t in1,uint8_t in2,Motor_TypeDef motor)
{
  if(motor == MOTOR_A)
  {
    in1==0?DL_GPIO_clearPins(GPIO_motor_PIN_AIN1_PORT, GPIO_motor_PIN_AIN1_PIN) : DL_GPIO_setPins(GPIO_motor_PIN_AIN1_PORT, GPIO_motor_PIN_AIN1_PIN);
    in2==0?DL_GPIO_clearPins(GPIO_motor_PIN_AIN2_PORT,GPIO_motor_PIN_AIN2_PIN) : DL_GPIO_setPins(GPIO_motor_PIN_AIN2_PORT,GPIO_motor_PIN_AIN2_PIN);
  }
  else 
  {
    in1==0?DL_GPIO_clearPins(GPIO_motor_PIN_BIN1_PORT, GPIO_motor_PIN_BIN1_PIN) : DL_GPIO_setPins(GPIO_motor_PIN_BIN1_PORT, GPIO_motor_PIN_BIN1_PIN);
    in2==0?DL_GPIO_clearPins(GPIO_motor_PIN_BIN2_PORT,GPIO_motor_PIN_BIN2_PIN) : DL_GPIO_setPins(GPIO_motor_PIN_BIN2_PORT,GPIO_motor_PIN_BIN2_PIN);
  }
}

void TB6612_Init(void)
{
    // 确保所有控制引脚为低电平
    DL_GPIO_clearPins(GPIO_motor_PIN_AIN1_PORT, GPIO_motor_PIN_AIN1_PIN);
    DL_GPIO_clearPins(GPIO_motor_PIN_AIN2_PORT, GPIO_motor_PIN_AIN2_PIN);
    DL_GPIO_clearPins(GPIO_motor_PIN_BIN1_PORT, GPIO_motor_PIN_BIN1_PIN);
    DL_GPIO_clearPins(GPIO_motor_PIN_BIN2_PORT, GPIO_motor_PIN_BIN2_PIN);
    
    // // 退出待机模式
    // DL_GPIO_setPins(htb6612->STBY_Port, htb6612->STBY_Pin, GPIO_PIN_SET);
    
    // 启动PWM
    DL_TimerA_startCounter(PWM_motor_INST);
    // 初始速度为0
    DL_TimerA_setCaptureCompareValue(PWM_motor_INST,0,GPIO_PWM_motor_C0_IDX);
    DL_TimerA_setCaptureCompareValue(PWM_motor_INST,0,GPIO_PWM_motor_C1_IDX);
}

// void motor_init(void)
// {
//     htb6612.AIN1_Port = GPIO_motor_PIN_AIN1_PORT;
//     htb6612.AIN1_Pin = GPIO_motor_PIN_AIN1_PIN;
//     htb6612.AIN2_Port =GPIO_motor_PIN_AIN2_PORT;
//     htb6612.AIN2_Pin = GPIO_motor_PIN_AIN2_PIN;
//     htb6612.BIN1_Port = GPIO_motor_PIN_BIN1_PORT;
//     htb6612.BIN1_Pin = GPIO_motor_PIN_BIN1_PIN;
//     htb6612.BIN2_Port = GPIO_motor_PIN_BIN2_PORT;
//     htb6612.BIN2_Pin = GPIO_motor_PIN_BIN2_PIN;
//     // htb6612.STBY_Port = GPIOB;
//     // htb6612.STBY_Pin = GPIO_PIN_15;
//     htb6612.PWM_Timer = PWM_motor_INST;
//     htb6612.PWM_A_Channel = GPIO_PWM_motor_C0_PIN;
//     htb6612.PWM_B_Channel = GPIO_PWM_motor_C1_PIN;
//     TB6612_Init(&htb6612);
// }
/**
  * @brief  设置电机方向
  * @param  htb6612: TB6612句柄指针
  * @param  motor: 电机编号(MOTOR_A或MOTOR_B)
  * @param  dir: 电机方向(MOTOR_CW, MOTOR_CCW或MOTOR_STOP)
  * @retval 无
  */
void TB6612_SetDirection(Motor_TypeDef motor, Motor_Direction dir)
{
   uint8_t in1, in2;
    
    switch(dir) {
        case MOTOR_STOP:
            in1 = 0;
            in2 = 0;
            break;
        case MOTOR_CW:
            in1 = 1;
            in2 = 0;
            break;
        case MOTOR_CCW:
            in1 = 0;
            in2 = 1;
            break;
        default:
            in1 = 0;
            in2 = 0;
            break;
    }
   
     GPIO_WritePin(in1,in2,motor);
   
}

/**
  * @brief  设置电机速度
  * @param  htb6612: TB6612句柄指针
  * @param  motor: 电机编号(MOTOR_A或MOTOR_B)
  * @param  speed: 电机速度(0-1000, 0表示停止，1000表示全速)
  * @retval 无
  */
void TB6612_SetSpeed( Motor_TypeDef motor, uint16_t speed)
{
    // 限制速度值在0-1000范围内
    if(speed > 1000) {
        speed = 1000;
    }
    if(motor == MOTOR_A) 
    {
        DL_TimerA_setCaptureCompareValue(PWM_motor_INST,speed,GPIO_PWM_motor_C0_IDX);
    } 
    else 
    {
       DL_TimerA_setCaptureCompareValue(PWM_motor_INST,speed,GPIO_PWM_motor_C1_IDX);
    }
}

// /**
//   * @brief  控制待机模式
//   * @param  htb6612: TB6612句柄指针
//   * @param  state: ENABLE-退出待机，DISABLE-进入待机
//   * @retval 无
//   */
// void TB6612_Standby(TB6612_HandleTypeDef* htb6612, FunctionalState state)
// {
//     HAL_GPIO_WritePin(htb6612->STBY_Port, htb6612->STBY_Pin, (state == ENABLE) ? GPIO_PIN_SET : GPIO_PIN_RESET);
// }

/**
  * @brief  综合控制电机方向和速度
  * @param  htb6612: TB6612句柄指针
  * @param  motor: 电机编号(MOTOR_A或MOTOR_B)
  * @param  dir: 电机方向(MOTOR_CW, MOTOR_CCW或MOTOR_STOP)
  * @param  speed: 电机速度(0-1000)
  * @retval 无
  */
void TB6612_MotorControl(Motor_TypeDef motor, Motor_Direction dir, uint16_t speed)
{
    TB6612_SetDirection(motor, dir);
    TB6612_SetSpeed( motor, speed);
}



//设置左电机的占空比
//duty - 占空比的具体值，范围-100.0f - 100.0f;
void APP_PWM_Set_L(float duty)
{
	float sign;
    duty = Motor_SanitizeDuty(duty);
	if(duty >=0)
		sign = 1;
	else
		sign = -1;
	duty = fabsf(duty);
	if(sign >= 0)
	{
		TB6612_SetDirection(MOTOR_A,MOTOR_CCW);
	}
	else 
	{
		TB6612_SetDirection(MOTOR_A,MOTOR_CW);
	}
	uint16_t ccr = duty;
	 DL_TimerA_setCaptureCompareValue(PWM_motor_INST, ccr,DL_TIMER_CC_1_INDEX);
}


//设置右电机的占空比
//duty - 占空比的具体值，范围-100.0f - 100.0f;
void APP_PWM_Set_R(float duty)
{
	float sign;
    duty = Motor_SanitizeDuty(duty);
	if(duty >=0)
		sign = 1;
	else
		sign = -1;
	duty = fabsf(duty);
	if(sign >= 0)
	{
		TB6612_SetDirection(MOTOR_B,MOTOR_CCW);
	}
	else 
	{
		TB6612_SetDirection(MOTOR_B,MOTOR_CW);
	}
	uint16_t ccr = duty;
	DL_TimerA_setCaptureCompareValue(PWM_motor_INST, ccr,DL_TIMER_CC_0_INDEX);
}

//小车原地右转(给ccr赋值）
//duty：ccr的值：0---1000.0f
void car_turnright(float duty)
{
	APP_PWM_Set_L(duty);
	APP_PWM_Set_R(-duty);
}

//小车行动函数（给ccr赋值）
//duty：ccr的值：-1000.0f -- 1000.0f
void car_action(float duty_l,float duty_r)
{
	APP_PWM_Set_L(duty_l);
	APP_PWM_Set_R(duty_r);
}




/**
  * @brief  小车直行
  * @param  htb6612: TB6612句柄指针
  * @param  speed: 电机速度(0-1000)
  * @retval 无
  */
void TB6612_MotorStraight(uint16_t speed)
{
	TB6612_MotorControl(MOTOR_A, MOTOR_CW, speed);
	TB6612_MotorControl(MOTOR_B, MOTOR_CW, speed);
	
}

/**
  * @brief  小车后退
  * @param  htb6612: TB6612句柄指针
  * @param  speed: 电机速度(0-1000)
  * @retval 无
  */
void TB6612_MotorBack(uint16_t speed)
{
	TB6612_MotorControl(MOTOR_A, MOTOR_CCW, speed);
	TB6612_MotorControl(MOTOR_B, MOTOR_CCW, speed);
	
}

/**
  * @brief  小车左转
  * @param  htb6612: TB6612句柄指针
  * @param  speed: 电机速度(0-1000)
  * @retval 无,
  */
void TB6612_MotorTurnLeft(uint16_t speed)
{
	TB6612_MotorControl(MOTOR_A, MOTOR_CCW, speed);
	TB6612_MotorControl(MOTOR_B, MOTOR_CW, speed);
	
}

/**
  * @brief  小车右转
  * @param  htb6612: TB6612句柄指针
  * @param  speed: 电机速度(0-1000)
  * @retval 无
  */
void TB6612_MotorTurnRight(uint16_t speed)
{
	TB6612_MotorControl(MOTOR_A, MOTOR_CW, speed);
	TB6612_MotorControl(MOTOR_B, MOTOR_CCW, speed);
	
}
/**
  * @brief  电机刹车
  * @param  htb6612: TB6612句柄指针
  * @param  motor: 电机编号(MOTOR_A或MOTOR_B)
  * @retval 无
  */
void TB6612_Brake(Motor_TypeDef motor)
{
    // 刹车时设置IN1和IN2都为高电平

        GPIO_WritePin(1,1,motor);

    
    // 设置PWM为0
    TB6612_SetSpeed(motor, 0);
}

/**
  * @brief  小车停止
  * @param  htb6612: TB6612句柄指针
  * @param  speed: 电机速度(0-1000)
  * @retval 无
  */
void TB6612_MotorBrake()
{
	 TB6612_Brake(MOTOR_A);
	 TB6612_Brake(MOTOR_B);
	
}





