#include "main.h"
#include "Powermodes.h"
#include "Leds.h"
#include "lcd.h"

/**
  * @brief  This function configures the system to enter Sleep mode for
  *         current consumption measurement purpose.
  *         Sleep Mode
  *         ==========  
  *            - System Running at PLL (100MHz)
  *            - Flash 3 wait state
  *            - Instruction and Data caches ON
  *            - Prefetch ON   
  *            - Code running from Internal FLASH
  *            - All peripherals disabled.
  *            - Wakeup using EXTI Line (user Button)
  * @param  None
  * @retval None
  */
	void SleepMode_Measure(void){ //Consumo: Modo run = 25.7mA / Modo sleep = 14.5mA
  GPIO_InitTypeDef GPIO_InitStruct;

  /* Disable USB Clock */
  __HAL_RCC_USB_OTG_FS_CLK_DISABLE();

  /* Disable Ethernet Clock */
  __HAL_RCC_ETH_CLK_DISABLE();

  /* Configure all GPIO as analog to reduce current consumption on non used IOs */
  /* Enable GPIOs clock */
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOE_CLK_ENABLE();
  __HAL_RCC_GPIOF_CLK_ENABLE();
  __HAL_RCC_GPIOG_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOI_CLK_ENABLE();
  __HAL_RCC_GPIOJ_CLK_ENABLE();
  __HAL_RCC_GPIOK_CLK_ENABLE();

//  GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
//  GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
//  GPIO_InitStruct.Pull = GPIO_NOPULL;
//  GPIO_InitStruct.Pin = GPIO_PIN_All;
//  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct); 
//  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
//  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
//  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);
//  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);
//  HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);
//  HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);
//  HAL_GPIO_Init(GPIOH, &GPIO_InitStruct); 
//  HAL_GPIO_Init(GPIOI, &GPIO_InitStruct);
//  HAL_GPIO_Init(GPIOJ, &GPIO_InitStruct); 
//  HAL_GPIO_Init(GPIOK, &GPIO_InitStruct);

  /* Disable GPIOs clock */
  __HAL_RCC_GPIOA_CLK_DISABLE();
  __HAL_RCC_GPIOB_CLK_DISABLE();
  __HAL_RCC_GPIOC_CLK_DISABLE();
  __HAL_RCC_GPIOD_CLK_DISABLE();
  __HAL_RCC_GPIOE_CLK_DISABLE();
  __HAL_RCC_GPIOF_CLK_DISABLE();
  __HAL_RCC_GPIOG_CLK_DISABLE();
  __HAL_RCC_GPIOH_CLK_DISABLE();
  __HAL_RCC_GPIOI_CLK_DISABLE();
  __HAL_RCC_GPIOJ_CLK_DISABLE();
  __HAL_RCC_GPIOK_CLK_DISABLE();

  /* Configure user Button */
  //BSP_PB_Init(BUTTON_KEY, BUTTON_MODE_EXTI);
	Pulsador_Azul();

  /* Suspend Tick increment to prevent wakeup by Systick interrupt. 
     Otherwise the Systick interrupt will wake up the device within 1ms (HAL time base) */
  HAL_SuspendTick();

  /* Request to enter SLEEP mode */
  HAL_PWR_EnterSLEEPMode(PWR_MAINREGULATOR_ON, PWR_SLEEPENTRY_WFI);

  /* Resume Tick interrupt if disabled prior to sleep mode entry */
  HAL_ResumeTick();
	
	LED_Initialize();//Si no pongo esto no vuelve a parpadear el led verde
	startLCD();//Si no pongo esto peta el lcd
	Apagar_rojo();//Se tiene que apagar el led cada vez que salimos del modo sleep
  
  /* Exit Ethernet Phy from LowPower mode */
  //ETH_PhyExitFromPowerDownMode();
}



/* Inicialización y configuración del pulsador azul = PC13 */
void Pulsador_Azul(void){ 
  
	GPIO_InitTypeDef GPIO_InitStruct;

  /*Enable clock to GPIO-C*/
  __HAL_RCC_GPIOC_CLK_ENABLE();
  
  GPIO_InitStruct.Pin = GPIO_PIN_13;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
	
  HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);
}

//Rutina a la Interrupcion del pulsador

void EXTI15_10_IRQHandler(void){
  HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_13);
}
