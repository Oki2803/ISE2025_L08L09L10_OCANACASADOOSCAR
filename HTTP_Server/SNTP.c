#include <time.h>
#include "rl_net_lib.h"
#include "Rtc.h"
#include "SNTP.h"
#include "Leds.h"

/* Estructuras y variables para representar la hora en el display */
extern RTC_DateTypeDef sdatestructure;
extern RTC_TimeTypeDef stimestructure;
/*** SNTP ***/
struct tm ts;
/* RTC handler declaration */
extern RTC_HandleTypeDef RtcHandle;

/*----------------------------------------------------------------------------
 *      								Thread 1 'Thread_SNTP'
 *---------------------------------------------------------------------------*/
 
osThreadId_t tid_ThSNTP;                        // thread id
osTimerId_t tim_3min;
osTimerId_t tim_5s;
bool parpadeo_ledrojo = false;

void ThSNTP (void *argument);                   // thread function
 
int Init_ThSNTP (void) {
 
  tid_ThSNTP = osThreadNew(ThSNTP, NULL, NULL);
  if (tid_ThSNTP == NULL) {
    return(-1);
  }
	
	tim_3min = osTimerNew((osTimerFunc_t)&Timer_Callback_3min, osTimerPeriodic, NULL, NULL); //Creación Timer virtual periodico
  tim_5s = osTimerNew((osTimerFunc_t)&Timer_Callback_5s, osTimerOnce, NULL, NULL); //Creación Timer virtual periodico
	
	return(0);
}
 
void ThSNTP (void *argument) {
	
	osTimerStart(tim_3min, 180000u);
	osTimerStart(tim_5s, 5000u);
  
	while (1) {
		
		if (parpadeo_ledrojo == true){
			Parpadeo_2s();
			parpadeo_ledrojo = false;
		}

  }
}


static void Timer_Callback_3min(void const *arg){ //Callback del timer de sincronización
	Iniciar_SNTP();
	parpadeo_ledrojo = true;

}

static void Timer_Callback_5s(void const *arg){ //Callback del timer de sincronización inicial
	Iniciar_SNTP();
	parpadeo_ledrojo = true;

}


/***SNTP***/
void Iniciar_SNTP (void) {
 netSNTPc_GetTime (NULL, time_callback);
}

void time_callback (uint32_t seconds, uint32_t seconds_fraction) {
  if (seconds != 0) {
   ts = *localtime(&seconds);
   sdatestructure.Year = ts.tm_year - 100;
   sdatestructure.Month = ts.tm_mon + 1;
   sdatestructure.Date = ts.tm_mday;
   sdatestructure.WeekDay = ts.tm_wday;
  
   HAL_RTC_SetDate(&RtcHandle,&sdatestructure,RTC_FORMAT_BIN);

   stimestructure.Hours = ts.tm_hour + 1 ;
   stimestructure.Minutes = ts.tm_min;
   stimestructure.Seconds = ts.tm_sec;
   stimestructure.TimeFormat = RTC_HOURFORMAT_24;
   stimestructure.DayLightSaving = ts.tm_isdst ;
   stimestructure.StoreOperation = RTC_STOREOPERATION_RESET;
   
	 HAL_RTC_SetTime(&RtcHandle, &stimestructure, RTC_FORMAT_BIN);
	
	   /*##-3- Writes a data in a RTC Backup data Register1 #######################*/
		HAL_RTCEx_BKUPWrite(&RtcHandle, RTC_BKP_DR1, 0x32F2);
  }
}

