#ifndef  key_h
#define  key_h
#include "main.h"

/*LED BOARD*/
#define LED1_ON()     DL_GPIO_clearPins(LED_LED1_PORT, LED_LED1_PIN )
#define LED1_OFF()    DL_GPIO_setPins(LED_LED1_PORT, LED_LED1_PIN )
#define LED1_TOGGLE() DL_GPIO_togglePins(LED_LED1_PORT, LED_LED1_PIN )

/*LED RED*/
#define LED2_ON()     DL_GPIO_clearPins(LED_LED2_PORT, LED_LED2_PIN)
#define LED2_OFF()    DL_GPIO_setPins(LED_LED2_PORT, LED_LED2_PIN)
#define LED2_TOGGLE() DL_GPIO_togglePins(LED_LED2_PORT, LED_LED2_PIN)
/*LED BLUE*/
#define LED3_ON()     DL_GPIO_clearPins(LED_LED3_PORT, LED_LED3_PIN)
#define LED3_OFF()    DL_GPIO_setPins(LED_LED3_PORT, LED_LED3_PIN)
#define LED3_TOGGLE() DL_GPIO_togglePins(LED_LED3_PORT, LED_LED3_PIN)


uint8_t Key_Read(void);
void led_task(void);


#endif


