#ifndef __LCD_H
#define __LCD_H

#include "main.h"
#include "Driver_SPI.h"
#include "RTE_Device.h"
#include "stdio.h"

void LCD_reset (void);
void delay(uint32_t n_microsegundos);
void LCD_init(void);
void startLCD(void);
void LCD_wr_cmd(unsigned char cmd);
void LCD_wr_data(unsigned char data);
void LCD_update(void);
void LCD_update_L1(void);
void LCD_update_L2(void);
void symbolToLocalBuffer_L1(uint8_t symbol);
void symbolToLocalBuffer_L2(uint8_t symbol);
void symbolToLocalBuffer(uint8_t line,uint8_t symbol);
void write_lcd(char cadena[], int linea);
void cleanBuffer(void);

extern unsigned char buffer[512];


#endif /* __LCD_H */
