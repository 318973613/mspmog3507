#include "FreeRTOS.h"
#include "task.h"
#include "tb6612.h"

volatile const char *g_assert_file = 0;
volatile unsigned long g_assert_line = 0;

void vAssertCalled(const char *file, unsigned long line)
{
    g_assert_file = file;
    g_assert_line = line;

    TB6612_MotorBrake();
    taskDISABLE_INTERRUPTS();
    for (;;) {
    }
}

void vApplicationMallocFailedHook(void)
{
    TB6612_MotorBrake();
    taskDISABLE_INTERRUPTS();
    for (;;) {
    }
}

void vApplicationStackOverflowHook(TaskHandle_t task, char *taskName)
{
    (void) task;
    (void) taskName;

    TB6612_MotorBrake();
    taskDISABLE_INTERRUPTS();
    for (;;) {
    }
}
