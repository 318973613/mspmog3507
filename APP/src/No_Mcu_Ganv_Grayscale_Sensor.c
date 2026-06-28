#include "No_Mcu_Ganv_Grayscale_Sensor_Config.h"
#include "interrupt_priority.h"

#include "FreeRTOS.h"
#include "task.h"

#include <string.h>

#define ADC_POLL_TIMEOUT_LOOPS 100000UL
#define ADC_NOTIFY_TIMEOUT_MS  2U

static volatile TaskHandle_t s_adc_wait_task = NULL;
static volatile uint16_t s_adc_last_result = 0U;

void Get_Analog_value(unsigned short *result)
{
    unsigned char i;
    unsigned char j;
    unsigned int analog_sum;

    for (i = 0; i < 8; i++) {
        Switch_Address_0(!(i & 0x01));
        Switch_Address_1(!(i & 0x02));
        Switch_Address_2(!(i & 0x04));

        analog_sum = 0U;
        for (j = 0; j < 8; j++) {
            analog_sum += adc_getValue();
        }

        if (!Direction) {
            result[i] = (unsigned short) (analog_sum / 8U);
        } else {
            result[7U - i] = (unsigned short) (analog_sum / 8U);
        }
    }
}

void convertAnalogToDigital(unsigned short *adc_value,
    unsigned short *Gray_white,
    unsigned short *Gray_black,
    unsigned char *Digital)
{
    for (int i = 0; i < 8; i++) {
        if (adc_value[i] > Gray_white[i]) {
            *Digital |= (1U << i);
        } else if (adc_value[i] < Gray_black[i]) {
            *Digital &= ~(1U << i);
        }
    }
}

void normalizeAnalogValues(unsigned short *adc_value,
    double *Normal_factor,
    unsigned short *Calibrated_black,
    unsigned short *result,
    double bits)
{
    for (int i = 0; i < 8; i++) {
        unsigned short normalized;

        if (adc_value[i] < Calibrated_black[i]) {
            normalized = 0U;
        } else {
            normalized = (unsigned short)
                ((adc_value[i] - Calibrated_black[i]) * Normal_factor[i]);
        }

        if (normalized > bits) {
            normalized = (unsigned short) bits;
        }
        result[i] = normalized;
    }
}

void No_MCU_Ganv_Sensor_Init_Frist(No_MCU_Sensor *sensor)
{
    memset(sensor->Calibrated_black, 0, sizeof(sensor->Calibrated_black));
    memset(sensor->Calibrated_white, 0, sizeof(sensor->Calibrated_white));
    memset(sensor->Normal_value, 0, sizeof(sensor->Normal_value));
    memset(sensor->Analog_value, 0, sizeof(sensor->Analog_value));

    for (int i = 0; i < 8; i++) {
        sensor->Normal_factor[i] = 0.0;
    }

    sensor->Digtal = 0U;
    sensor->Time_out = 0U;
    sensor->Tick = 0U;
    sensor->ok = 0U;
}

void No_MCU_Ganv_Sensor_Init(No_MCU_Sensor *sensor,
    unsigned short *Calibrated_white,
    unsigned short *Calibrated_black)
{
    double normal_diff[8];
    unsigned short temp;

    No_MCU_Ganv_Sensor_Init_Frist(sensor);

    if (Sensor_ADCbits == _8Bits) {
        sensor->bits = 255.0;
    } else if (Sensor_ADCbits == _10Bits) {
        sensor->bits = 1024.0;
    } else if (Sensor_ADCbits == _12Bits) {
        sensor->bits = 4096.0;
    } else if (Sensor_ADCbits == _14Bits) {
        sensor->bits = 16384.0;
    }

    if (Sensor_Edition == Class) {
        sensor->Time_out = 1U;
    } else {
        sensor->Time_out = 10U;
    }

    for (int i = 0; i < 8; i++) {
        if (Calibrated_black[i] >= Calibrated_white[i]) {
            temp = Calibrated_white[i];
            Calibrated_white[i] = Calibrated_black[i];
            Calibrated_black[i] = temp;
        }

        sensor->Gray_white[i] =
            (unsigned short) ((Calibrated_white[i] * 2U +
                               Calibrated_black[i]) / 3U);
        sensor->Gray_black[i] =
            (unsigned short) ((Calibrated_white[i] +
                               Calibrated_black[i] * 2U) / 3U);

        sensor->Calibrated_black[i] = Calibrated_black[i];
        sensor->Calibrated_white[i] = Calibrated_white[i];

        if (((Calibrated_white[i] == 0U) && (Calibrated_black[i] == 0U)) ||
            (Calibrated_white[i] == Calibrated_black[i])) {
            sensor->Normal_factor[i] = 0.0;
            continue;
        }

        normal_diff[i] =
            (double) Calibrated_white[i] - (double) Calibrated_black[i];
        sensor->Normal_factor[i] = sensor->bits / normal_diff[i];
    }

    sensor->ok = 1U;
}

void No_Mcu_Ganv_Sensor_Task_Without_tick(No_MCU_Sensor *sensor)
{
    Get_Analog_value(sensor->Analog_value);
    convertAnalogToDigital(sensor->Analog_value, sensor->Gray_white,
        sensor->Gray_black, &sensor->Digtal);
    normalizeAnalogValues(sensor->Analog_value, sensor->Normal_factor,
        sensor->Calibrated_black, sensor->Normal_value, sensor->bits);
}

void No_Mcu_Ganv_Sensor_Task_With_tick(No_MCU_Sensor *sensor)
{
    if (sensor->Tick >= sensor->Time_out) {
        No_Mcu_Ganv_Sensor_Task_Without_tick(sensor);
        sensor->Tick = 0U;
    }
}

void Task_tick(No_MCU_Sensor *sensor)
{
    sensor->Tick++;
}

unsigned char Get_Digtal_For_User(No_MCU_Sensor *sensor)
{
    return sensor->Digtal;
}

unsigned char Get_Normalize_For_User(No_MCU_Sensor *sensor,
    unsigned short *result)
{
    if (!sensor->ok) {
        return 0U;
    }

    memcpy(result, sensor->Normal_value, sizeof(sensor->Normal_value));
    return 1U;
}

unsigned char Get_Anolog_Value(No_MCU_Sensor *sensor, unsigned short *result)
{
    Get_Analog_value(sensor->Analog_value);
    memcpy(result, sensor->Analog_value, sizeof(sensor->Analog_value));

    if (!sensor->ok) {
        return 0U;
    }
    return 1U;
}

static void adc_start_single_conversion(void)
{
    DL_ADC12_clearInterruptStatus(ADC12_0_INST,
        DL_ADC12_INTERRUPT_MEM0_RESULT_LOADED |
        DL_ADC12_INTERRUPT_OVERFLOW |
        DL_ADC12_INTERRUPT_TRIG_OVF);
    DL_ADC12_enableConversions(ADC12_0_INST);
    DL_ADC12_startConversion(ADC12_0_INST);
}

static void adc_stop_single_conversion(void)
{
    DL_ADC12_stopConversion(ADC12_0_INST);
    DL_ADC12_disableConversions(ADC12_0_INST);
}

static unsigned int adc_getValue_polling(void)
{
    uint32_t timeout = ADC_POLL_TIMEOUT_LOOPS;

    adc_start_single_conversion();
    while ((DL_ADC12_getStatus(ADC12_0_INST) !=
            DL_ADC12_STATUS_CONVERSION_IDLE) &&
           (timeout > 0UL)) {
        timeout--;
    }

    if (timeout > 0UL) {
        s_adc_last_result = (uint16_t) DL_ADC12_getMemResult(
            ADC12_0_INST, DL_ADC12_MEM_IDX_0);
    }

    adc_stop_single_conversion();
    return s_adc_last_result;
}

unsigned int adc_getValue(void)
{
    uint32_t primask;

    if (xTaskGetSchedulerState() != taskSCHEDULER_RUNNING) {
        return adc_getValue_polling();
    }

    (void) ulTaskNotifyTake(pdTRUE, 0U);

    primask = __get_PRIMASK();
    __disable_irq();
    s_adc_wait_task = xTaskGetCurrentTaskHandle();
    if (primask == 0U) {
        __enable_irq();
    }

    DL_ADC12_enableInterrupt(ADC12_0_INST,
        DL_ADC12_INTERRUPT_MEM0_RESULT_LOADED |
        DL_ADC12_INTERRUPT_OVERFLOW |
        DL_ADC12_INTERRUPT_TRIG_OVF);
    NVIC_ClearPendingIRQ(ADC12_0_INST_INT_IRQN);
    NVIC_SetPriority(ADC12_0_INST_INT_IRQN, IRQ_PRIORITY_ADC_SAMPLE);
    NVIC_EnableIRQ(ADC12_0_INST_INT_IRQN);
    adc_start_single_conversion();

    if (ulTaskNotifyTake(pdTRUE,
            pdMS_TO_TICKS(ADC_NOTIFY_TIMEOUT_MS)) == 0U) {
        DL_ADC12_disableInterrupt(ADC12_0_INST,
            DL_ADC12_INTERRUPT_MEM0_RESULT_LOADED |
            DL_ADC12_INTERRUPT_OVERFLOW |
            DL_ADC12_INTERRUPT_TRIG_OVF);
        adc_stop_single_conversion();
    }

    primask = __get_PRIMASK();
    __disable_irq();
    s_adc_wait_task = NULL;
    if (primask == 0U) {
        __enable_irq();
    }

    return s_adc_last_result;
}

void ADC12_0_INST_IRQHandler(void)
{
    BaseType_t higher_priority_task_woken = pdFALSE;

    switch (DL_ADC12_getPendingInterrupt(ADC12_0_INST)) {
    case DL_ADC12_IIDX_MEM0_RESULT_LOADED:
        s_adc_last_result = (uint16_t) DL_ADC12_getMemResult(
            ADC12_0_INST, DL_ADC12_MEM_IDX_0);
        adc_stop_single_conversion();
        DL_ADC12_disableInterrupt(ADC12_0_INST,
            DL_ADC12_INTERRUPT_MEM0_RESULT_LOADED |
            DL_ADC12_INTERRUPT_OVERFLOW |
            DL_ADC12_INTERRUPT_TRIG_OVF);
        if (s_adc_wait_task != NULL) {
            vTaskNotifyGiveFromISR((TaskHandle_t) s_adc_wait_task,
                &higher_priority_task_woken);
        }
        portYIELD_FROM_ISR(higher_priority_task_woken);
        break;
    case DL_ADC12_IIDX_OVERFLOW:
    case DL_ADC12_IIDX_TRIG_OVERFLOW:
        adc_stop_single_conversion();
        DL_ADC12_disableInterrupt(ADC12_0_INST,
            DL_ADC12_INTERRUPT_MEM0_RESULT_LOADED |
            DL_ADC12_INTERRUPT_OVERFLOW |
            DL_ADC12_INTERRUPT_TRIG_OVF);
        if (s_adc_wait_task != NULL) {
            vTaskNotifyGiveFromISR((TaskHandle_t) s_adc_wait_task,
                &higher_priority_task_woken);
        }
        portYIELD_FROM_ISR(higher_priority_task_woken);
        break;
    default:
        break;
    }
}
