#include "FreeRTOS.h"
#include "task.h"
#include "interrupt_priority.h"
#include "ti_msp_dl_config.h"

#define RTOS_TICK_TIMER_INST          (TIMG12)
#define RTOS_TICK_TIMER_IRQN          (TIMG12_INT_IRQn)
#define RTOS_TICK_TIMER_CLK_HZ        (CPUCLK_FREQ / 2UL)
#define RTOS_TICK_TIMER_LOAD_VALUE    ((RTOS_TICK_TIMER_CLK_HZ / configTICK_RATE_HZ) - 1UL)

void vPortSetupTimerInterrupt(void)
{
    static const DL_TimerG_ClockConfig tickClockConfig = {
        .clockSel    = DL_TIMER_CLOCK_BUSCLK,
        .divideRatio = DL_TIMER_CLOCK_DIVIDE_1,
        .prescale    = 0U,
    };

    const DL_TimerG_TimerConfig tickTimerConfig = {
        .period     = RTOS_TICK_TIMER_LOAD_VALUE,
        .timerMode  = DL_TIMER_TIMER_MODE_PERIODIC,
        .startTimer = DL_TIMER_STOP,
    };

    DL_TimerG_reset(RTOS_TICK_TIMER_INST);
    DL_TimerG_enablePower(RTOS_TICK_TIMER_INST);
    delay_cycles(POWER_STARTUP_DELAY);

    DL_TimerG_setClockConfig(RTOS_TICK_TIMER_INST,
        (DL_TimerG_ClockConfig *) &tickClockConfig);
    DL_TimerG_initTimerMode(RTOS_TICK_TIMER_INST,
        (DL_TimerG_TimerConfig *) &tickTimerConfig);
    DL_TimerG_enableInterrupt(RTOS_TICK_TIMER_INST,
        DL_TIMERG_INTERRUPT_ZERO_EVENT);
    DL_TimerG_enableClock(RTOS_TICK_TIMER_INST);
    DL_Timer_setCoreHaltBehavior(RTOS_TICK_TIMER_INST,
        DL_TIMER_CORE_HALT_IMMEDIATE);

    NVIC_ClearPendingIRQ(RTOS_TICK_TIMER_IRQN);
    NVIC_SetPriority(RTOS_TICK_TIMER_IRQN,
        IRQ_PRIORITY_RTOS_TICK);
    NVIC_EnableIRQ(RTOS_TICK_TIMER_IRQN);

    DL_TimerG_startCounter(RTOS_TICK_TIMER_INST);
}

void TIMG12_IRQHandler(void)
{
    if (DL_TimerG_getPendingInterrupt(RTOS_TICK_TIMER_INST) ==
        DL_TIMER_IIDX_ZERO) {
        uint32_t previousMask = portSET_INTERRUPT_MASK_FROM_ISR();

        if (xTaskIncrementTick() != pdFALSE) {
            portYIELD_FROM_ISR(pdTRUE);
        }

        portCLEAR_INTERRUPT_MASK_FROM_ISR(previousMask);
    }
}
