#ifndef __SNTP_H
#define __SNTP_H

#include "main.h"
#include "cmsis_os2.h"                          // CMSIS RTOS header file 

int Init_ThSNTP (void);
void ThSNTP (void *argument);
static void Timer_Callback_3min(void const *arg);
static void Timer_Callback_5s(void const *arg);
void Iniciar_SNTP(void);
void time_callback (uint32_t seconds, uint32_t seconds_fraction);

#endif /* __SNTP_H */
