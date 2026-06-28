#ifndef uart_h
#define uart_h

#include "main.h"

void uart0_send_string(const char *str);
void uart0_init(void);
uint32_t uart0_get_tx_dropped(void);

#endif


