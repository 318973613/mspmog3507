/*
 * Copyright (c) 2021, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "ti_msp_dl_config.h"
#include "main.h"

#include "FreeRTOS.h"
#include "task.h"

#define MOTOR_TASK_PERIOD_MS       (20U)
#define TRACK_TASK_PERIOD_MS       (20U)
#define OLED_TASK_PERIOD_MS        (100U)
#define KEY_LED_TASK_PERIOD_MS     (20U)

#define MOTOR_TASK_STACK_WORDS     (192U)
#define TRACK_TASK_STACK_WORDS     (192U)
#define OLED_TASK_STACK_WORDS      (384U)
#define KEY_LED_TASK_STACK_WORDS   (128U)

#define MOTOR_TASK_PRIORITY        (3U)
#define TRACK_TASK_PRIORITY        (2U)
#define OLED_TASK_PRIORITY         (1U)
#define KEY_LED_TASK_PRIORITY      (1U)

static void Board_Init(void);
static void MotorControlTask(void *argument);
static void LineTrackTask(void *argument);
static void OledTask(void *argument);
static void KeyLedTask(void *argument);
static void CreateApplicationTasks(void);

int main(void)
{
    Board_Init();
    CreateApplicationTasks();
    vTaskStartScheduler();

    TB6612_MotorBrake();
    while (1) {
    }
}

static void Board_Init(void)
{
    SYSCFG_DL_init();
    uart0_init();
    Interrupt_Init();
    TB6612_Init();
    trctive_init();
    encoder_init();
    App_Motor_init();
    car_control_init();
}

static void CreateApplicationTasks(void)
{
    BaseType_t ok = pdPASS;

    if (xTaskCreate(MotorControlTask, "motor",
        MOTOR_TASK_STACK_WORDS, NULL, MOTOR_TASK_PRIORITY, NULL) != pdPASS) {
        ok = pdFAIL;
    }
    if (xTaskCreate(LineTrackTask, "track",
        TRACK_TASK_STACK_WORDS, NULL, TRACK_TASK_PRIORITY, NULL) != pdPASS) {
        ok = pdFAIL;
    }
    if (xTaskCreate(OledTask, "oled",
        OLED_TASK_STACK_WORDS, NULL, OLED_TASK_PRIORITY, NULL) != pdPASS) {
        ok = pdFAIL;
    }
    if (xTaskCreate(KeyLedTask, "keyled",
        KEY_LED_TASK_STACK_WORDS, NULL, KEY_LED_TASK_PRIORITY, NULL) != pdPASS) {
        ok = pdFAIL;
    }

    if (ok != pdPASS) {
        TB6612_MotorBrake();
        while (1) {
        }
    }
}

static void MotorControlTask(void *argument)
{
    (void) argument;
    TickType_t lastWake = xTaskGetTickCount();

    while (1) {
        motor_getspeed(&encoder);
        if (car_control_is_running() != 0U) {
            App_Motor_Proc();
        } else {
            App_Motor_Stop();
        }
        vTaskDelayUntil(&lastWake, pdMS_TO_TICKS(MOTOR_TASK_PERIOD_MS));
    }
}

static void LineTrackTask(void *argument)
{
    (void) argument;
    TickType_t lastWake = xTaskGetTickCount();

    while (1) {
        question1(3);
        vTaskDelayUntil(&lastWake, pdMS_TO_TICKS(TRACK_TASK_PERIOD_MS));
    }
}

static void OledTask(void *argument)
{
    (void) argument;
    TickType_t lastWake;

    OLED_Init();
    OLED_Clear();
    lastWake = xTaskGetTickCount();

    while (1) {
        oled_show();
        vTaskDelayUntil(&lastWake, pdMS_TO_TICKS(OLED_TASK_PERIOD_MS));
    }
}

static void KeyLedTask(void *argument)
{
    (void) argument;
    TickType_t lastWake = xTaskGetTickCount();

    while (1) {
        led_task();
        vTaskDelayUntil(&lastWake, pdMS_TO_TICKS(KEY_LED_TASK_PERIOD_MS));
    }
}
