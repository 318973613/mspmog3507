#ifndef  active_h
#define  active_h
#include "stdint.h"
#include "main.h"

typedef enum {
    CAR_MODE_IDLE = 0,
    CAR_MODE_READY,
    CAR_MODE_RUN_LINE,
    CAR_MODE_FAULT,
} CarMode_t;

void car_control_init(void);
void car_control_on_key(uint8_t key);
uint8_t car_control_is_running(void);
CarMode_t car_control_get_mode(void);
const char *car_control_get_mode_name(void);

void question1(uint8_t number);



#endif

