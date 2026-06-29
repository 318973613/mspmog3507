#ifndef INTERRUPT_PRIORITY_H_
#define INTERRUPT_PRIORITY_H_

/*
 * MSPM0G3507 uses 2 NVIC priority bits in this project.
 * Lower numeric value means higher interrupt priority.
 *
 * Priority 0 is reserved for future hard real-time ISRs only.
 * Any ISR that calls FreeRTOS APIs must stay at priority 1 or lower urgency
 * numerically greater/equal than configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY.
 */
#define IRQ_PRIORITY_HARD_REALTIME     (0U)
#define IRQ_PRIORITY_FAST_ISR          (1U)
#define IRQ_PRIORITY_CONTROL_ISR       (2U)
#define IRQ_PRIORITY_BACKGROUND_ISR    (3U)

/* Encoder GPIO ISR is short and does not call FreeRTOS. */
#define IRQ_PRIORITY_ENCODER           IRQ_PRIORITY_FAST_ISR

/* RTOS tick calls FreeRTOS from ISR, so keep it at the lowest urgency. */
#define IRQ_PRIORITY_RTOS_TICK         IRQ_PRIORITY_BACKGROUND_ISR

/* Driver/service ISRs. */
#define IRQ_PRIORITY_ADC_SAMPLE        IRQ_PRIORITY_CONTROL_ISR
#define IRQ_PRIORITY_UART_DEBUG        IRQ_PRIORITY_BACKGROUND_ISR
#define IRQ_PRIORITY_I2C_OLED          IRQ_PRIORITY_BACKGROUND_ISR
#define IRQ_PRIORITY_UNUSED_TIMER      IRQ_PRIORITY_BACKGROUND_ISR

#endif
