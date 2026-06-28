/*
 * FreeRTOS configuration for the MSPM0G3507 smart car project.
 *
 * The RTOS tick is driven by TIMG12 in rtos_tick_timer.c. SysTick is left
 * unused so it cannot conflict with legacy delay code or SysConfig output.
 */

#ifndef FREERTOS_CONFIG_H
#define FREERTOS_CONFIG_H

#include "ti_msp_dl_config.h"

void vAssertCalled(const char *file, unsigned long line);

#define configENABLE_MPU                                0

#ifdef CPUCLK_FREQ
#define configCPU_CLOCK_HZ                              ((unsigned long) CPUCLK_FREQ)
#else
#define configCPU_CLOCK_HZ                              ((unsigned long) 80000000UL)
#endif

#define configTICK_RATE_HZ                              1000
#define configUSE_PREEMPTION                            1
#define configUSE_TIME_SLICING                          1
#define configUSE_PORT_OPTIMISED_TASK_SELECTION         0
#define configUSE_TICKLESS_IDLE                         0
#define configMAX_PRIORITIES                            5
#define configMINIMAL_STACK_SIZE                        ((unsigned short) 128)
#define configMAX_TASK_NAME_LEN                         16
#define configTICK_TYPE_WIDTH_IN_BITS                   TICK_TYPE_WIDTH_32_BITS
#define configIDLE_SHOULD_YIELD                         1
#define configTASK_NOTIFICATION_ARRAY_ENTRIES           1
#define configQUEUE_REGISTRY_SIZE                       0
#define configENABLE_BACKWARD_COMPATIBILITY             0
#define configUSE_MINI_LIST_ITEM                        1
#define configSTACK_DEPTH_TYPE                          size_t
#define configMESSAGE_BUFFER_LENGTH_TYPE                size_t

#define configUSE_TIMERS                                0
#define configUSE_EVENT_GROUPS                          0
#define configUSE_STREAM_BUFFERS                        0
#define configUSE_CO_ROUTINES                           0
#define configMAX_CO_ROUTINE_PRIORITIES                 1

#define configSUPPORT_STATIC_ALLOCATION                 0
#define configSUPPORT_DYNAMIC_ALLOCATION                1
#define configTOTAL_HEAP_SIZE                           ((size_t) (10U * 1024U))
#define configAPPLICATION_ALLOCATED_HEAP                0
#define configSTACK_ALLOCATION_FROM_SEPARATE_HEAP       0

#ifdef __NVIC_PRIO_BITS
#define configPRIO_BITS                                 __NVIC_PRIO_BITS
#else
#define configPRIO_BITS                                 2
#endif

#define configLIBRARY_LOWEST_INTERRUPT_PRIORITY         3
#define configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY    1
#define configKERNEL_INTERRUPT_PRIORITY                 (configLIBRARY_LOWEST_INTERRUPT_PRIORITY << (8 - configPRIO_BITS))
#define configMAX_SYSCALL_INTERRUPT_PRIORITY            (configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY << (8 - configPRIO_BITS))

#define configUSE_IDLE_HOOK                             0
#define configUSE_TICK_HOOK                             0
#define configUSE_MALLOC_FAILED_HOOK                    1
#define configCHECK_FOR_STACK_OVERFLOW                  2
#define configUSE_TRACE_FACILITY                        0
#define configUSE_STATS_FORMATTING_FUNCTIONS            0
#define configGENERATE_RUN_TIME_STATS                   0
#define configCHECK_HANDLER_INSTALLATION                0

#define configUSE_TASK_NOTIFICATIONS                    1
#define configUSE_MUTEXES                               0
#define configUSE_RECURSIVE_MUTEXES                     0
#define configUSE_COUNTING_SEMAPHORES                   0
#define configUSE_QUEUE_SETS                            0
#define configUSE_APPLICATION_TASK_TAG                  0

#define INCLUDE_vTaskPrioritySet                        0
#define INCLUDE_uxTaskPriorityGet                       0
#define INCLUDE_vTaskDelete                             0
#define INCLUDE_vTaskSuspend                            0
#define INCLUDE_xResumeFromISR                          0
#define INCLUDE_vTaskDelayUntil                         1
#define INCLUDE_vTaskDelay                              1
#define INCLUDE_xTaskGetSchedulerState                  1
#define INCLUDE_xTaskGetCurrentTaskHandle               1
#define INCLUDE_uxTaskGetStackHighWaterMark             0
#define INCLUDE_xTaskGetIdleTaskHandle                  0
#define INCLUDE_eTaskGetState                           0
#define INCLUDE_xEventGroupSetBitFromISR                0
#define INCLUDE_xTimerPendFunctionCall                  0
#define INCLUDE_xTaskAbortDelay                         0
#define INCLUDE_xTaskGetHandle                          0
#define INCLUDE_xTaskResumeFromISR                      0

#define configASSERT(x)                     \
    do {                                    \
        if ((x) == 0) {                     \
            vAssertCalled(__FILE__, __LINE__); \
        }                                   \
    } while (0)

#endif
