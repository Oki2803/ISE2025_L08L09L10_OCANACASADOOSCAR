#ifndef __RTC_H
#define __RTC_H

#include "stm32f4xx_hal.h"

/* Defines related to Clock configuration */
#define RTC_ASYNCH_PREDIV  0x7F   /* LSE as RTC clock */
#define RTC_SYNCH_PREDIV   0x00FF /* LSE as RTC clock */

//Prototipos de funciones
int Init_ThRTC (void);
void ThRTC (void *argument);
void RTC_configuration(void);
void RTC_CalendarConfig(void);
void RTC_CalendarShow(uint8_t *showtime, uint8_t *showdate);
void RTC_lectura_BackUp(void);
void Error_Handler(void);
void RTC_AlarmConfig(void);
void RTC_Alarm_IRQHandler(void);
void HAL_RTC_AlarmAEventCallback(RTC_HandleTypeDef *hrtc);

#endif /* __MAIN_H */
