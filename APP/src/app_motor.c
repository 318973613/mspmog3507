#include "app_motor.h"
PID_TypeDef pid_motor_l;  //左电机调速系统的PID控制器
PID_TypeDef pid_motor_r;  //右电机调速系统的PID控制器
PID_TypeDef pid_angler;  //角度环调速系统的PID控制器
float omega_l;
float omega_r;
float out_l;
float out_r;
float angle_yaw;
float angler;
 void car_turn(float arget);
//初始化左右电机调速系统
//
void App_Motor_init(void)
{
	pid_init(&pid_motor_l,1.271,0.2,0);
	pid_init(&pid_motor_r,1.26,0.3,0);
	pid_init(&pid_angler,0.1,0,0);
}

//电机pid调控
void App_Motor_Proc(void)  
{
	//通过编码器获取左右电机的旋转角速度
	 omega_l =  encoder.left_MotorSpeed;
	 omega_r =  encoder.right_MotorSpeed;

	//计算PID控制器的输出
//	static uint32_t time = 0;
//    uint32_t current_time = HAL_GetTick();
//	if((current_time- time) >10)	
//	{
		 out_l = pid_compute(&pid_motor_l,omega_l);
		 out_r = pid_compute(&pid_motor_r,omega_r);
		APP_PWM_Set_L(out_l);
		APP_PWM_Set_R(out_r);
//		time=current_time;
//	}
		    
}

//双环控制 外环：角度环  
//函数作用通过角度控制速度
//右转
void angle_Motor_Proc(void) 
{
	//通过编码器获取左右电机的旋转角速度
	//    angle_yaw = yaw;
	   angler = pid_angle(&pid_angler,angle_yaw);  //角度环
      App_motor_SetOmega_L(-angler);
	  App_motor_SetOmega_R(angler);
//		time=current_time;
//	}
		    
}

//左转
void angle_Motor_LEFT(void) 
{
	//通过编码器获取左右电机的旋转角速度
	//    angle_yaw = yaw;
	   angler = pid_angle(&pid_angler,angle_yaw);  //角度环
      App_motor_SetOmega_L(-angler);
	  App_motor_SetOmega_R(angler);
//		time=current_time;
//	}
		    
}

//简介:用来设置左电机的转速Omega的值
//omeag a ：表示电机的转速，单位是rad/s
void App_motor_SetOmega_L(float Omega)
{
	pid_changeTarget(&pid_motor_l,Omega);
	
}

//简介：用来设置右电机的转速Omega的值
//omega：表示电机的转速，单位是rad/s
void App_motor_SetOmega_R(float Omega)
{
	pid_changeTarget(&pid_motor_r,Omega);
	
}

//简介：用来设置目标角度值
//omega：表示电机的转速，单位是rad/s
void App_motor_angle(float Omega)
{
	pid_changeTarget(&pid_angler,Omega);
	
}



/**
  * 函    数：基于HAL库的串口打印函数
  * 参    数：huart 串口句柄指针
  * 参    数：format 格式化字符串
  * 参    数：... 可变的参数列表
  * 返 回 值：实际发送的字符数，负数表示错误
  */
// int Serial_Printf(UART_HandleTypeDef *huart, const char *format, ...)
// {
//     static char String[256];  // 定义静态缓冲区
//     va_list arg;
//     int len;
    
//     va_start(arg, format);
//     len = vsnprintf(String, sizeof(String), format, arg);
//     va_end(arg);
    
//     if (len < 0) {
//         return -1;  // 格式化失败
//     }
    
//     // 处理截断情况
//     if (len >= sizeof(String)) {
//         len = sizeof(String) - 1;
//     }
    
//     // 发送数据（注意：len是实际写入的字符数，不包括\0）
//     if (HAL_UART_Transmit(huart, (uint8_t *)String, len, 1000) != HAL_OK) {
//         return -1;  // 发送失败
//     }
    
//     return len;  // 返回实际发送的字符数
// }

//将 PID 参数通过串口打印出来，便于调试
// void PID_PrintParams(PID_TypeDef * PID)
// {
// //    Serial_Printf(huart, "Kp=%.4f, Ki=%.4f, Kd=%.4f\r\n", 
// //                  pid->kp, pid->ki, pid->kd);
//     printf( "%.2f,%.2f,%.2f,%.2f\r\n", 
//                   pid->target,omega_l,out_l,pid->errorint);
// }
void PID_Print(PID_TypeDef * pid)
{
	printf("%.2f,%.2f,%.2f,%.2f\r\n",pid->target,omega_l,out_l,pid->errorint);
}
              

void PID_PrintProc(void)
{
	PID_Print(&pid_motor_l);
}

//小车原地左右转(pid调速）
//arget：ccr的值：-60-60f   正值：右转  负值：左转
void car_turn(float arget)
{
	App_motor_SetOmega_L(arget);
	App_motor_SetOmega_R(-arget);
}

//小车行动函数（pid调速）
//duty：ccr的值：-60.0f -- 60.0f
void car_come(float arget_l,float arget_r)  
{
	App_motor_SetOmega_L(arget_l);
	App_motor_SetOmega_R(arget_r);
}



