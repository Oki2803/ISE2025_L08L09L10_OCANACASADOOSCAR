#ifndef __RTC_H
#define __RTC_H

#include "cmsis_os2.h"         // CMSIS RTOS header file
#include "stm32f4xx_hal.h"
#include "lcd.h"
#include "string.h"
#include "Leds.h"

/* Defines related to Clock configuration */
#define RTC_ASYNCH_PREDIV  0x7F   /* LSE as RTC clock */
#define RTC_SYNCH_PREDIV   0x00FF /* LSE as RTC clock */

//Prototipos de funciones
void RTC_configuration(void);
void RTC_CalendarConfig(void);
void RTC_CalendarShow(void);
void RTC_lectura_BackUp(void);
void Error_Handler(void);
void RTC_AlarmConfig(void);
void RTC_Alarm_IRQHandler(void);
void HAL_RTC_AlarmAEventCallback(RTC_HandleTypeDef *hrtc);

#endif /* __RTC_H */
