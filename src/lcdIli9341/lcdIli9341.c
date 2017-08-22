/*
 * lcdIli9341.c
 *
 *  Created on: 2017/08/10
 *      Author: take-iwiw
 */
#include "stm32f4xx.h"
#include <stdio.h>
#include "../type.h"
#include "../system.h"
#include "./lcdIli9341.h"
#include "./lcdIli9341Config.h"

/*** Internal Const Values, Macros ***/
#define FSMC_ADDRESS  (0x60000000 + ((FSMC_NEx-1) << 26))
#define LCD_CMD_ADDR  (FSMC_ADDRESS)

#ifdef BIT_WIDTH_16
#define LCD_DATA_ADDR (FSMC_ADDRESS | 1 << (FSMC_Ax + 1))
#define LCD_CMD       (*((volatile uint16_t*) LCD_CMD_ADDR))
#define LCD_DATA      (*((volatile uint16_t*) LCD_DATA_ADDR))
#else
#define LCD_DATA_ADDR (FSMC_ADDRESS | 1 << (FSMC_Ax + 0))
#define LCD_CMD       (*((volatile uint8_t*) LCD_CMD_ADDR))
#define LCD_DATA      (*((volatile uint8_t*) LCD_DATA_ADDR))
#endif

/*** Internal Static Variables ***/
static uint8_t s_isWorking = 0;

/*** Internal Function Declarations ***/
static RET lcdIli9341_initLowLevelIO();
static RET lcdIli9341_initLowLevelFSMC();
static RET lcdIli9341_initSequence();
static void lcdIli9341_setDMA(uint16_t* pBuffer, uint32_t num);
#ifdef BIT_WIDTH_16
static void lcdIli9341_writeData(uint16_t data);
static void lcdIli9341_writeCmd(uint16_t cmd);
#else
static void lcdIli9341_writeData(uint8_t data);
static void lcdIli9341_writeCmd(uint8_t cmd);
#endif
static void lcdIli9341_readData();

/*** External Function Defines ***/
RET lcdIli9341_refreshDisplay(uint8_t isNewFrame, uint16_t* pBuffer, uint32_t num)
{
  if(s_isWorking) return ERR_STATUS;
  s_isWorking = 1;
  if(isNewFrame){
    lcdIli9341_setArea(0, 0, LCD_ILI9342_WIDTH - 1, LCD_ILI9342_HEIGHT - 1);
  }
  lcdIli9341_setDMA(pBuffer, num);
  return OK;
}


void lcdIli9341_setArea(uint16_t xStart, uint16_t yStart, uint16_t xEnd, uint16_t yEnd)
{
  lcdIli9341_writeCmd(0x2a);
  lcdIli9341_writeData(xStart >> 8);
  lcdIli9341_writeData(xStart & 0xff);
  lcdIli9341_writeData(xEnd >> 8);
  lcdIli9341_writeData(xEnd & 0xff);

  lcdIli9341_writeCmd(0x2b);
  lcdIli9341_writeData(yStart >> 8);
  lcdIli9341_writeData(yStart & 0xff);
  lcdIli9341_writeData(yEnd >> 8);
  lcdIli9341_writeData(yEnd & 0xff);

  lcdIli9341_writeCmd(0x2c);
}

void lcdIli9341_drawRect(uint16_t xStart, uint16_t yStart, uint16_t width, uint16_t height, uint16_t color)
{
  lcdIli9341_setArea(xStart, yStart, xStart + width - 1, yStart + height - 1);
  for( uint16_t y = 0; y < height; y++ ){
    for( uint16_t x = 0; x < width; x++ ){
//      lcdIli9341_writeData(color >> 8);
//      lcdIli9341_writeData(color);
      * ((uint16_t*)LCD_DATA_ADDR) = color;
    }
  }
}

uint16_t* lcdIli9341_getDrawAddress()
{
  return (uint16_t*)LCD_DATA_ADDR;
}

RET lcdIli9341_init()
{
  lcdIli9341_initLowLevelIO();
  lcdIli9341_initLowLevelFSMC();
  lcdIli9341_initSequence();

//  lcdIli9341_drawRect(0, 0, LCD_ILI9342_WIDTH, LCD_ILI9342_HEIGHT, LCD_ILI9342_COLOR_RED);
//  lcdIli9341_drawRect(0, 0, LCD_ILI9342_WIDTH, LCD_ILI9342_HEIGHT, LCD_ILI9342_COLOR_GREEN);
//  lcdIli9341_drawRect(0, 0, LCD_ILI9342_WIDTH, LCD_ILI9342_HEIGHT, LCD_ILI9342_COLOR_BLUE);
//  lcdIli9341_drawRect(0, 0, 100, 100, LCD_ILI9342_COLOR_RED);
//  lcdIli9341_drawRect(100, 100, 50, 50, LCD_ILI9342_COLOR_GREEN);
//  lcdIli9341_drawRect(200, 100, 120, 140, LCD_ILI9342_COLOR_BLUE);
//
//  lcdIli9341_writeCmd(0xd3);
//  lcdIli9341_readData();
//  lcdIli9341_readData();
//  lcdIli9341_readData();
//  lcdIli9341_readData();

  lcdIli9341_drawRect(0, 0, LCD_ILI9342_WIDTH, LCD_ILI9342_HEIGHT, 0xffff);
  lcdIli9341_setArea(0, 0, LCD_ILI9342_WIDTH - 1, LCD_ILI9342_HEIGHT - 1);

  return OK;
}


/*** Internal Function Defines ***/
static RET lcdIli9341_initLowLevelIO()
{
  /*** RCC Configuration ***/
  RCC_AHB3PeriphClockCmd(RCC_AHB3Periph_FSMC, ENABLE);
  RCC_AHB1PeriphClockCmd(GPIO_RS_CLK | GPIO_CS_CLK | GPIO_WR_CLK | GPIO_RD_CLK | GPIO_D0_CLK | GPIO_D4_CLK, ENABLE);
#ifdef BIT_WIDTH_16
  RCC_AHB1PeriphClockCmd(GPIO_D8_CLK | GPIO_D13_CLK, ENABLE);
#endif

  /*** GPIO Configuration ***/
  GPIO_InitTypeDef  GPIO_InitStructure;
  GPIO_InitStructure.GPIO_Pin   = GPIO_RS_PIN;
  GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIO_RS_PORT, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin   = GPIO_CS_PIN;
  GPIO_Init(GPIO_CS_PORT, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin   = GPIO_WR_PIN;
  GPIO_Init(GPIO_WR_PORT, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin   = GPIO_RD_PIN;
  GPIO_Init(GPIO_RD_PORT, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin   = GPIO_D0_PIN;
  GPIO_Init(GPIO_D0_PORT, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin   = GPIO_D4_PIN;
  GPIO_Init(GPIO_D4_PORT, &GPIO_InitStructure);

#ifdef BIT_WIDTH_16
  GPIO_InitStructure.GPIO_Pin   = GPIO_D8_PIN;
  GPIO_Init(GPIO_D8_PORT, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin   = GPIO_D13_PIN;
  GPIO_Init(GPIO_D13_PORT, &GPIO_InitStructure);
#endif

  /*** Alternative functions for FSMC ***/
  GPIO_PinAFConfig(GPIO_RS_PORT, GPIO_RS_PIN_SOURCE, GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIO_CS_PORT, GPIO_CS_PIN_SOURCE, GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIO_WR_PORT, GPIO_WR_PIN_SOURCE, GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIO_RD_PORT, GPIO_RD_PIN_SOURCE, GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIO_D0_PORT, GPIO_D0_PIN_SOURCE, GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIO_D0_PORT, GPIO_D1_PIN_SOURCE, GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIO_D0_PORT, GPIO_D2_PIN_SOURCE, GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIO_D0_PORT, GPIO_D3_PIN_SOURCE, GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIO_D4_PORT, GPIO_D4_PIN_SOURCE, GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIO_D4_PORT, GPIO_D5_PIN_SOURCE, GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIO_D4_PORT, GPIO_D6_PIN_SOURCE, GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIO_D4_PORT, GPIO_D7_PIN_SOURCE, GPIO_AF_FSMC);
#ifdef BIT_WIDTH_16
  GPIO_PinAFConfig(GPIO_D8_PORT, GPIO_D8_PIN_SOURCE, GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIO_D8_PORT, GPIO_D9_PIN_SOURCE, GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIO_D8_PORT, GPIO_D10_PIN_SOURCE, GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIO_D8_PORT, GPIO_D11_PIN_SOURCE, GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIO_D8_PORT, GPIO_D12_PIN_SOURCE, GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIO_D13_PORT, GPIO_D13_PIN_SOURCE, GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIO_D13_PORT, GPIO_D14_PIN_SOURCE, GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIO_D13_PORT, GPIO_D15_PIN_SOURCE, GPIO_AF_FSMC);
#endif

  return OK;
}


static RET lcdIli9341_initLowLevelFSMC()
{
  FSMC_NORSRAMInitTypeDef  FSMC_NORSRAMInitStructure;
  FSMC_NORSRAMTimingInitTypeDef FSMC_NORSRAMTimingInitStructure;
  FSMC_NORSRAMInitStructure.FSMC_Bank                  = FSMC_Bank1_NORSRAM1;
  FSMC_NORSRAMInitStructure.FSMC_DataAddressMux        = FSMC_DataAddressMux_Disable;
  FSMC_NORSRAMInitStructure.FSMC_MemoryType            = FSMC_MemoryType_SRAM;
#ifdef BIT_WIDTH_16
  FSMC_NORSRAMInitStructure.FSMC_MemoryDataWidth       = FSMC_MemoryDataWidth_16b;
#else
  FSMC_NORSRAMInitStructure.FSMC_MemoryDataWidth       = FSMC_MemoryDataWidth_8b;
#endif
  FSMC_NORSRAMInitStructure.FSMC_BurstAccessMode       = FSMC_BurstAccessMode_Disable;
  FSMC_NORSRAMInitStructure.FSMC_WaitSignalPolarity    = FSMC_WaitSignalPolarity_Low;
  FSMC_NORSRAMInitStructure.FSMC_WrapMode              = FSMC_WrapMode_Disable;
  FSMC_NORSRAMInitStructure.FSMC_WaitSignalActive      = FSMC_WaitSignalActive_BeforeWaitState;
  FSMC_NORSRAMInitStructure.FSMC_WriteOperation        = FSMC_WriteOperation_Enable;
  FSMC_NORSRAMInitStructure.FSMC_WaitSignal            = FSMC_WaitSignal_Disable;
  FSMC_NORSRAMInitStructure.FSMC_AsynchronousWait      = FSMC_AsynchronousWait_Disable;
  FSMC_NORSRAMInitStructure.FSMC_ExtendedMode          = FSMC_ExtendedMode_Disable;
  FSMC_NORSRAMInitStructure.FSMC_WriteBurst            = FSMC_WriteBurst_Enable;
  FSMC_NORSRAMInitStructure.FSMC_ReadWriteTimingStruct = &FSMC_NORSRAMTimingInitStructure;
  FSMC_NORSRAMInitStructure.FSMC_WriteTimingStruct     = &FSMC_NORSRAMTimingInitStructure;
  FSMC_NORSRAMTimingInitStructure.FSMC_AddressSetupTime       = 1;
  FSMC_NORSRAMTimingInitStructure.FSMC_AddressHoldTime        = 1;
  FSMC_NORSRAMTimingInitStructure.FSMC_DataSetupTime          = 4;
  FSMC_NORSRAMTimingInitStructure.FSMC_BusTurnAroundDuration  = 0;
  FSMC_NORSRAMTimingInitStructure.FSMC_CLKDivision            = 1;
  FSMC_NORSRAMTimingInitStructure.FSMC_DataLatency            = 0;
  FSMC_NORSRAMTimingInitStructure.FSMC_AccessMode             = FSMC_AccessMode_A;

  FSMC_NORSRAMInit(&FSMC_NORSRAMInitStructure);
  FSMC_NORSRAMCmd(FSMC_Bank1_NORSRAM1, ENABLE);
  return OK;
}


static RET lcdIli9341_initSequence()
{
//  GPIO_SetBits(GPIO_RESET_PORT, GPIO_RESET_PIN);  delay(10);
//  GPIO_ResetBits(GPIO_RESET_PORT, GPIO_RESET_PIN);  delay(10);
//  GPIO_SetBits(GPIO_RESET_PORT, GPIO_RESET_PIN);  delay(10);

  lcdIli9341_writeCmd(0x01); //software reset
  delay(50);
  lcdIli9341_writeCmd(0x11); //exit sleep
  delay(50);

  lcdIli9341_writeCmd(0xB6);
  lcdIli9341_writeData(0x0A);
  lcdIli9341_writeData(0xC2);

  lcdIli9341_writeCmd(0x36);   // memory access control
  lcdIli9341_writeData(0x68);     // BGR -> seems RGB
//  lcdIli9341_writeData(0x60);     // RGB -> seems BGR

  lcdIli9341_writeCmd(0x3A); // pixel format
  lcdIli9341_writeData(0x55); //RGB565 (16bit)

  lcdIli9341_writeCmd(0xE0); //gamma
  lcdIli9341_writeData(0x10);
  lcdIli9341_writeData(0x10);
  lcdIli9341_writeData(0x10);
  lcdIli9341_writeData(0x08);
  lcdIli9341_writeData(0x0E);
  lcdIli9341_writeData(0x06);
  lcdIli9341_writeData(0x42);
  lcdIli9341_writeData(0x28);
  lcdIli9341_writeData(0x36);
  lcdIli9341_writeData(0x03);
  lcdIli9341_writeData(0x0E);
  lcdIli9341_writeData(0x04);
  lcdIli9341_writeData(0x13);
  lcdIli9341_writeData(0x0E);
  lcdIli9341_writeData(0x0C);

  lcdIli9341_writeCmd(0XE1); //gamma
  lcdIli9341_writeData(0x0C);
  lcdIli9341_writeData(0x23);
  lcdIli9341_writeData(0x26);
  lcdIli9341_writeData(0x04);
  lcdIli9341_writeData(0x0C);
  lcdIli9341_writeData(0x04);
  lcdIli9341_writeData(0x39);
  lcdIli9341_writeData(0x24);
  lcdIli9341_writeData(0x4B);
  lcdIli9341_writeData(0x03);
  lcdIli9341_writeData(0x0B);
  lcdIli9341_writeData(0x0B);
  lcdIli9341_writeData(0x33);
  lcdIli9341_writeData(0x37);
  lcdIli9341_writeData(0x0F);

  lcdIli9341_writeCmd(0x2a);//
  lcdIli9341_writeData(0x00);
  lcdIli9341_writeData(0x00);
  lcdIli9341_writeData(0x00);
  lcdIli9341_writeData(0xef);

  lcdIli9341_writeCmd(0x2b); //
  lcdIli9341_writeData(0x00);
  lcdIli9341_writeData(0x00);
  lcdIli9341_writeData(0x01);
  lcdIli9341_writeData(0x3f);

  lcdIli9341_writeCmd(0x29);
  delay(10);
  lcdIli9341_writeCmd(0x2C);

  return OK;
}

static void lcdIli9341_setDMA(uint16_t* pBuffer, uint32_t num)
{
  NVIC_InitTypeDef NVIC_InitStructure;
  DMA_InitTypeDef  DMA_InitStructure;
  RCC_AHB1PeriphClockCmd(DMA_STREAM_CLOCK, ENABLE);

  DMA_DeInit(DMA_STREAM);
  while(DMA_GetCmdStatus(DMA_STREAM) != DISABLE);

  NVIC_InitStructure.NVIC_IRQChannel                   = DMA_STREAM_IRQ;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority        = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd                = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

  DMA_InitStructure.DMA_Channel            = DMA_CHANNEL;
  DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)pBuffer;
  DMA_InitStructure.DMA_Memory0BaseAddr    = (uint32_t)LCD_DATA_ADDR;
  DMA_InitStructure.DMA_DIR                = DMA_DIR_MemoryToMemory;
  DMA_InitStructure.DMA_BufferSize         = num;
  DMA_InitStructure.DMA_PeripheralInc      = DMA_PeripheralInc_Disable;
  DMA_InitStructure.DMA_MemoryInc          = DMA_MemoryInc_Disable;
#ifdef BIT_WIDTH_16
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
  DMA_InitStructure.DMA_MemoryDataSize     = DMA_MemoryDataSize_HalfWord;
#else
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
  DMA_InitStructure.DMA_MemoryDataSize     = DMA_MemoryDataSize_Byte;
#endif
  DMA_InitStructure.DMA_Mode               = DMA_Mode_Normal;
  DMA_InitStructure.DMA_Priority           = DMA_Priority_High;
  DMA_InitStructure.DMA_FIFOMode           = DMA_FIFOMode_Disable;
  DMA_InitStructure.DMA_FIFOThreshold      = DMA_FIFOThreshold_Full;
  DMA_InitStructure.DMA_MemoryBurst        = DMA_MemoryBurst_Single;
  DMA_InitStructure.DMA_PeripheralBurst    = DMA_PeripheralBurst_Single;
  DMA_Init(DMA_STREAM, &DMA_InitStructure);
  DMA_ITConfig(DMA_STREAM, DMA_IT_TC, ENABLE);
  DMA_Cmd(DMA_STREAM, ENABLE);
  while(DMA_GetCmdStatus(DMA_STREAM) != ENABLE);
}

void DMA_STREAM_IRQHANDLER(void)
{
  /* Test on DMA Stream Transfer Complete interrupt */
  if(DMA_GetITStatus(DMA_STREAM, DMA_IT_TCIF))
  {
    /* Clear DMA Stream Transfer Complete interrupt pending bit */
    DMA_ClearITPendingBit(DMA_STREAM, DMA_IT_TCIF);

//    printf("end = %d\n", getSysTickCount());
    s_isWorking = 0;
    videoBuffer_doneDispBuffer();
  }
}

#ifdef BIT_WIDTH_16
inline static void lcdIli9341_writeCmd(uint16_t cmd)
#else
inline static void lcdIli9341_writeCmd(uint8_t cmd)
#endif
{
#if defined(BIT_WIDTH_16) && defined(BIT_SWAP_D0_D8)
  cmd = (cmd & 0xff) << 8;    // LCD_D[0:7] is connected to FSMCD[8:15]
  LCD_CMD = cmd;
#else
  LCD_CMD = cmd;
#endif
}

#ifdef BIT_WIDTH_16
inline static void lcdIli9341_writeData(uint16_t data)
#else
inline static void lcdIli9341_writeData(uint8_t data)
#endif
{
#if defined(BIT_WIDTH_16) && defined(BIT_SWAP_D0_D8)
  data = (data & 0xff) << 8;    // LCD_D[0:7] is connected to FSMCD[8:15]
  LCD_DATA = data;
#else
  LCD_DATA = data;
#endif
}

inline static void lcdIli9341_readData()
{
#ifdef BIT_WIDTH_16
  uint16_t data = LCD_DATA;
  printf("%04X\n", data);
#else
  uint8_t data = LCD_DATA;
  printf("%02X\n", data);
#endif

}

uint16_t convRGB565(uint8_t r, uint8_t g, uint8_t b)
{
  r >>= 3;
  g >>= 2;
  b >>= 3;
  uint8_t dataHigh = r << 3 | g >> 3;
  uint8_t dataLow = (g & 0x07) << 5 | b;
//  return dataHigh<< 8 | dataLow;
  return dataLow<< 8 | dataHigh;
}

