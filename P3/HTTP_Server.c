/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::Network
 * Copyright (c) 2004-2019 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    HTTP_Server.c
 * Purpose: HTTP Server example
 *----------------------------------------------------------------------------*/

#include <stdio.h>
#include "main.h"
#include "rl_net.h"                     // Keil.MDK-Pro::Network:CORE
#include "stm32f4xx_hal.h"              // Keil::Device:STM32Cube HAL:Common
#include "Leds.h"                 
#include "Board_Buttons.h"              // ::Board Support:Buttons
#include "lcd.h"
#include "Adc.h"                  
#include "Rtc.h"
#include "SNTP.h"
#include "Powermodes.h"

// Main stack size must be multiple of 8 Bytes
#define APP_MAIN_STK_SZ (1024U)
uint64_t app_main_stk[APP_MAIN_STK_SZ / 8];
const osThreadAttr_t app_main_attr = {
  .stack_mem  = &app_main_stk[0],
  .stack_size = sizeof(app_main_stk)
};



extern uint16_t AD_in          (uint32_t ch);
extern uint8_t  get_button     (void);
extern void     netDHCP_Notify (uint32_t if_num, uint8_t option, const uint8_t *val, uint32_t len);

extern bool LEDrun;
extern char lcd_text[2][20+1];

extern osThreadId_t TID_Display;
extern osThreadId_t TID_Led;

/* Booleano que indica si la alarma está activa o no*/
extern bool alarma_activada;
/* Estructuras y variables para representar la hora en el display */
extern RTC_DateTypeDef sdatestructure;
extern RTC_TimeTypeDef stimestructure;
extern char Hora[40];
extern char Fecha[40];

bool modo_sleep = false;
bool LEDrun;
char lcd_text[2][20+1] = { "LCD line 1",
                           "LCD line 2" };
ADC_HandleTypeDef adchandle; //handler definition													 
													 
/* Thread IDs */
osThreadId_t TID_Display;
osThreadId_t TID_Led;
osThreadId_t TID_Alarma;
osThreadId_t TID_Led_Verde;


/* Thread declarations */
static void BlinkLed  (void *arg);
static void Display   (void *arg);
static void	Alarma	  (void *arg);	
static void	Led_verde (void *arg);														 

/* Timer IDs */													 
osTimerId_t tim_modo_sleep; //Identificador del timer virtual que nos mete en el modo sleep	
static void Timer_Callback_sleep(void const *arg);//Prototipo de la función
													 
__NO_RETURN void app_main (void *arg);

/* Read analog inputs */
uint16_t AD_in (uint32_t ch) {
  uint32_t val = 0;

  if (ch == 0) {
		ADC_Init_Single_Conversion(&adchandle , ADC1); //ADC1 configuration
    val = ADC_getVoltage(&adchandle, 3);
  }
	else if (ch == 1) {
		ADC_Init_Single_Conversion(&adchandle , ADC1); //ADC1 configuration
    val = ADC_getVoltage(&adchandle, 10);
  }
	
  return ((uint16_t)val);
}

/* Read digital inputs */
uint8_t get_button (void) {
  return ((uint8_t)Buttons_GetState ());
}

/* IP address change notification */
void netDHCP_Notify (uint32_t if_num, uint8_t option, const uint8_t *val, uint32_t len) {

  (void)if_num;
  (void)val;
  (void)len;

  if (option == NET_DHCP_OPTION_IP_ADDRESS) {
    /* IP address change, trigger LCD update */
    osThreadFlagsSet (TID_Display, 0x01);
  }
}



/*----------------------------------------------------------------------------
  Thread 'LED_VERDE': Hace parpadear el led verde para ver si entramos en el modo bajo consumo
 *---------------------------------------------------------------------------*/
static __NO_RETURN void Led_verde (void *arg) {

	tim_modo_sleep = osTimerNew((osTimerFunc_t)&Timer_Callback_sleep, osTimerOnce, NULL, NULL);
	osTimerStart(tim_modo_sleep, 15000u);
	
	while (1) {
		
		HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_0);//Parpadeo del led verde cada 100ms
		osDelay(100);
		
		if(modo_sleep == true){//De esta forma volvemos a iniciar el timer una vez salimos del modo sleep
			osTimerStart(tim_modo_sleep, 15000u);
			modo_sleep = false;
		}
		
	}
}

static void Timer_Callback_sleep(void const *arg){ //Callback del timer
	
	Encender_rojo();//Lo encendemos antes de entrar en el modo sleep
	SleepMode_Measure();
	
}




/*----------------------------------------------------------------------------
  Thread 'Alarma': Comprobar si salta la alarma
 *---------------------------------------------------------------------------*/
static __NO_RETURN void Alarma (void *arg) {

	//RTC_AlarmConfig();//Configuramos la alarma del rtc

	while (1) {
		RTC_CalendarShow();
		osThreadFlagsSet(TID_Display,0x02);//Mandamos un flag al display para que pinte la hora
//		if(alarma_activada==true){
//			alarma_activada=false;
//				Parpadeo_5s();
//    }
		osDelay(1000);
	}
}



/*----------------------------------------------------------------------------
  Thread 'Display': LCD display handler
 *---------------------------------------------------------------------------*/
static __NO_RETURN void Display (void *arg) {
  uint8_t flag;

  (void)arg;
	
	startLCD();
	limpiar_LCD();

  while(1) {
    /* Wait for signal from DHCP */
    flag = osThreadFlagsWait (0x03U, osFlagsWaitAny, osWaitForever);//Ponemos un 3 porque queremos recibir el 1 del cgi y el 2 del calendarshow

		cleanBuffer();
		
		if(flag == 0x01){//Flag para pintar desde la página
			
			write_lcd(lcd_text[0], 1);
			LCD_update_L1();

			write_lcd(lcd_text[1], 2);
			LCD_update_L2();
			
		}else if(flag == 0x02){//Flag para pintar la hora
			
		  /* Display time Format : hh:mm:ss */
			sprintf(Hora, "%2d:%2d:%2d", stimestructure.Hours, stimestructure.Minutes, stimestructure.Seconds);
			write_lcd(Hora,1);
			
			/* Display date Format : mm-dd-yy */
			sprintf(Fecha, "%2d-%2d-%2d", sdatestructure.Date, sdatestructure.Month, 2000 + sdatestructure.Year);
			write_lcd(Fecha,2);
			
		}
  }
}


/*----------------------------------------------------------------------------
  Thread 'BlinkLed': Blink the LEDs on an eval board
 *---------------------------------------------------------------------------*/
static __NO_RETURN void BlinkLed (void *arg) {
  const uint8_t led_val[16] = { 0x48,0x88,0x84,0x44,0x42,0x22,0x21,0x11,
                                0x12,0x0A,0x0C,0x14,0x18,0x28,0x30,0x50 };
  uint32_t cnt = 0U;

  (void)arg;

  LEDrun = false;
  while(1) {
    /* Every 100 ms */
    if (LEDrun == true) {
      LED_SetOut (led_val[cnt]);
      if (++cnt >= sizeof(led_val)) {
        cnt = 0U;
      }
    }
    osDelay (100);
  }
}

/*----------------------------------------------------------------------------
  Main Thread 'main': Run Network
 *---------------------------------------------------------------------------*/
__NO_RETURN void app_main (void *arg) {
  (void)arg;

  LED_Initialize();
	ADC1_pins_F429ZI_config(); //specific PINS configuration -> Inicializamos los pines del ADC

  netInitialize ();
  RTC_configuration();//Configuramos el RTC
	Init_ThSNTP();

	TID_Led     = osThreadNew (BlinkLed, NULL, NULL);
  TID_Display = osThreadNew (Display,  NULL, NULL);
	TID_Alarma  = osThreadNew (Alarma,  NULL, NULL);
	TID_Led_Verde  = osThreadNew (Led_verde,  NULL, NULL);

  osThreadExit();
}
