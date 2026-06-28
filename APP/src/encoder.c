#include "encoder.h"
#include "interrupt_priority.h"

encoderData_t encoder;

void encoder_init(void)
{
    NVIC_SetPriority(GPIO_encoder_left_INT_IRQN, IRQ_PRIORITY_ENCODER);
    NVIC_SetPriority(GPIO_encoder_right_INT_IRQN, IRQ_PRIORITY_ENCODER);
    NVIC_EnableIRQ(GPIO_encoder_left_INT_IRQN);
    NVIC_EnableIRQ(GPIO_encoder_right_INT_IRQN);

    NVIC_SetPriority(TIMER_0_INST_INT_IRQN, IRQ_PRIORITY_UNUSED_TIMER);
    NVIC_DisableIRQ(TIMER_0_INST_INT_IRQN);
    DL_TimerA_stopCounter(TIMER_0_INST);
}

void encoder_proc(void)
{
    uint32_t gpioB = DL_GPIO_getEnabledInterruptStatus(GPIOB,GPIO_encoder_right_PIN_rightA_PIN);
    uint32_t gpioA = DL_GPIO_getEnabledInterruptStatus(GPIOA,GPIO_encoder_left_PIN_leftA_PIN);

        if (gpioA & GPIO_encoder_left_PIN_leftA_PIN) //左轮
        {
            if (DL_GPIO_readPins(GPIOB,GPIO_encoder_left_PIN_leftB_PIN)) 
             (encoder.left_count)++;
            else  (encoder.left_count)--;
             DL_GPIO_clearInterruptStatus(GPIOA,GPIO_encoder_left_PIN_leftA_PIN);
        }
        if (gpioB & GPIO_encoder_right_PIN_rightA_PIN) //右轮
        {
            if (DL_GPIO_readPins(GPIOA,GPIO_encoder_right_PIN_rightB_PIN)) 
            (encoder.right_count)--;
            else   (encoder.right_count)++;
            DL_GPIO_clearInterruptStatus(GPIOB,GPIO_encoder_right_PIN_rightA_PIN);
        }
}

void motor_getspeed(encoderData_t * encoderdat)
{
    int32_t left_count;
    int32_t right_count;
    uint32_t primask = __get_PRIMASK();

    __disable_irq();
    left_count = encoder.left_count;
    right_count = encoder.right_count;
    encoder.left_count = 0;
    encoder.right_count = 0;
    if (primask == 0U) {
        __enable_irq();
    }

    encoderdat->left_MotorSpeed =
        left_count / PULSE_PRE_ROUND / MOTOR_SPEED_RERATIO /
        ENCODER_SAMPLE_PERIOD_S * 360.0f;
    encoderdat->right_MotorSpeed =
        right_count / PULSE_PRE_ROUND / MOTOR_SPEED_RERATIO /
        ENCODER_SAMPLE_PERIOD_S * 360.0f;

}









