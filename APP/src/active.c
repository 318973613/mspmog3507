#include "active.h"

static volatile CarMode_t s_car_mode = CAR_MODE_IDLE;

static void car_control_stop_to_mode(CarMode_t mode)
{
    s_car_mode = mode;
    car_come(0.0f, 0.0f);
    App_Motor_Stop();
    encoder_clear_counts();
}

static void car_control_update_leds(void)
{
    switch (s_car_mode) {
    case CAR_MODE_RUN_LINE:
        LED1_ON();
        LED2_OFF();
        LED3_OFF();
        break;
    case CAR_MODE_READY:
        LED1_OFF();
        LED2_ON();
        LED3_OFF();
        break;
    case CAR_MODE_FAULT:
        LED1_OFF();
        LED2_OFF();
        LED3_ON();
        break;
    case CAR_MODE_IDLE:
    default:
        LED1_OFF();
        LED2_OFF();
        LED3_OFF();
        break;
    }
}

void car_control_init(void)
{
    car_control_stop_to_mode(CAR_MODE_IDLE);
    car_control_update_leds();
}

uint8_t car_control_is_running(void)
{
    return (s_car_mode == CAR_MODE_RUN_LINE) ? 1U : 0U;
}

CarMode_t car_control_get_mode(void)
{
    return s_car_mode;
}

const char *car_control_get_mode_name(void)
{
    switch (s_car_mode) {
    case CAR_MODE_READY:
        return "READY";
    case CAR_MODE_RUN_LINE:
        return "RUN";
    case CAR_MODE_FAULT:
        return "FAULT";
    case CAR_MODE_IDLE:
    default:
        return "IDLE";
    }
}

void car_control_on_key(uint8_t key)
{
    switch (key) {
    case 1U:
        if (s_car_mode == CAR_MODE_RUN_LINE) {
            car_control_stop_to_mode(CAR_MODE_IDLE);
        } else {
            encoder_clear_counts();
            s_car_mode = CAR_MODE_RUN_LINE;
        }
        break;
    case 2U:
        if (s_car_mode != CAR_MODE_RUN_LINE) {
            trctive_capture_white();
            s_car_mode = CAR_MODE_READY;
        }
        break;
    case 3U:
        if (s_car_mode == CAR_MODE_RUN_LINE) {
            car_control_stop_to_mode(CAR_MODE_IDLE);
        } else {
            trctive_capture_black();
            s_car_mode = CAR_MODE_READY;
        }
        break;
    default:
        break;
    }

    car_control_update_leds();
}

void question1(uint8_t number)
{
    (void) number;

    if (s_car_mode != CAR_MODE_RUN_LINE) {
        trctive_read();
        car_come(0.0f, 0.0f);
        return;
    }

    motor_trctive();
}
