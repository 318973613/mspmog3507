#ifndef _CLOCK_H_
#define _CLOCK_H_
#include "main.h"
#include <stdint.h>
extern volatile uint32_t uwTick;;

void mspm0_delay_ms(unsigned long xms);
uint32_t Sys_GetTick(void);
uint32_t mspm0_get_clock_ms(unsigned long *times);
void SysTick_Init(void);
void SysTick_Increasment(void);




#endif  /* #ifndef _CLOCK_H_ */
