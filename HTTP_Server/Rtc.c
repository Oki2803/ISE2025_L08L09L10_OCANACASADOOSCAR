#include "Rtc.h"
#include "main.h"


/* RTC handler declaration */
RTC_HandleTypeDef RtcHandle;
/* Buffers used for displaying Time and Date */
char Hora[40];
char Fecha[40];
/* Estructuras para guardar horas, fecha y funcionamiento de la alarma */
RTC_DateTypeDef sdatestructureget = {0};
RTC_TimeTypeDef stimestructureget = {0};
RTC_AlarmTypeDef Alarma = {0};
/* Booleano que indica si la alarma está activa o no*/
bool alarma_activada = false;

/*##-1- Configure the RTC peripheral #######################################*/
/* Configure RTC prescaler and RTC data registers */
/* RTC configured as follows:
    - Hour Format    = Format 24
    - Asynch Prediv  = Value according to source clock
    - Synch Prediv   = Value according to source clock
    - OutPut         = Output Disable
    - OutPutPolarity = High Polarity
    - OutPutType     = Open Drain */ 
void RTC_configuration(void){

  RtcHandle.Instance = RTC; 
  RtcHandle.Init.HourFormat = RTC_HOURFORMAT_24;
  RtcHandle.Init.AsynchPrediv = RTC_ASYNCH_PREDIV;
  RtcHandle.Init.SynchPrediv = RTC_SYNCH_PREDIV;
  RtcHandle.Init.OutPut = RTC_OUTPUT_DISABLE;
  RtcHandle.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
  RtcHandle.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;
  __HAL_RTC_RESET_HANDLE_STATE(&RtcHandle);
  if (HAL_RTC_Init(&RtcHandle) != HAL_OK)
  {
    /* Initialization Error */
    Error_Handler();
  }

}


/**
  * @brief  Configure the current time and date.
  * @param  None
  * @retval None
  */
void RTC_CalendarConfig(void){
  RTC_DateTypeDef sdatestructure;
  RTC_TimeTypeDef stimestructure;

  /*##-1- Configure the Date #################################################*/
  /* Set Date: Tuesday February 18th 2014 */
  sdatestructure.Year = 0x14;
  sdatestructure.Month = RTC_MONTH_FEBRUARY;
  sdatestructure.Date = 0x18;
  sdatestructure.WeekDay = RTC_WEEKDAY_TUESDAY;
  
  if(HAL_RTC_SetDate(&RtcHandle,&sdatestructure,RTC_FORMAT_BCD) != HAL_OK)
  {
    /* Initialization Error */
    Error_Handler();
  }

  /*##-2- Configure the Time #################################################*/
  /* Set Time: 02:00:00 */
  stimestructure.Hours = 0x02;
  stimestructure.Minutes = 0x00;
  stimestructure.Seconds = 0x00;
  stimestructure.TimeFormat = RTC_HOURFORMAT12_AM;
  stimestructure.DayLightSaving = RTC_DAYLIGHTSAVING_NONE ;
  stimestructure.StoreOperation = RTC_STOREOPERATION_RESET;

  if (HAL_RTC_SetTime(&RtcHandle, &stimestructure, RTC_FORMAT_BCD) != HAL_OK)
  {
    /* Initialization Error */
    Error_Handler();
  }

  /*##-3- Writes a data in a RTC Backup data Register1 #######################*/
  HAL_RTCEx_BKUPWrite(&RtcHandle, RTC_BKP_DR1, 0x32F2);
}



/**
  * @brief  Display the current time and date.
  * @param  showtime : pointer to buffer
  * @param  showdate : pointer to buffer
  * @retval None
  */
void RTC_CalendarShow(){

	cleanBuffer();
	
  /* Get the RTC current Time */
  HAL_RTC_GetTime(&RtcHandle, &stimestructureget, RTC_FORMAT_BIN);
  /* Get the RTC current Date */
  HAL_RTC_GetDate(&RtcHandle, &sdatestructureget, RTC_FORMAT_BIN);
	
  /* Display time Format : hh:mm:ss */
  sprintf(Hora, "%2d:%2d:%2d", stimestructureget.Hours, stimestructureget.Minutes, stimestructureget.Seconds);
  write_lcd(Hora,1);
  
  /* Display date Format : mm-dd-yy */
  sprintf(Fecha, "%2d-%2d-%2d", sdatestructureget.Month, sdatestructureget.Date, 2000 + sdatestructureget.Year);
  write_lcd(Fecha,2);
 
}


/*##-2- Check if Data stored in BackUp register1: No Need to reconfigure RTC#*/
/* Read the Back Up Register 1 Data */
void RTC_lectura_BackUp(void){ 
  if (HAL_RTCEx_BKUPRead(&RtcHandle, RTC_BKP_DR1) != 0x32F2){
    /* Configure RTC Calendar */
    RTC_CalendarConfig();
  }else{
    /* Check if the Power On Reset flag is set */
    if (__HAL_RCC_GET_FLAG(RCC_FLAG_PORRST) != RESET){
      /* Turn on LED2: Power on reset occurred */
      LED_On(1);
    }
    /* Check if Pin Reset flag is set */
    if (__HAL_RCC_GET_FLAG(RCC_FLAG_PINRST) != RESET){
      /* Turn on LED1: External reset occurred */
      LED_On(0);
    }
    /* Clear source Reset Flag */
    __HAL_RCC_CLEAR_RESET_FLAGS();
  }
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @param  None
  * @retval None
  */
void Error_Handler(void){
  while (1){}
}

/* Funciones para el funcionamiento de la alarma */
void RTC_AlarmConfig(void){
  
	HAL_RTC_GetTime(&RtcHandle, &stimestructureget, RTC_FORMAT_BIN);

	/* Configurar la alarma */
	Alarma.AlarmTime.Hours = stimestructureget.Hours;
	Alarma.AlarmTime.Minutes = stimestructureget.Minutes;
	Alarma.AlarmTime.Seconds = 0;
	Alarma.AlarmMask = RTC_ALARMMASK_DATEWEEKDAY | RTC_ALARMMASK_HOURS | RTC_ALARMMASK_MINUTES; 
	Alarma.AlarmDateWeekDaySel = RTC_ALARMDATEWEEKDAYSEL_DATE;
	Alarma.AlarmDateWeekDay = 0x1;
	Alarma.Alarm = RTC_ALARM_A;

	HAL_NVIC_SetPriority(RTC_Alarm_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(RTC_Alarm_IRQn);

	if (HAL_RTC_SetAlarm_IT(&RtcHandle, &Alarma, RTC_FORMAT_BIN) != HAL_OK) {
			Error_Handler();
	}
}

void RTC_Alarm_IRQHandler(void){
	HAL_RTC_AlarmIRQHandler(&RtcHandle);
}

/* Callback de la alarma*/
void HAL_RTC_AlarmAEventCallback(RTC_HandleTypeDef *hrtc){
	alarma_activada=true;
}
