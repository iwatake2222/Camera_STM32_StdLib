/*
 * lcdIli9341Gpio.c
 *
 *  Created on: 2017/08/10
 *      Author: take-iwiw
 */
#include "stm32f4xx.h"
#include <stdio.h>
#include "../type.h"
#include "../system.h"
#include "lcdIli9341Gpio.h"
#include "lcdIli9341GpioConfig.h"


/*** Internal Const Values, Macros ***/
/*** Internal Static Variables ***/
/*** Internal Function Declarations ***/
static RET lcdIli9341Gpio_initLowLevelIO();
static RET lcdIli9341Gpio_initSequence();
static void lcdIli9341Gpio_sendDataBit(uint8_t data);
static void lcdIli9341Gpio_setDataPortOut();
static void lcdIli9341Gpio_setDataPortIn();
static void lcdIli9341Gpio_sendDataBit(uint8_t data);
static void lcdIli9341Gpio_writeData(uint8_t data);
static void lcdIli9341Gpio_writeCmd(uint8_t cmd);
static void lcdIli9341Gpio_readData();

/*** External Function Defines ***/
void lcdIli9341Gpio_blockWrite(uint16_t Xstart, uint16_t Ystart, uint16_t Xend, uint16_t Yend)
{
  lcdIli9341Gpio_writeCmd(0x2a);
  lcdIli9341Gpio_writeData(Xstart>>8);
  lcdIli9341Gpio_writeData(Xstart&0xff);
  lcdIli9341Gpio_writeData(Xend>>8);
  lcdIli9341Gpio_writeData(Xend&0xff);

  lcdIli9341Gpio_writeCmd(0x2b);
  lcdIli9341Gpio_writeData(Ystart>>8);
  lcdIli9341Gpio_writeData(Ystart&0xff);
  lcdIli9341Gpio_writeData(Yend>>8);
  lcdIli9341Gpio_writeData(Yend&0xff);

  lcdIli9341Gpio_writeCmd(0x2c);
}

void lcdIli9341Gpio_dispColor(uint16_t Xstart, uint16_t Ystart, uint16_t Xend, uint16_t Yend, uint16_t color)
{
  uint8_t data;
  data = color >> 8;

  lcdIli9341Gpio_blockWrite(Xstart, Ystart, Xend, Yend);
  GPIO_ResetBits(GPIO_CS_PORT, GPIO_CS_PIN);
  GPIO_SetBits(GPIO_RS_PORT, GPIO_RS_PIN);
  GPIO_SetBits(GPIO_RD_PORT, GPIO_RD_PIN);

  for( uint16_t j = Ystart; j < Yend; j++ ){
    for( uint16_t i = Xstart; i < Xend; i++ ){
      lcdIli9341Gpio_sendDataBit(color >> 8);
      GPIO_ResetBits(GPIO_WR_PORT, GPIO_WR_PIN);
      GPIO_SetBits(GPIO_WR_PORT, GPIO_WR_PIN);
      lcdIli9341Gpio_sendDataBit(color);
      GPIO_ResetBits(GPIO_WR_PORT, GPIO_WR_PIN);
      GPIO_SetBits(GPIO_WR_PORT, GPIO_WR_PIN);
    }
  }

  GPIO_SetBits(GPIO_CS_PORT, GPIO_CS_PIN);
}

RET lcdIli9341Gpio_init()
{
  lcdIli9341Gpio_initLowLevelIO();
  lcdIli9341Gpio_initSequence();

  lcdIli9341Gpio_dispColor(0, 0, LCD_ILI9342_GPIO_WIDTH - 1, LCD_ILI9342_GPIO_HEIGHT - 1, LCD_ILI9342_GPIO_COLOR_RED);
  lcdIli9341Gpio_dispColor(0, 0, LCD_ILI9342_GPIO_WIDTH - 1, LCD_ILI9342_GPIO_HEIGHT - 1, LCD_ILI9342_GPIO_COLOR_GREEN);
  lcdIli9341Gpio_dispColor(0, 0, LCD_ILI9342_GPIO_WIDTH - 1, LCD_ILI9342_GPIO_HEIGHT - 1, LCD_ILI9342_GPIO_COLOR_BLUE);
  lcdIli9341Gpio_dispColor(0, 0, LCD_ILI9342_GPIO_WIDTH - 1, LCD_ILI9342_GPIO_HEIGHT - 1, 0xFFFF);
  lcdIli9341Gpio_dispColor(0, 0, 100, 100, LCD_ILI9342_GPIO_COLOR_RED);
  lcdIli9341Gpio_dispColor(100, 100, 150, 150, LCD_ILI9342_GPIO_COLOR_GREEN);
  lcdIli9341Gpio_dispColor(200, 100, 320-1, 240-1, LCD_ILI9342_GPIO_COLOR_BLUE);

  lcdIli9341Gpio_writeCmd(0xd3);
  lcdIli9341Gpio_readData();
  lcdIli9341Gpio_readData();
  lcdIli9341Gpio_readData();
  lcdIli9341Gpio_readData();

  return OK;
}


/*** Internal Function Defines ***/
static void lcdIli9341Gpio_setDataPortOut()
{
  GPIO_InitTypeDef  GPIO_InitStructure;
  GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;

  GPIO_InitStructure.GPIO_Pin   = GPIO_D0_PIN;
  GPIO_Init(GPIO_D0_PORT, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin   = GPIO_D4_PIN;
  GPIO_Init(GPIO_D4_PORT, &GPIO_InitStructure);
}

static void lcdIli9341Gpio_setDataPortIn()
{
  GPIO_InitTypeDef  GPIO_InitStructure;
  GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IN;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;

  GPIO_InitStructure.GPIO_Pin   = GPIO_D0_PIN;
  GPIO_Init(GPIO_D0_PORT, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin   = GPIO_D4_PIN;
  GPIO_Init(GPIO_D4_PORT, &GPIO_InitStructure);
}


static RET lcdIli9341Gpio_initLowLevelIO()
{
  RCC_AHB1PeriphClockCmd(GPIO_RESET_CLK | GPIO_RS_CLK | GPIO_CS_CLK | GPIO_D0_CLK | GPIO_D4_CLK | GPIO_WR_CLK | GPIO_RD_CLK, ENABLE);

  /*** GPIO Configuration ***/
  GPIO_InitTypeDef  GPIO_InitStructure;
  GPIO_InitStructure.GPIO_Pin   = GPIO_RESET_PIN;
  GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIO_RESET_PORT, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin   = GPIO_RS_PIN;
  GPIO_Init(GPIO_RS_PORT, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin   = GPIO_CS_PIN;
  GPIO_Init(GPIO_CS_PORT, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin   = GPIO_D0_PIN;
  GPIO_Init(GPIO_D0_PORT, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin   = GPIO_D4_PIN;
  GPIO_Init(GPIO_D4_PORT, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin   = GPIO_WR_PIN;
  GPIO_Init(GPIO_WR_PORT, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin   = GPIO_RD_PIN;
  GPIO_Init(GPIO_RD_PORT, &GPIO_InitStructure);

  return OK;
}

static RET lcdIli9341Gpio_initSequence()
{
  GPIO_SetBits(GPIO_RESET_PORT, GPIO_RESET_PIN);  delay(10);
  GPIO_ResetBits(GPIO_RESET_PORT, GPIO_RESET_PIN);  delay(10);
  GPIO_SetBits(GPIO_RESET_PORT, GPIO_RESET_PIN);  delay(10);

  lcdIli9341Gpio_writeCmd(0x11); //Exit Sleep
  delay(10);

  lcdIli9341Gpio_writeCmd(0x26); //Gamma curve selected
  lcdIli9341Gpio_writeData(0x01);

  lcdIli9341Gpio_writeCmd(0xB6);
  lcdIli9341Gpio_writeData(0x0A);
  lcdIli9341Gpio_writeData(0xC2);

  lcdIli9341Gpio_writeCmd(0x36);   // Memory Access Control
  lcdIli9341Gpio_writeData(0x68);

  lcdIli9341Gpio_writeCmd(0x3A); //Set_pixel_format
  lcdIli9341Gpio_writeData(0x55); //64k color

  lcdIli9341Gpio_writeCmd(0xE0); //Set Gamma
  lcdIli9341Gpio_writeData(0x10);
  lcdIli9341Gpio_writeData(0x10);
  lcdIli9341Gpio_writeData(0x10);
  lcdIli9341Gpio_writeData(0x08);
  lcdIli9341Gpio_writeData(0x0E);
  lcdIli9341Gpio_writeData(0x06);
  lcdIli9341Gpio_writeData(0x42);
  lcdIli9341Gpio_writeData(0x28);
  lcdIli9341Gpio_writeData(0x36);
  lcdIli9341Gpio_writeData(0x03);
  lcdIli9341Gpio_writeData(0x0E);
  lcdIli9341Gpio_writeData(0x04);
  lcdIli9341Gpio_writeData(0x13);
  lcdIli9341Gpio_writeData(0x0E);
  lcdIli9341Gpio_writeData(0x0C);

  lcdIli9341Gpio_writeCmd(0XE1); //Set Gamma
  lcdIli9341Gpio_writeData(0x0C);
  lcdIli9341Gpio_writeData(0x23);
  lcdIli9341Gpio_writeData(0x26);
  lcdIli9341Gpio_writeData(0x04);
  lcdIli9341Gpio_writeData(0x0C);
  lcdIli9341Gpio_writeData(0x04);
  lcdIli9341Gpio_writeData(0x39);
  lcdIli9341Gpio_writeData(0x24);
  lcdIli9341Gpio_writeData(0x4B);
  lcdIli9341Gpio_writeData(0x03);
  lcdIli9341Gpio_writeData(0x0B);
  lcdIli9341Gpio_writeData(0x0B);
  lcdIli9341Gpio_writeData(0x33);
  lcdIli9341Gpio_writeData(0x37);
  lcdIli9341Gpio_writeData(0x0F);

  lcdIli9341Gpio_writeCmd(0x2a);//
  lcdIli9341Gpio_writeData(0x00);
  lcdIli9341Gpio_writeData(0x00);
  lcdIli9341Gpio_writeData(0x00);
  lcdIli9341Gpio_writeData(0xef);

  lcdIli9341Gpio_writeCmd(0x2b); //
  lcdIli9341Gpio_writeData(0x00);
  lcdIli9341Gpio_writeData(0x00);
  lcdIli9341Gpio_writeData(0x01);
  lcdIli9341Gpio_writeData(0x3f);

  lcdIli9341Gpio_writeCmd(0x29);
  delay(10);
  lcdIli9341Gpio_writeCmd(0x2C);

  return OK;
}

static void lcdIli9341Gpio_sendDataBit(uint8_t data)
{
  if(data & 0x01) {GPIO_SetBits(GPIOD, GPIO_Pin_14);
  } else {GPIO_ResetBits(GPIOD, GPIO_Pin_14);}
  if(data & 0x02) {GPIO_SetBits(GPIOD, GPIO_Pin_15);
  } else {GPIO_ResetBits(GPIOD, GPIO_Pin_15);}
  if(data & 0x04) {GPIO_SetBits(GPIOD, GPIO_Pin_0);
  } else {GPIO_ResetBits(GPIOD, GPIO_Pin_0);}
  if(data & 0x08) {GPIO_SetBits(GPIOD, GPIO_Pin_1);
  } else {GPIO_ResetBits(GPIOD, GPIO_Pin_1);}
  if(data & 0x10) {GPIO_SetBits(GPIOE, GPIO_Pin_7);
  } else {GPIO_ResetBits(GPIOE, GPIO_Pin_7);}
  if(data & 0x20) {GPIO_SetBits(GPIOE, GPIO_Pin_8);
  } else {GPIO_ResetBits(GPIOE, GPIO_Pin_8);}
  if(data & 0x40) {GPIO_SetBits(GPIOE, GPIO_Pin_9);
  } else {GPIO_ResetBits(GPIOE, GPIO_Pin_9);}
  if(data & 0x80) {GPIO_SetBits(GPIOE, GPIO_Pin_10);
  } else {GPIO_ResetBits(GPIOE, GPIO_Pin_10);}
}

static void lcdIli9341Gpio_writeCmd(uint8_t cmd)
{
  GPIO_ResetBits(GPIO_CS_PORT, GPIO_CS_PIN);
  GPIO_ResetBits(GPIO_RS_PORT, GPIO_RS_PIN);
  lcdIli9341Gpio_setDataPortOut();
  lcdIli9341Gpio_sendDataBit(cmd);
  GPIO_SetBits(GPIO_RD_PORT, GPIO_RD_PIN);
  GPIO_ResetBits(GPIO_WR_PORT, GPIO_WR_PIN);
  GPIO_SetBits(GPIO_WR_PORT, GPIO_WR_PIN);
  GPIO_SetBits(GPIO_CS_PORT, GPIO_CS_PIN);
}

static void lcdIli9341Gpio_writeData(uint8_t data)
{
  GPIO_ResetBits(GPIO_CS_PORT, GPIO_CS_PIN);
  GPIO_SetBits(GPIO_RS_PORT, GPIO_RS_PIN);
  lcdIli9341Gpio_setDataPortOut();
  lcdIli9341Gpio_sendDataBit(data);
  GPIO_SetBits(GPIO_RD_PORT, GPIO_RD_PIN);
  GPIO_ResetBits(GPIO_WR_PORT, GPIO_WR_PIN);
  GPIO_SetBits(GPIO_WR_PORT, GPIO_WR_PIN);
  GPIO_SetBits(GPIO_CS_PORT, GPIO_CS_PIN);
}

static void lcdIli9341Gpio_readData()
{
  GPIO_ResetBits(GPIO_CS_PORT, GPIO_CS_PIN);
  GPIO_SetBits(GPIO_RS_PORT, GPIO_RS_PIN);
  lcdIli9341Gpio_setDataPortIn();
  GPIO_SetBits(GPIO_WR_PORT, GPIO_WR_PIN);
  GPIO_ResetBits(GPIO_RD_PORT, GPIO_RD_PIN);
  GPIO_SetBits(GPIO_RD_PORT, GPIO_RD_PIN);
  uint8_t data = GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_10) << 7 | GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_9) << 6 | GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_8) << 5 | GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_7) << 4
       | GPIO_ReadInputDataBit(GPIOD, GPIO_Pin_1) << 3 | GPIO_ReadInputDataBit(GPIOD, GPIO_Pin_0) << 2 | GPIO_ReadInputDataBit(GPIOD, GPIO_Pin_15) << 1 | GPIO_ReadInputDataBit(GPIOD, GPIO_Pin_14);
  GPIO_SetBits(GPIO_CS_PORT, GPIO_CS_PIN);
  printf("%02X\n", data);
}

