#include "oledapp.h"

static char buffer[32];

static void oled_show_line(uint8_t page, const char *text)
{
    snprintf(buffer, sizeof(buffer), "%-16s", text);
    OLED_ShowString(0, page, (uint8_t *) buffer, 8);
}

void oled_show(void)
{
    char line[32];

    snprintf(line, sizeof(line), "M:%s CAL:%c",
        car_control_get_mode_name(),
        (trctive_is_calibrated() != 0U) ? 'Y' : 'N');
    oled_show_line(0, line);

    snprintf(line, sizeof(line), "L:%4.0f R:%4.0f",
        encoder.left_MotorSpeed,
        encoder.right_MotorSpeed);
    oled_show_line(2, line);

    snprintf(line, sizeof(line), "G:%02X B:%u",
        trctive_get_digital(),
        trctive_get_black_count());
    oled_show_line(4, line);

    snprintf(line, sizeof(line), "E:%d K1R K3S",
        trctive_get_error_x10());
    oled_show_line(6, line);
}
