#include "key.h"

#define KEY_DEBOUNCE_SAMPLES 2U

static uint8_t key_read_raw(void)
{
    if (!DL_GPIO_readPins(KEY_PORT, KEY_K1_PIN)) {
        return 1U;
    }
    if (!DL_GPIO_readPins(KEY_PORT, KEY_K2_PIN)) {
        return 2U;
    }
    if (!DL_GPIO_readPins(KEY_PORT, KEY_K3_PIN)) {
        return 3U;
    }
    return 0U;
}

uint8_t Key_Read(void)
{
    static uint8_t last_sample = 0U;
    static uint8_t stable_state = 0U;
    static uint8_t stable_count = 0U;
    uint8_t sample = key_read_raw();

    if (sample == last_sample) {
        if (stable_count < KEY_DEBOUNCE_SAMPLES) {
            stable_count++;
        }
    } else {
        last_sample = sample;
        stable_count = 0U;
    }

    if ((stable_count >= KEY_DEBOUNCE_SAMPLES) &&
        (sample != stable_state)) {
        stable_state = sample;
        if (stable_state != 0U) {
            return stable_state;
        }
    }

    return 0U;
}

void led_task(void)
{
    switch (Key_Read()) {
    case 1U:
        LED1_TOGGLE();
        break;
    case 2U:
        LED2_TOGGLE();
        break;
    case 3U:
        LED3_TOGGLE();
        break;
    default:
        break;
    }
}
