#include "stm32f4xx_hal.h"
#include "Leds.h"

/*Con la variable num que tenemos en algunas funciones representamos que led estamos usando
LD1 = 0; LD2 = 1; LD3 = 2; RGBAZUL = 3; RGBVERDE = 4; RGBROJO = 5;
*/


/* Inicialización de pines */
int32_t LED_Initialize(void){ 
   GPIO_InitTypeDef GPIO_InitStruct;

  /*Enable clock to GPIO-B*/
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Set GPIOB pin */
  GPIO_InitStruct.Pin = GPIO_PIN_0 | GPIO_PIN_7 | GPIO_PIN_14 ;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_LOW;  

  /*Init GPIOB Pins*/
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
	  
	/*Enable clock to GPIO-D*/
	__HAL_RCC_GPIOD_CLK_ENABLE();
	
	/*Set GPIOD pin */
	GPIO_InitStruct.Pin = GPIO_PIN_11 | GPIO_PIN_12 | GPIO_PIN_13;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  GPIO_InitStruct.Speed = GPIO_SPEED_LOW;
  
  /*Init GPIOD Pins*/	
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);
	
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_11 | GPIO_PIN_12 | GPIO_PIN_13, GPIO_PIN_SET);//Para que el RGB inicie apagado

	return 0;
}


/**
  \fn          int32_t LED_Uninitialize (void)
  \brief       De-initialize LEDs
  \returns
   - \b  0: function succeeded
   - \b -1: function failed
*/
int32_t LED_Uninitialize (void) {

	  HAL_GPIO_DeInit(GPIOB, GPIO_PIN_0 & GPIO_PIN_7 & GPIO_PIN_14);
    HAL_GPIO_DeInit(GPIOD, GPIO_PIN_11 & GPIO_PIN_12 & GPIO_PIN_13);

  return 0;
}

/**
  \fn          int32_t LED_On (uint32_t num)
  \brief       Turn on requested LED
  \param[in]   num  LED number
  \returns
   - \b  0: function succeeded
   - \b -1: function failed
*/
int32_t LED_On (uint32_t num) {

  if (num >= 6) {
    return -1;
  }
	
	switch(num){
		case 0: 
			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_SET);//LD1
		break;
		case 1: 
			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, GPIO_PIN_SET);//LD2			
		break;
		case 2: 
			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_SET);//LD3	
		break;
		case 3: 
			HAL_GPIO_WritePin(GPIOD, GPIO_PIN_11, GPIO_PIN_RESET);//RGBAZUL		
		break;
		case 4: 
			HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12, GPIO_PIN_RESET);//RGBVERDE			
		break;
		case 5: 
			HAL_GPIO_WritePin(GPIOD, GPIO_PIN_13, GPIO_PIN_RESET);//RGBROJO			
		break;
	}
	
  return 0;
}

/**
  \fn          int32_t LED_Off (uint32_t num)
  \brief       Turn off requested LED
  \param[in]   num  LED number
  \returns
   - \b  0: function succeeded
   - \b -1: function failed
*/
int32_t LED_Off (uint32_t num) {

  if (num >= 6) {
    return -1;
  }

	switch(num){
		case 0: 
			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);//LD1
		break;
		case 1: 
			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, GPIO_PIN_RESET);//LD2			
		break;
		case 2: 
			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_RESET);//LD3	
		break;
		case 3: 
			HAL_GPIO_WritePin(GPIOD, GPIO_PIN_11, GPIO_PIN_SET);//RGBROJO			
		break;
		case 4: 
			HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12, GPIO_PIN_SET);//RGBVERDE			
		break;
		case 5: 
			HAL_GPIO_WritePin(GPIOD, GPIO_PIN_13, GPIO_PIN_SET);//RGBAZUL			
		break;
	}
  return 0;
}

/**
  \fn          int32_t LED_SetOut (uint32_t val)
  \brief       Write value to LEDs
  \param[in]   val  value to be displayed on LEDs
  \returns
   - \b  0: function succeeded
   - \b -1: function failed
*/
int32_t LED_SetOut (uint32_t val) {
  uint8_t num;

  for (num = 0U; num < 6; num++) {//Ponemos un 6 porque tenemos 6 leds. Del 0 al 5.
    if ((val & (1U << num)) != 0U) {
      LED_On (num);
    } else {
      LED_Off(num);
    }
  }

  return 0;
}

//Función echa expresamente para la funcionalidad de la alarma del apartado 1 de la P2
void Parpadeo_5s(void){//Led Verde de la stm32
	
	for(int i = 0; i < 5; i++){
		LED_On(0);
		HAL_Delay(500);
		LED_Off(0);
		HAL_Delay(500);		
	}

}

//Función echa expresamente para la funcionalidad del SNTP del apartado 4 de la P2
void Parpadeo_2s(void){//Led rojo de la stm32
	
	for(int i = 0; i < 2; i++){
		LED_On(2);
		HAL_Delay(500);
		LED_Off(2);
		HAL_Delay(500);		
	}

}
