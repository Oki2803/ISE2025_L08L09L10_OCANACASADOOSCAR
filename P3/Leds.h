#ifndef __LEDS_H
#define __LEDS_H

#include "stm32f4xx_hal.h"

//Cabeceras de funciones
int32_t LED_Initialize (void);
int32_t LED_Uninitialize (void);
int32_t LED_On (uint32_t num);
int32_t LED_Off (uint32_t num);
int32_t LED_SetOut (uint32_t val);
void Parpadeo_5s(void);
void Parpadeo_2s(void);
void Encender_rojo(void);
void Apagar_rojo(void);

#endif /* __LEDS_H*/
