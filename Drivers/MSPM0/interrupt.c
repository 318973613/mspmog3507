#include "interrupt.h"
#include "interrupt_priority.h"


void Interrupt_Init(void)
{
    NVIC_SetPriority(GPIO_encoder_left_INT_IRQN, IRQ_PRIORITY_ENCODER);
    NVIC_SetPriority(GPIO_encoder_right_INT_IRQN, IRQ_PRIORITY_ENCODER);
    NVIC_SetPriority(ADC12_0_INST_INT_IRQN, IRQ_PRIORITY_ADC_SAMPLE);
    NVIC_SetPriority(UART_0_INST_INT_IRQN, IRQ_PRIORITY_UART_RX);
    NVIC_SetPriority(I2C_OLED_INST_INT_IRQN, IRQ_PRIORITY_I2C_OLED);
    NVIC_SetPriority(TIMER_0_INST_INT_IRQN, IRQ_PRIORITY_UNUSED_TIMER);

    DL_ADC12_disableInterrupt(ADC12_0_INST,
        DL_ADC12_INTERRUPT_MEM0_RESULT_LOADED |
        DL_ADC12_INTERRUPT_OVERFLOW |
        DL_ADC12_INTERRUPT_TRIG_OVF);
    NVIC_DisableIRQ(ADC12_0_INST_INT_IRQN);

    DL_I2C_disableInterrupt(I2C_OLED_INST,
        DL_I2C_INTERRUPT_CONTROLLER_TX_DONE |
        DL_I2C_INTERRUPT_CONTROLLER_NACK |
        DL_I2C_INTERRUPT_CONTROLLER_ARBITRATION_LOST);
    NVIC_DisableIRQ(I2C_OLED_INST_INT_IRQN);

    NVIC_DisableIRQ(TIMER_0_INST_INT_IRQN);
    DL_TimerA_disableInterrupt(TIMER_0_INST, DL_TIMERA_INTERRUPT_ZERO_EVENT);
    DL_TimerA_stopCounter(TIMER_0_INST);
}

void GROUP1_IRQHandler(void)
{
     encoder_proc();
}

void TIMER_0_INST_IRQHandler(void)
{
    (void) DL_TimerA_getPendingInterrupt(TIMER_0_INST);
}
