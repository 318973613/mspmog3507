#include "app_trc.h"

unsigned short Anolog[8] = {0};
unsigned short white[8] = {3100, 3020, 3200, 3200, 3400, 3220, 3000, 2800};
unsigned short black[8] = {2400, 2400, 2800, 3000, 2300, 2500, 2700, 2100};
unsigned short Normal[8];
unsigned char rx_buff[256] = {0};
No_MCU_Sensor sensor;
unsigned char Digtal;
uint8_t black_turn = 0;

static float trc_clampf(float value, float min_value, float max_value)
{
    if (value > max_value) {
        return max_value;
    }
    if (value < min_value) {
        return min_value;
    }
    return value;
}

void trctive_init(void)
{
    DL_ADC12_initSingleSample(ADC12_0_INST,
        DL_ADC12_REPEAT_MODE_ENABLED, DL_ADC12_SAMPLING_SOURCE_AUTO,
        DL_ADC12_TRIG_SRC_SOFTWARE, DL_ADC12_SAMP_CONV_RES_12_BIT,
        DL_ADC12_SAMP_CONV_DATA_FORMAT_UNSIGNED);

    No_MCU_Ganv_Sensor_Init_Frist(&sensor);
    No_Mcu_Ganv_Sensor_Task_Without_tick(&sensor);
    Get_Anolog_Value(&sensor, Anolog);
    No_MCU_Ganv_Sensor_Init(&sensor, white, black);
}

void trctive_read(void)
{
    No_Mcu_Ganv_Sensor_Task_Without_tick(&sensor);
}

void motor_trctive(void)
{
    unsigned char sensor_state[8];
    unsigned char black_pos[8];
    unsigned char black_count = 0;
    float black_center;
    float deviation;
    int sum = 0;
    const float base_speed = 300.0f;
    float left;
    float right;

    No_Mcu_Ganv_Sensor_Task_Without_tick(&sensor);

    uint8_t data = Get_Digtal_For_User(&sensor);
    for (int i = 0; i < 8; i++) {
        sensor_state[i] = (data >> i) & 0x01;
        if (sensor_state[i] == 0) {
            black_pos[black_count++] = i;
        }
    }

    black_turn = black_count;

    /*
     * No black sensor means the line is lost. Stop instead of dividing by zero
     * or feeding invalid speed targets into the motor PID loop.
     */
    if (black_count == 0) {
        car_come(0, 0);
        return;
    }

    /*
     * All black is usually an intersection or marker. Move slowly forward so
     * the car does not aggressively steer while the line center is ambiguous.
     */
    if (black_count == 8) {
        car_come(150, 150);
        return;
    }

    for (int i = 0; i < black_count; i++) {
        sum += black_pos[i];
    }

    black_center = (float) sum / (float) black_count;
    deviation = black_center - 3.5f;

    left = base_speed - (deviation * 50.0f);
    right = base_speed + (deviation * 50.0f);

    left = trc_clampf(left, -600.0f, 600.0f);
    right = trc_clampf(right, -600.0f, 600.0f);

    car_come(right, left);
}
