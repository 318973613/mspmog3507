#include "clock.h"

#include "FreeRTOS.h"
#include "task.h"

volatile uint32_t uwTick = 0;
volatile uint32_t delayTick = 0;


void SysTick_Init(void)
{
    /* FreeRTOS owns the system tick. This function is kept for compatibility. */
}


/**
 * @brief systick毫秒设置
 * 
 */
void SysTick_Increasment(void)
{
    uwTick++;
    if (delayTick) delayTick--;
}

/**
 * @brief 获取时基
 * 
 * @return uint32_t uwTick
 */
uint32_t Sys_GetTick(void)
{
    if (xTaskGetSchedulerState() != taskSCHEDULER_NOT_STARTED) {
        return (uint32_t) (xTaskGetTickCount() * portTICK_PERIOD_MS);
    }

    return uwTick;
}

/**
 * @brief 获取时间戳
 * 
 * @param timestamp 
 * @return uint32_t 
 * @note 移植DMP库需要的函数
 */
uint32_t mspm0_get_clock_ms(unsigned long *times)
{
    *times = Sys_GetTick();
    return *times;
}

/**
 * @brief 微秒级延时
 * 
 * @param xms 延时时间
 */
void mspm0_delay_ms(unsigned long xms)
{
    if (xTaskGetSchedulerState() != taskSCHEDULER_NOT_STARTED) {
        vTaskDelay(pdMS_TO_TICKS(xms));
        return;
    }

    while (xms--) {
        delay_cycles(CPUCLK_FREQ / 1000U);
    }
}
