/*
 * oledSh1106.c
 *
 *  Created on: 2017/08/05
 *  Author: take-iwiw
 */
#include "stm32f4xx.h"
#include <stdio.h>
#include "../type.h"
#include "../system.h"
#include "./oledSh1106.h"
#include "./oledSh1106Cmd.h"
#include "./oledSh1106Config.h"


/*** Internal Const Values, Macros ***/
#define CMD  0x00
#define DATA 0x40

#define I2C_WAIT_EVENT(EVENT) {\
    for(uint32_t WAIT_NUM = 0; (WAIT_NUM <= I2C_TIMEOUT) && !I2C_CheckEvent(I2Cx, EVENT); WAIT_NUM++){\
      if(WAIT_NUM == I2C_TIMEOUT) return ERR_TIMEOUT;\
    }\
}


/*** Internal Static Variables ***/
#ifdef USE_OUTPUT_DRAW_BUFFER
uint8_t s_oledSh1106_buffer[DRAW_AREA_WIDTH*DRAW_AREA_HEIGHT/OLED_SH1106_PAGE_SIZE];
#endif

/*** Internal Function Declarations ***/
static RET oledSh1106_initLowLevel();
static RET oledSh1106_i2cWrite(uint8_t slaveAddr, uint8_t regAddr, uint16_t dataNum, uint8_t data[]);
static RET oledSh1106_i2cRead(uint8_t slaveAddr, int8_t dataNum, uint8_t data[]);

/*** External Function Defines ***/
RET oledSh1106_init()
{
  oledSh1106_initLowLevel();

  /* init sequence */
  oledSh1106_sendCommand(SH1106_DISPLAYOFF);
  oledSh1106_sendCommand(SH1106_SETMULTIPLEX);
  oledSh1106_sendCommand(0x3F);
  oledSh1106_sendCommand(SH1106_SETDISPLAYOFFSET);
  oledSh1106_sendCommand(0);
  oledSh1106_sendCommand(SH1106_SETSTARTLINE | 0x00);
  oledSh1106_sendCommand(SH1106_SEGREMAP | 0x01);
  oledSh1106_sendCommand(SH1106_COMSCANDEC);
  oledSh1106_sendCommand(SH1106_SETCOMPINS);
  oledSh1106_sendCommand(0x12);
  oledSh1106_sendCommand(SH1106_SETCONTRAST);
  oledSh1106_sendCommand(0xCF);
  oledSh1106_sendCommand(SH1106_DISPLAYALLON_RESUME);
  oledSh1106_sendCommand(SH1106_NORMALDISPLAY);
  oledSh1106_sendCommand(SH1106_SETDISPLAYCLOCKDIV);
  oledSh1106_sendCommand(0x80);
  oledSh1106_sendCommand(SH1106_CHARGEPUMP);
  oledSh1106_sendCommand(0x14);
//  oledSH1106_sendCommand(SH1106_MEMORYMODE);
//  oledSH1106_sendCommand(0x00);  // vertical addressing mode
  oledSh1106_sendCommand(0x02);  /* column address = 2 */
  oledSh1106_sendCommand(0x10);
  oledSh1106_sendCommand(0x40);  /* display start line = 0 */
  oledSh1106_sendCommand(0xB0);  /* page start = 0 */
  oledSh1106_sendCommand(SH1106_DISPLAYON);

  oledSh1106_fillRect(0, 0, 0, OLED_SH1106_WIDTH, OLED_SH1106_HEIGHT);
  oledSh1106_putchar('O');
  oledSh1106_putchar('K');

  return OK;
}

// y must be 8 align
RET oledSh1106_setPos(uint8_t x, uint8_t y)
{
  x += 2; /* column pos offset of 2 */
  oledSh1106_sendCommand(x&0x0F);          /* lower column address */
  oledSh1106_sendCommand(0x10 | (x >> 4)); /* higher column address */
  oledSh1106_sendCommand(0xB0 + (y / OLED_SH1106_PAGE_SIZE));  /* row address */

  return OK;
}

// y must be 8 align
RET oledSh1106_fillRect(uint8_t on, uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1)
{
  uint8_t data = (on == 0 ? 0 : 0xff);

  for(uint8_t y = y0; y < y1; y += OLED_SH1106_PAGE_SIZE) {
    oledSh1106_setPos(x0, y);
    for(uint8_t x = x0; x < x1; x++) {
      oledSh1106_sendDataByte(data);
    }
  }

  return OK;

}

uint8_t* oledSh1106_getBuffer()
{
#ifdef USE_OUTPUT_DRAW_BUFFER
  return s_oledSh1106_buffer;
#else
  /* shouldn't be called */
  while(1);
  return (void*)0;
#endif
}



RET oledSh1106_refresh()
{
#ifdef USE_OUTPUT_DRAW_BUFFER
  for(uint8_t y = 0; y < DRAW_AREA_HEIGHT/OLED_SH1106_PAGE_SIZE; y++) {
    oledSh1106_setPos(DRAW_AREA_ORIGIN_X, DRAW_AREA_ORIGIN_Y + y*OLED_SH1106_PAGE_SIZE);
    oledSh1106_sendDataBurst(DRAW_AREA_WIDTH, &s_oledSh1106_buffer[0 + DRAW_AREA_WIDTH*y]);
  }
#endif

  return OK;
}

inline RET oledSh1106_sendCommand(uint8_t data)
{
  oledSh1106_i2cWrite(SLAVE_ADDR, CMD, 1, &data);
  return OK;
}

inline RET oledSh1106_sendDataByte(uint8_t data)
{
  oledSh1106_i2cWrite(SLAVE_ADDR, DATA, 1, &data);
  return OK;
}

inline RET oledSh1106_sendDataBurst(uint8_t n, uint8_t data[])
{
  oledSh1106_i2cWrite(SLAVE_ADDR, DATA, n, data);
  return OK;
}


/*** Internal Function Defines ***/
static RET oledSh1106_initLowLevel()
{
  GPIO_InitTypeDef  GPIO_InitStructure;

  /* RCC Configuration */
  /*I2C Peripheral clock enable */
  RCC_APB1PeriphClockCmd(I2C_CLK, ENABLE);

  /*SCL GPIO clock enable PA8*/
  RCC_AHB1PeriphClockCmd(GPIO_SCL_CLK, ENABLE);

  /*SDA GPIO clock enable PC9 */
  RCC_AHB1PeriphClockCmd(GPIO_SDA_CLK, ENABLE);

  /* Reset I2Cx IP */
  RCC_APB1PeriphResetCmd(I2C_CLK, ENABLE);

  /* Release reset signal of I2Cx IP */
  RCC_APB1PeriphResetCmd(I2C_CLK, DISABLE);

  /* GPIO Configuration */
  /*Configure I2C SCL pin */
  GPIO_InitStructure.GPIO_Pin   = GPIO_SCL_PIN;
  GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIO_SCL_PORT, &GPIO_InitStructure);

  /*Configure I2C SDA pin */
  GPIO_InitStructure.GPIO_Pin = GPIO_SDA_PIN;
  GPIO_Init(GPIO_SDA_PORT, &GPIO_InitStructure);

  /* Connect PXx to I2C_SCL */
  GPIO_PinAFConfig(GPIO_SCL_PORT, GPIO_SCL_PIN_SOURCE, GPIO_SCL_AF);

  /* Connect PXx to I2C_SDA */
  GPIO_PinAFConfig(GPIO_SDA_PORT, GPIO_SDA_PIN_SOURCE, GPIO_SDA_AF);

  /* I2C Struct Initialize */
  I2C_InitTypeDef  I2C_InitStructure;
  I2C_InitStructure.I2C_Mode                = I2C_Mode_I2C;
  I2C_InitStructure.I2C_DutyCycle           = I2C_DutyCycle_2;
  I2C_InitStructure.I2C_OwnAddress1         = 0xAA;
  I2C_InitStructure.I2C_Ack                 = I2C_Ack_Enable;
  I2C_InitStructure.I2C_ClockSpeed          = I2C_SPEED_KHZ * 1000;
  I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;

  /* I2C Initialize */
  I2C_Init(I2Cx, &I2C_InitStructure);

  I2C_Cmd(I2Cx, ENABLE);

  return OK;
}


static RET oledSh1106_i2cWrite(uint8_t slaveAddr, uint8_t regAddr, uint16_t dataNum, uint8_t data[])
{
  I2C_GenerateSTART(I2Cx, ENABLE);
  I2C_WAIT_EVENT(I2C_EVENT_MASTER_MODE_SELECT);

  I2C_Send7bitAddress(I2Cx, slaveAddr, I2C_Direction_Transmitter);
  I2C_WAIT_EVENT(I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED);

  I2C_SendData(I2Cx, regAddr);
  I2C_WAIT_EVENT(I2C_EVENT_MASTER_BYTE_TRANSMITTED);

  for(uint16_t i = 0; i < dataNum; i++) {
    I2C_SendData(I2Cx, *data);
    I2C_WAIT_EVENT(I2C_EVENT_MASTER_BYTE_TRANSMITTED);
    data++;
  }
  I2C_GenerateSTOP(I2Cx, ENABLE);

  return OK;
}

static RET oledSh1106_i2cRead(uint8_t slaveAddr, int8_t dataNum, uint8_t data[])
{
  I2C_GenerateSTART(I2Cx, ENABLE);
  I2C_WAIT_EVENT(I2C_EVENT_MASTER_MODE_SELECT);

  I2C_Send7bitAddress(I2Cx, slaveAddr, I2C_Direction_Receiver);
  I2C_WAIT_EVENT(I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED);

  if (dataNum > 1){
    I2C_AcknowledgeConfig(I2Cx, ENABLE);
    for(int8_t i = 0; i < dataNum - 1; i++) {
      I2C_WAIT_EVENT(I2C_EVENT_MASTER_BYTE_RECEIVED);
      *data = I2C_ReceiveData(I2Cx);
      data++;
    }
  }
  I2C_AcknowledgeConfig(I2Cx, DISABLE);
  I2C_GenerateSTOP(I2Cx, ENABLE);
  I2C_WAIT_EVENT(I2C_EVENT_MASTER_BYTE_RECEIVED);
  *data = I2C_ReceiveData(I2Cx);

  return OK;
}
