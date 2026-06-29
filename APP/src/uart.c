#include "uart.h"
#include "interrupt_priority.h"

#include <stdio.h>

#define UART_TX_BUFFER_SIZE 512U

static volatile uint16_t s_tx_head = 0U;
static volatile uint16_t s_tx_tail = 0U;
static volatile uint32_t s_tx_dropped = 0U;
static uint8_t s_tx_buffer[UART_TX_BUFFER_SIZE];

static uint16_t uart_tx_next(uint16_t index)
{
    index++;
    if (index >= UART_TX_BUFFER_SIZE) {
        index = 0U;
    }
    return index;
}

static void uart_tx_service(void)
{
    while ((s_tx_tail != s_tx_head) &&
           !DL_UART_Main_isTXFIFOFull(UART_0_INST)) {
        DL_UART_Main_transmitData(UART_0_INST, s_tx_buffer[s_tx_tail]);
        s_tx_tail = uart_tx_next(s_tx_tail);
    }

    if (s_tx_tail == s_tx_head) {
        DL_UART_Main_disableInterrupt(UART_0_INST, DL_UART_MAIN_INTERRUPT_TX);
    } else {
        DL_UART_Main_enableInterrupt(UART_0_INST, DL_UART_MAIN_INTERRUPT_TX);
    }
}

static void uart_tx_kick(void)
{
    uint32_t primask = __get_PRIMASK();

    __disable_irq();
    uart_tx_service();
    if (primask == 0U) {
        __enable_irq();
    }
}

static int uart_tx_queue_byte(uint8_t byte)
{
    uint16_t next;
    uint32_t primask = __get_PRIMASK();
    int queued = 0;

    __disable_irq();
    next = uart_tx_next(s_tx_head);
    if (next != s_tx_tail) {
        s_tx_buffer[s_tx_head] = byte;
        s_tx_head = next;
        queued = 1;
    } else {
        s_tx_dropped++;
    }
    if (primask == 0U) {
        __enable_irq();
    }

    if (queued != 0) {
        uart_tx_kick();
    }

    return queued;
}

void uart0_init(void)
{
    uint32_t primask = __get_PRIMASK();

    __disable_irq();
    s_tx_head = 0U;
    s_tx_tail = 0U;
    s_tx_dropped = 0U;
    DL_UART_Main_disableInterrupt(UART_0_INST, DL_UART_MAIN_INTERRUPT_TX);
    DL_UART_Main_clearInterruptStatus(UART_0_INST, DL_UART_MAIN_INTERRUPT_TX);
    NVIC_ClearPendingIRQ(UART_0_INST_INT_IRQN);
    NVIC_SetPriority(UART_0_INST_INT_IRQN, IRQ_PRIORITY_UART_DEBUG);
    NVIC_EnableIRQ(UART_0_INST_INT_IRQN);
    if (primask == 0U) {
        __enable_irq();
    }
}

void uart0_send_string(const char *str)
{
    if (str == NULL) {
        return;
    }

    while (*str != '\0') {
        (void) uart_tx_queue_byte((uint8_t) *str++);
    }
}

uint32_t uart0_get_tx_dropped(void)
{
    return s_tx_dropped;
}

int fputc(int c, FILE *stream)
{
    (void) stream;
    (void) uart_tx_queue_byte((uint8_t) c);
    return c;
}

int fputs(const char *s, FILE *stream)
{
    int count = 0;

    (void) stream;
    if (s == NULL) {
        return 0;
    }

    while (*s != '\0') {
        (void) uart_tx_queue_byte((uint8_t) *s++);
        count++;
    }

    return count;
}

int puts(const char *s)
{
    int count = fputs(s, stdout);

    count += fputs("\n", stdout);
    return count;
}

void UART_0_INST_IRQHandler(void)
{
    switch (DL_UART_Main_getPendingInterrupt(UART_0_INST)) {
    case DL_UART_MAIN_IIDX_TX:
        uart_tx_service();
        break;
    default:
        break;
    }
}
