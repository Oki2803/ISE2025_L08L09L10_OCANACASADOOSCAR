#include "lcd.h"
#include "Arial12x12.h"
#include "stdio.h"
#include "string.h"

extern ARM_DRIVER_SPI Driver_SPI1;
ARM_DRIVER_SPI* SPIdrv = &Driver_SPI1;

static TIM_HandleTypeDef htim7;
GPIO_InitTypeDef GPIO_InitStruct;
unsigned char buffer[512];

uint16_t positionL1 = 0;
uint16_t positionL2 = 256;


void cleanBuffer(void){
  int i = 0;
		
  for(i=0;i<512;i++){
    buffer[i] = 0x00;
    }
		
  //Reseteamos ambas páginas
  positionL1 = 0;
  positionL2 = 256;
	
  //LCD_update();
}


void LCD_update(void){

  int i;
  LCD_wr_cmd(0x00); // 4 bits de la parte baja de la direcci?n a 0
  LCD_wr_cmd(0x10); // 4 bits de la parte alta de la direcci?n a 0
  LCD_wr_cmd(0xB0); // Pagina 0

  for(i=0;i<128;i++){
    LCD_wr_data(buffer[i]);
  }

  LCD_wr_cmd(0x00); // 4 bits de la parte baja de la direcci?n a 0
  LCD_wr_cmd(0x10); // 4 bits de la parte alta de la direcci?n a 0
  LCD_wr_cmd(0xB1); // P?gina 1

  for(i=128;i<256;i++){
    LCD_wr_data(buffer[i]);
  }

  LCD_wr_cmd(0x00);
  LCD_wr_cmd(0x10);
  LCD_wr_cmd(0xB2); //P?gina 2
  for(i=256;i<384;i++){
    LCD_wr_data(buffer[i]);
  }

  LCD_wr_cmd(0x00);
  LCD_wr_cmd(0x10);
  LCD_wr_cmd(0xB3); // Pagina 3

  for(i=384;i<512;i++){
    LCD_wr_data(buffer[i]);
  }
}


void startLCD(void){
  LCD_reset();
  LCD_init();
}

void LCD_init(void){
  LCD_wr_cmd(0xAE);//Display off
  LCD_wr_cmd(0xA2);//Fija el valor de la relación de la tensión de polarización del LCD a 1/9
  LCD_wr_cmd(0xA0);//El direccionamiento de la RAM de datos del display es la normal
  LCD_wr_cmd(0xC8);
  LCD_wr_cmd(0x22);
  LCD_wr_cmd(0x2F);
  LCD_wr_cmd(0x40);
  LCD_wr_cmd(0xAF);
  LCD_wr_cmd(0x81);
  LCD_wr_cmd(0x15);
  LCD_wr_cmd(0xA4);
  LCD_wr_cmd(0xA6);
}

void LCD_reset(void){
	
  GPIO_InitTypeDef GPIO_InitStruct;
	
  //INICIALIZACIÓN Y CONFIGURACIÓN DRIVER SPI PARA LCD
  SPIdrv->Initialize(NULL);
  SPIdrv->PowerControl(ARM_POWER_FULL);
  SPIdrv->Control(ARM_SPI_MODE_MASTER | ARM_SPI_CPOL1_CPHA1 | ARM_SPI_MSB_LSB | ARM_SPI_DATA_BITS(8), 2000000);
	
  //Inicializar los pines necesarios
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOF_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
	
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pin = GPIO_PIN_6;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
	
  GPIO_InitStruct.Pin = GPIO_PIN_13;
  HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);
	
  GPIO_InitStruct.Pin = GPIO_PIN_14;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);
	
  //Generar una señal de reset
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, GPIO_PIN_RESET);
  delay(1);
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, GPIO_PIN_SET);
  delay(1000);
	
}


void delay(uint32_t n_microsegundos){
	
  //Tiene reloj 42Mhz
	
  __HAL_RCC_TIM7_CLK_ENABLE();
  htim7.Instance = TIM7;
  htim7.Init.Prescaler = 41; //1 us por cuenta
  htim7.Init.Period = (n_microsegundos-1); //El timer salta cada n cuentas
  HAL_TIM_Base_Init(&htim7); // Inicializo
  HAL_TIM_Base_Start(&htim7);
	
  while(!__HAL_TIM_GET_FLAG(&htim7,TIM_FLAG_UPDATE)){};
 
  __HAL_TIM_CLEAR_FLAG(&htim7, TIM_FLAG_UPDATE);
		
  HAL_TIM_Base_Stop(&htim7);//Paramos el timer
  HAL_TIM_Base_DeInit(&htim7);
	
}

void LCD_wr_data(unsigned char data){
	
  // Seleccionar CS = 0;
  HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, GPIO_PIN_RESET);
  // Seleccionar A0 = 1;
  HAL_GPIO_WritePin(GPIOF, GPIO_PIN_13, GPIO_PIN_SET);
  // Escribir un dato (data) usando la función SPIDrv->Send(…);
  SPIdrv->Send(&data,sizeof(data));
  // Esperar a que se libere el bus SPI;
  while(SPIdrv->GetStatus().busy == 1){}
  // Seleccionar CS = 1;
  HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, GPIO_PIN_SET);
}

void LCD_wr_cmd(unsigned char cmd)
{
  // Seleccionar CS = 0;
  HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, GPIO_PIN_RESET);
  // Seleccionar A0 = 0;
  HAL_GPIO_WritePin(GPIOF, GPIO_PIN_13, GPIO_PIN_RESET);
  // Escribir un comando (cmd) usando la función SPIDrv->Send(…);
  SPIdrv->Send(&cmd,sizeof(cmd));
  // Esperar a que se libere el bus SPI;
  while(SPIdrv->GetStatus().busy == 1){}
  // Seleccionar CS = 1;
  HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, GPIO_PIN_SET);
}


void symbolToLocalBuffer_L1(uint8_t symbol){
  
  uint8_t i, value1, value2;
  uint16_t offset=0;
  
  offset=25*(symbol - ' ');

  for(i=0; i<12; i++){//Hasta el 12 porque es 12x12
  
    value1=Arial12x12[offset+i*2+1];
    value2=Arial12x12[offset+i*2+2];
			
    buffer[i+positionL1]=value1;//Defines en que columna dibujas de la página 0
    buffer[i+128+positionL1]=value2;//Defines en que columna dibujas de la página 1 -> Por eso se le suman 128 porque la página 0 va desde la 0 hasta la 127.
      
    }
		
  positionL1=positionL1+Arial12x12[offset];    
}
	
	
	
void symbolToLocalBuffer_L2(uint8_t symbol){ // 3º y 4º página

  uint8_t i, value1, value2;
  uint16_t offset=0;
  
  offset=25*(symbol - ' ');
		
  for(i=0; i<12; i++){//Hasta el 12 porque es 12x12
  
    value1=Arial12x12[offset+i*2+1]; 
    value2=Arial12x12[offset+i*2+2];
			

    buffer[i+256+positionL2]=value1;//Modificamos los valores para que esto sea página 2
    buffer[i+384+positionL2]=value2;//Página 3
      
  }
		
  positionL2=positionL2+Arial12x12[offset];
}	

	
void symbolToLocalBuffer(uint8_t line,uint8_t symbol){
	
  uint8_t i, value1, value2;
  uint16_t offset=0;
  uint16_t position = 0;//la usaremos para saber si la linea que me pasan es la 1 o la 2

  position = (line ==1) ? positionL1 : positionL2;
  /*La línea de arriba equivale a: 
	if(line == 1)
		position = positionL1;
	else
		position = positionL2;
	*/
		
  offset=25*(symbol - ' ');
		
  for(i=0; i<12; i++){
  
    value1=Arial12x12[offset+i*2+1]; 
    value2=Arial12x12[offset+i*2+2];

    buffer[i+position]=value1;
    buffer[i+128+position]=value2;
      
  }

  if(line == 1)
    positionL1=positionL1+Arial12x12[offset];
  else
		positionL2=positionL2+Arial12x12[offset];	

}
	
	

	
void LCD_update_L1(void){

  int i;
  LCD_wr_cmd(0x00); // 4 bits de la parte baja de la direcci?n a 0
  LCD_wr_cmd(0x10); // 4 bits de la parte alta de la direcci?n a 0
  LCD_wr_cmd(0xB0); // Pagina 0
 
  for(i=0;i<128;i++){
	LCD_wr_data(buffer[i]);
  }

  LCD_wr_cmd(0x00); // 4 bits de la parte baja de la direcci?n a 0
  LCD_wr_cmd(0x10); // 4 bits de la parte alta de la direcci?n a 0
  LCD_wr_cmd(0xB1); // P?gina 1

  for(i=128;i<256;i++){
	LCD_wr_data(buffer[i]);
  }
}
	
	
void LCD_update_L2(void){//Solo actualizamos la Línea 2

  int i;

  LCD_wr_cmd(0x00);
  LCD_wr_cmd(0x10);
  LCD_wr_cmd(0xB2); //P?gina 2
  for(i=256;i<384;i++){
    LCD_wr_data(buffer[i]);
  }

  LCD_wr_cmd(0x00);
  LCD_wr_cmd(0x10);
  LCD_wr_cmd(0xB3); // Pagina 3

  for(i=384;i<512;i++){
    LCD_wr_data(buffer[i]);
  }
}
		
	
void write_lcd(char cadena[], int linea){
	
  int j = 0;		
		
  for(j=0;j<strlen(cadena);j++){//strlen te devuelve el tamaño de la cadena -> Requiere la librería string.h
    symbolToLocalBuffer(linea,cadena[j]);
    }
		
  if(linea == 1)
    LCD_update_L1();
  else
    LCD_update_L2();
	
}
	
