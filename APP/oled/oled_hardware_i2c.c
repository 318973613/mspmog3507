#include "oled_hardware_i2c.h"
#include "oledfont.h"
#include "clock.h"

#include "FreeRTOS.h"
#include "task.h"

#include <string.h>

#define OLED_I2C_ADDR             (0x3CU)
#define I2C_TIMEOUT_MS            (10U)
#define OLED_I2C_MAX_TX_BYTES     (8U)
#define OLED_I2C_MAX_DATA_BYTES   (OLED_I2C_MAX_TX_BYTES - 1U)

void delay_ms(uint32_t ms)
{
    if (xTaskGetSchedulerState() == taskSCHEDULER_RUNNING) {
        vTaskDelay(pdMS_TO_TICKS(ms));
    } else {
        mspm0_delay_ms(ms);
    }
}

static int mspm0_i2c_disable(void)
{
    DL_I2C_reset(I2C_OLED_INST);
    DL_GPIO_initDigitalOutput(GPIO_I2C_OLED_IOMUX_SCL);
    DL_GPIO_initDigitalInputFeatures(GPIO_I2C_OLED_IOMUX_SDA,
        DL_GPIO_INVERSION_DISABLE, DL_GPIO_RESISTOR_NONE,
        DL_GPIO_HYSTERESIS_DISABLE, DL_GPIO_WAKEUP_DISABLE);
    DL_GPIO_clearPins(GPIO_I2C_OLED_SCL_PORT, GPIO_I2C_OLED_SCL_PIN);
    DL_GPIO_enableOutput(GPIO_I2C_OLED_SCL_PORT, GPIO_I2C_OLED_SCL_PIN);
    return 0;
}

static int mspm0_i2c_enable(void)
{
    DL_I2C_reset(I2C_OLED_INST);
    DL_GPIO_initPeripheralInputFunctionFeatures(GPIO_I2C_OLED_IOMUX_SDA,
        GPIO_I2C_OLED_IOMUX_SDA_FUNC, DL_GPIO_INVERSION_DISABLE,
        DL_GPIO_RESISTOR_NONE, DL_GPIO_HYSTERESIS_DISABLE,
        DL_GPIO_WAKEUP_DISABLE);
    DL_GPIO_initPeripheralInputFunctionFeatures(GPIO_I2C_OLED_IOMUX_SCL,
        GPIO_I2C_OLED_IOMUX_SCL_FUNC, DL_GPIO_INVERSION_DISABLE,
        DL_GPIO_RESISTOR_NONE, DL_GPIO_HYSTERESIS_DISABLE,
        DL_GPIO_WAKEUP_DISABLE);
    DL_GPIO_enableHiZ(GPIO_I2C_OLED_IOMUX_SDA);
    DL_GPIO_enableHiZ(GPIO_I2C_OLED_IOMUX_SCL);
    DL_I2C_enablePower(I2C_OLED_INST);
    SYSCFG_DL_I2C_OLED_init();
    return 0;
}

void oled_i2c_sda_unlock(void)
{
    uint8_t cycle_cnt = 0U;

    mspm0_i2c_disable();
    do {
        DL_GPIO_clearPins(GPIO_I2C_OLED_SCL_PORT, GPIO_I2C_OLED_SCL_PIN);
        mspm0_delay_ms(1);
        DL_GPIO_setPins(GPIO_I2C_OLED_SCL_PORT, GPIO_I2C_OLED_SCL_PIN);
        mspm0_delay_ms(1);

        if (DL_GPIO_readPins(GPIO_I2C_OLED_SDA_PORT, GPIO_I2C_OLED_SDA_PIN)) {
            break;
        }
    } while (++cycle_cnt < 100U);
    mspm0_i2c_enable();
}

static uint32_t oled_elapsed_ms(unsigned long start, unsigned long now)
{
    return (uint32_t) (now - start);
}

static void oled_wait_slice(void)
{
    if (xTaskGetSchedulerState() == taskSCHEDULER_RUNNING) {
        vTaskDelay(pdMS_TO_TICKS(1U));
    }
}

static int oled_i2c_transfer(const uint8_t *data, uint8_t len)
{
    unsigned long start;
    unsigned long now;

    mspm0_get_clock_ms(&start);
    while (!(DL_I2C_getControllerStatus(I2C_OLED_INST) &
             DL_I2C_CONTROLLER_STATUS_IDLE)) {
        mspm0_get_clock_ms(&now);
        if (oled_elapsed_ms(start, now) >= I2C_TIMEOUT_MS) {
            oled_i2c_sda_unlock();
            return -1;
        }
        oled_wait_slice();
    }

    DL_I2C_clearInterruptStatus(I2C_OLED_INST,
        DL_I2C_INTERRUPT_CONTROLLER_TX_DONE |
        DL_I2C_INTERRUPT_CONTROLLER_NACK |
        DL_I2C_INTERRUPT_CONTROLLER_ARBITRATION_LOST);
    (void) DL_I2C_fillControllerTXFIFO(I2C_OLED_INST, data, len);
    DL_I2C_startControllerTransfer(I2C_OLED_INST, OLED_I2C_ADDR,
        DL_I2C_CONTROLLER_DIRECTION_TX, len);

    while (!DL_I2C_getRawInterruptStatus(I2C_OLED_INST,
               DL_I2C_INTERRUPT_CONTROLLER_TX_DONE)) {
        mspm0_get_clock_ms(&now);
        if ((DL_I2C_getRawInterruptStatus(I2C_OLED_INST,
                 DL_I2C_INTERRUPT_CONTROLLER_NACK |
                 DL_I2C_INTERRUPT_CONTROLLER_ARBITRATION_LOST) != 0U) ||
            (oled_elapsed_ms(start, now) >= I2C_TIMEOUT_MS)) {
            oled_i2c_sda_unlock();
            return -1;
        }
        oled_wait_slice();
    }

    return 0;
}

static void OLED_WR_DataBuffer(const uint8_t *data, uint8_t len)
{
    uint8_t packet[OLED_I2C_MAX_TX_BYTES];
    uint8_t offset = 0U;

    while (offset < len) {
        uint8_t chunk = (uint8_t) (len - offset);

        if (chunk > OLED_I2C_MAX_DATA_BYTES) {
            chunk = OLED_I2C_MAX_DATA_BYTES;
        }

        packet[0] = 0x40U;
        memcpy(&packet[1], &data[offset], chunk);
        (void) oled_i2c_transfer(packet, (uint8_t) (chunk + 1U));
        offset = (uint8_t) (offset + chunk);
    }
}

void OLED_ColorTurn(uint8_t i)
{
    if (i == 0U) {
        OLED_WR_Byte(0xA6U, OLED_CMD);
    } else if (i == 1U) {
        OLED_WR_Byte(0xA7U, OLED_CMD);
    }
}

void OLED_DisplayTurn(uint8_t i)
{
    if (i == 0U) {
        OLED_WR_Byte(0xC8U, OLED_CMD);
        OLED_WR_Byte(0xA1U, OLED_CMD);
    } else if (i == 1U) {
        OLED_WR_Byte(0xC0U, OLED_CMD);
        OLED_WR_Byte(0xA0U, OLED_CMD);
    }
}

void OLED_WR_Byte(uint8_t dat, uint8_t mode)
{
    uint8_t packet[2];

    packet[0] = mode ? 0x40U : 0x00U;
    packet[1] = dat;
    (void) oled_i2c_transfer(packet, 2U);
}

void OLED_Set_Pos(uint8_t x, uint8_t y)
{
    OLED_WR_Byte((uint8_t) (0xB0U + y), OLED_CMD);
    OLED_WR_Byte((uint8_t) (((x & 0xF0U) >> 4U) | 0x10U), OLED_CMD);
    OLED_WR_Byte((uint8_t) (x & 0x0FU), OLED_CMD);
}

void OLED_Display_On(void)
{
    OLED_WR_Byte(0x8DU, OLED_CMD);
    OLED_WR_Byte(0x14U, OLED_CMD);
    OLED_WR_Byte(0xAFU, OLED_CMD);
}

void OLED_Display_Off(void)
{
    OLED_WR_Byte(0x8DU, OLED_CMD);
    OLED_WR_Byte(0x10U, OLED_CMD);
    OLED_WR_Byte(0xAEU, OLED_CMD);
}

void OLED_Clear(void)
{
    uint8_t zeros[OLED_I2C_MAX_DATA_BYTES] = {0U};

    for (uint8_t page = 0U; page < 8U; page++) {
        OLED_WR_Byte((uint8_t) (0xB0U + page), OLED_CMD);
        OLED_WR_Byte(0x00U, OLED_CMD);
        OLED_WR_Byte(0x10U, OLED_CMD);

        for (uint8_t col = 0U; col < 128U; col += OLED_I2C_MAX_DATA_BYTES) {
            uint8_t chunk = (uint8_t) (128U - col);

            if (chunk > OLED_I2C_MAX_DATA_BYTES) {
                chunk = OLED_I2C_MAX_DATA_BYTES;
            }
            OLED_WR_DataBuffer(zeros, chunk);
        }
    }
}

void OLED_ShowChar(uint8_t x, uint8_t y, uint8_t chr, uint8_t sizey)
{
    uint8_t c;
    uint8_t sizex = (uint8_t) (sizey / 2U);
    uint16_t size1;

    if ((chr < ' ') || (chr > '~')) {
        chr = ' ';
    }
    c = (uint8_t) (chr - ' ');

    if (sizey == 8U) {
        size1 = 6U;
    } else {
        size1 = (uint16_t) ((sizey / 8U + ((sizey % 8U) ? 1U : 0U)) *
                            (sizey / 2U));
    }

    OLED_Set_Pos(x, y);
    for (uint16_t i = 0U; i < size1; i++) {
        if (((i % sizex) == 0U) && (sizey != 8U)) {
            OLED_Set_Pos(x, y++);
        }
        if (sizey == 8U) {
            OLED_WR_Byte(asc2_0806[c][i], OLED_DATA);
        } else if (sizey == 16U) {
            OLED_WR_Byte(asc2_1608[c][i], OLED_DATA);
        } else {
            return;
        }
    }
}

uint32_t oled_pow(uint8_t m, uint8_t n)
{
    uint32_t result = 1U;

    while (n--) {
        result *= m;
    }
    return result;
}

void OLED_ShowNum(uint8_t x, uint8_t y, uint32_t num, uint8_t len,
    uint8_t sizey)
{
    uint8_t m = 0U;
    uint8_t enshow = 0U;

    if (sizey == 8U) {
        m = 2U;
    }

    for (uint8_t t = 0U; t < len; t++) {
        uint8_t temp = (uint8_t) ((num / oled_pow(10U, len - t - 1U)) % 10U);

        if ((enshow == 0U) && (t < (len - 1U))) {
            if (temp == 0U) {
                OLED_ShowChar((uint8_t) (x + (sizey / 2U + m) * t),
                    y, ' ', sizey);
                continue;
            }
            enshow = 1U;
        }
        OLED_ShowChar((uint8_t) (x + (sizey / 2U + m) * t),
            y, (uint8_t) (temp + '0'), sizey);
    }
}

void OLED_ShowString(uint8_t x, uint8_t y, uint8_t *chr, uint8_t sizey)
{
    uint8_t j = 0U;

    while (chr[j] != '\0') {
        OLED_ShowChar(x, y, chr[j++], sizey);
        if (sizey == 8U) {
            x = (uint8_t) (x + 6U);
        } else {
            x = (uint8_t) (x + sizey / 2U);
        }
    }
}

void OLED_ShowChinese(uint8_t x, uint8_t y, uint8_t no, uint8_t sizey)
{
    uint16_t size1 = (uint16_t) ((sizey / 8U + ((sizey % 8U) ? 1U : 0U)) *
                                 sizey);

    for (uint16_t i = 0U; i < size1; i++) {
        if ((i % sizey) == 0U) {
            OLED_Set_Pos(x, y++);
        }
        if (sizey == 16U) {
            OLED_WR_Byte(Hzk[no][i], OLED_DATA);
        } else {
            return;
        }
    }
}

void OLED_DrawBMP(uint8_t x, uint8_t y, uint8_t sizex, uint8_t sizey,
    uint8_t BMP[])
{
    uint16_t j = 0U;
    uint8_t pages = (uint8_t) (sizey / 8U + ((sizey % 8U) ? 1U : 0U));

    for (uint8_t i = 0U; i < pages; i++) {
        OLED_Set_Pos(x, (uint8_t) (i + y));
        for (uint8_t m = 0U; m < sizex; m++) {
            OLED_WR_Byte(BMP[j++], OLED_DATA);
        }
    }
}

void OLED_Init(void)
{
    if (DL_I2C_getSDAStatus(I2C_OLED_INST) == DL_I2C_CONTROLLER_SDA_LOW) {
        oled_i2c_sda_unlock();
    }

    delay_ms(200U);

    OLED_WR_Byte(0xAEU, OLED_CMD);
    OLED_WR_Byte(0x00U, OLED_CMD);
    OLED_WR_Byte(0x10U, OLED_CMD);
    OLED_WR_Byte(0x40U, OLED_CMD);
    OLED_WR_Byte(0x81U, OLED_CMD);
    OLED_WR_Byte(0xCFU, OLED_CMD);
    OLED_WR_Byte(0xA1U, OLED_CMD);
    OLED_WR_Byte(0xC8U, OLED_CMD);
    OLED_WR_Byte(0xA6U, OLED_CMD);
    OLED_WR_Byte(0xA8U, OLED_CMD);
    OLED_WR_Byte(0x3FU, OLED_CMD);
    OLED_WR_Byte(0xD3U, OLED_CMD);
    OLED_WR_Byte(0x00U, OLED_CMD);
    OLED_WR_Byte(0xD5U, OLED_CMD);
    OLED_WR_Byte(0x80U, OLED_CMD);
    OLED_WR_Byte(0xD9U, OLED_CMD);
    OLED_WR_Byte(0xF1U, OLED_CMD);
    OLED_WR_Byte(0xDAU, OLED_CMD);
    OLED_WR_Byte(0x12U, OLED_CMD);
    OLED_WR_Byte(0xDBU, OLED_CMD);
    OLED_WR_Byte(0x40U, OLED_CMD);
    OLED_WR_Byte(0x20U, OLED_CMD);
    OLED_WR_Byte(0x02U, OLED_CMD);
    OLED_WR_Byte(0x8DU, OLED_CMD);
    OLED_WR_Byte(0x14U, OLED_CMD);
    OLED_WR_Byte(0xA4U, OLED_CMD);
    OLED_WR_Byte(0xA6U, OLED_CMD);
    OLED_Clear();
    OLED_WR_Byte(0xAFU, OLED_CMD);
}
