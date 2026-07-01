#ifndef app_trc_h
#define app_trc_h

#include <stdint.h>
#include "main.h"

extern unsigned short Anolog[8];
extern unsigned short white[8];
extern unsigned short black[8];


void trctive_init(void);
void trctive_read(void);
void motor_trctive(void);
void trctive_capture_white(void);
void trctive_capture_black(void);
uint8_t trctive_get_digital(void);
uint8_t trctive_get_black_count(void);
int16_t trctive_get_error_x10(void);
uint8_t trctive_is_calibrated(void);

#endif


