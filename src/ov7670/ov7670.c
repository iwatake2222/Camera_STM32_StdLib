/*
 * ov7670.c
 *
 *  Created on: 2017/08/13
 *      Author: take-iwiw
 */
#include "stm32f4xx.h"
#include <stdio.h>
#include "../type.h"
#include "../system.h"
#include "./ov7670.h"
#include "./ov7670Config.h"
#include "./ov7670Reg.h"

/*** Internal Const Values, Macros ***/
#define DCMI_DR_ADDRESS       0x50050028

#define I2C_WAIT_EVENT(EVENT) {\
    for(uint32_t WAIT_NUM = 0; (WAIT_NUM <= I2C_TIMEOUT) && !I2C_CheckEvent(I2Cx, EVENT); WAIT_NUM++){\
      if(WAIT_NUM == I2C_TIMEOUT) {printf("err\n");return ERR_TIMEOUT;}\
    }\
}

/*** Internal Static Variables ***/

/*** Internal Function Declarations ***/
static RET ov7670_initComm();
static RET ov7670_initDcmiIo();
static RET ov7670_initDcmi();
static RET ov7670_write(uint8_t regAddr, uint8_t data);
static RET ov7670_read(uint8_t regAddr, uint8_t *data);
static RET ov7670_i2cWrite(uint8_t slaveAddr, uint8_t regAddr, uint16_t dataNum, uint8_t data[]);
static RET ov7670_i2cRead(uint8_t slaveAddr, int8_t dataNum, uint8_t data[]);


/*** External Function Defines ***/
RET ov7670_init()
{
  /* MCO */
  RCC_AHB1PeriphClockCmd(GPIO_MCLK_CLK, ENABLE);
//  RCC_ClockSecuritySystemCmd(ENABLE);
  GPIO_InitTypeDef  GPIO_InitStructure;
  GPIO_InitStructure.GPIO_Pin   = GPIO_MCLK_PIN;
  GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIO_MCLK_PORT, &GPIO_InitStructure);
  GPIO_PinAFConfig(GPIO_MCLK_PORT, GPIO_MCLK_PIN_SOURCE, GPIO_MCLK_AF);
  RCC_MCO1Config(RCC_MCO1Source_PLLCLK, RCC_MCO1Div_4); // clk = 42MHz(168/4)
                                                        // then, divided by 2 in OV7670 pre scaler, and becomes 21MHz
//  RCC_MCO1Config(RCC_MCO1Source_HSI, RCC_MCO1Div_1);  // clk = 16 MHz

  ov7670_initComm();

  ov7670_write(0x12, 0x80);  // RESET
  delay(30);

//  uint8_t buffer[4];
//  ov7670_read(0x0b, buffer);
//  printf("%02X\n", buffer[0]);

  ov7670_initDcmiIo();

  return OK;
}

RET ov7670_config(uint32_t mode)
{
  ov7670_stopCap();
  ov7670_write(0x12, 0x80);  // RESET
  delay(30);
  for(int i = 0; OV7670_reg[i][0] != REG_BATT; i++) {
    ov7670_write(OV7670_reg[i][0], OV7670_reg[i][1]);
    delay(1);
  }
  return OK;
}

RET ov7670_startCap(uint32_t cont)
{
  ov7670_stopCap();
  ov7670_initDcmi(cont);
  DCMI_CaptureCmd(ENABLE);
  return OK;
}

RET ov7670_stopCap()
{
  DCMI_CaptureCmd(DISABLE);
  delay(30);
  return OK;
}

/*** Internal Function Defines ***/
static RET ov7670_initDcmiIo()
{
  /* RCC Configuration */
  RCC_AHB1PeriphClockCmd(GPIO_PCLK_CLK | GPIO_VS_CLK | GPIO_HS_CLK | GPIO_D0_CLK | GPIO_D1_CLK | GPIO_D2_CLK |
                         GPIO_D3_CLK | GPIO_D4_CLK | GPIO_D5_CLK | GPIO_D6_CLK | GPIO_D7_CLK, ENABLE);

  /* GPIO Configuration */
  GPIO_InitTypeDef GPIO_InitStructure;
  GPIO_InitStructure.GPIO_Pin   = GPIO_PCLK_PIN;
  GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_DOWN;
  GPIO_Init(GPIO_PCLK_PORT, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin = GPIO_VS_PIN;
  GPIO_Init(GPIO_VS_PORT, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin = GPIO_HS_PIN;
  GPIO_Init(GPIO_HS_PORT, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin = GPIO_D0_PIN;
  GPIO_Init(GPIO_D0_PORT, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin = GPIO_D1_PIN;
  GPIO_Init(GPIO_D1_PORT, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin = GPIO_D2_PIN;
  GPIO_Init(GPIO_D2_PORT, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin = GPIO_D3_PIN;
  GPIO_Init(GPIO_D3_PORT, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin = GPIO_D4_PIN;
  GPIO_Init(GPIO_D4_PORT, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin = GPIO_D5_PIN;
  GPIO_Init(GPIO_D5_PORT, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin = GPIO_D6_PIN;
  GPIO_Init(GPIO_D6_PORT, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin = GPIO_D7_PIN;
  GPIO_Init(GPIO_D7_PORT, &GPIO_InitStructure);

  GPIO_PinAFConfig(GPIO_PCLK_PORT, GPIO_PCLK_PIN_SOURCE, GPIO_AF_DCMI);
  GPIO_PinAFConfig(GPIO_VS_PORT, GPIO_VS_PIN_SOURCE, GPIO_AF_DCMI);
  GPIO_PinAFConfig(GPIO_HS_PORT, GPIO_HS_PIN_SOURCE, GPIO_AF_DCMI);
  GPIO_PinAFConfig(GPIO_D0_PORT, GPIO_D0_PIN_SOURCE, GPIO_AF_DCMI);
  GPIO_PinAFConfig(GPIO_D1_PORT, GPIO_D1_PIN_SOURCE, GPIO_AF_DCMI);
  GPIO_PinAFConfig(GPIO_D2_PORT, GPIO_D2_PIN_SOURCE, GPIO_AF_DCMI);
  GPIO_PinAFConfig(GPIO_D3_PORT, GPIO_D3_PIN_SOURCE, GPIO_AF_DCMI);
  GPIO_PinAFConfig(GPIO_D4_PORT, GPIO_D4_PIN_SOURCE, GPIO_AF_DCMI);
  GPIO_PinAFConfig(GPIO_D5_PORT, GPIO_D5_PIN_SOURCE, GPIO_AF_DCMI);
  GPIO_PinAFConfig(GPIO_D6_PORT, GPIO_D6_PIN_SOURCE, GPIO_AF_DCMI);
  GPIO_PinAFConfig(GPIO_D7_PORT, GPIO_D7_PIN_SOURCE, GPIO_AF_DCMI);


  return OK;
}

static RET ov7670_initDcmi(uint32_t cont)
{
  RCC_AHB2PeriphClockCmd(RCC_AHB2Periph_DCMI, ENABLE);
  RCC_AHB1PeriphClockCmd(DMA_STREAM_CLOCK, ENABLE);

  DCMI_InitTypeDef DCMI_InitStructure;
  if (cont == OV7670_CAP_CONTINUOUS) {
    DCMI_InitStructure.DCMI_CaptureMode = DCMI_CaptureMode_Continuous;
  } else if (cont == OV7670_CAP_SINGLE_FRAME) {
    DCMI_InitStructure.DCMI_CaptureMode = DCMI_CaptureMode_SnapShot;
  }
  DCMI_InitStructure.DCMI_SynchroMode = DCMI_SynchroMode_Hardware;
  DCMI_InitStructure.DCMI_PCKPolarity = DCMI_PCKPolarity_Rising;
  DCMI_InitStructure.DCMI_VSPolarity = DCMI_VSPolarity_High;
  DCMI_InitStructure.DCMI_HSPolarity = DCMI_HSPolarity_Low;
  DCMI_InitStructure.DCMI_CaptureRate = DCMI_CaptureRate_All_Frame;
  DCMI_InitStructure.DCMI_ExtendedDataMode = DCMI_ExtendedDataMode_8b;
  DCMI_Init(&DCMI_InitStructure);

  DMA_InitTypeDef  DMA_InitStructure;
  DMA_DeInit(DMA_STREAM);
  DMA_InitStructure.DMA_Channel = DMA_CHANNEL;
  DMA_InitStructure.DMA_PeripheralBaseAddr = DCMI_DR_ADDRESS;
  DMA_InitStructure.DMA_Memory0BaseAddr = lcdIli9341_getDrawAddress();
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;
  DMA_InitStructure.DMA_BufferSize = 1;
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Disable;
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Word;
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
  DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
  DMA_InitStructure.DMA_Priority = DMA_Priority_Low;
  DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;
  DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;
  DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
  DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
  DMA_Init(DMA_STREAM, &DMA_InitStructure);

  DMA_FlowControllerConfig(DMA_STREAM, DMA_FlowCtrl_Peripheral);

  DCMI_Cmd(ENABLE);
  DMA_Cmd(DMA_STREAM, ENABLE);

  return OK;
}

static RET ov7670_initComm()
{
  GPIO_InitTypeDef  GPIO_InitStructure;

  /* RCC Configuration */
  RCC_APB1PeriphClockCmd(I2C_CLK, ENABLE);
  RCC_AHB1PeriphClockCmd(GPIO_SCL_CLK | GPIO_SDA_CLK, ENABLE);

  /* Reset I2Cx IP */
  RCC_APB1PeriphResetCmd(I2C_CLK, ENABLE);
  RCC_APB1PeriphResetCmd(I2C_CLK, DISABLE);

  /* GPIO Configuration */
  GPIO_InitStructure.GPIO_Pin   = GPIO_SCL_PIN;
  GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
  GPIO_Init(GPIO_SCL_PORT, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin = GPIO_SDA_PIN;
  GPIO_Init(GPIO_SDA_PORT, &GPIO_InitStructure);

  GPIO_PinAFConfig(GPIO_SCL_PORT, GPIO_SCL_PIN_SOURCE, GPIO_SCL_AF);
  GPIO_PinAFConfig(GPIO_SDA_PORT, GPIO_SDA_PIN_SOURCE, GPIO_SDA_AF);


  /* I2C Struct Initialize */
  I2C_InitTypeDef  I2C_InitStructure;
  I2C_InitStructure.I2C_Mode                = I2C_Mode_I2C;
  I2C_InitStructure.I2C_DutyCycle           = I2C_DutyCycle_2;
  I2C_InitStructure.I2C_OwnAddress1         = 0xAA;
  I2C_InitStructure.I2C_Ack                 = I2C_Ack_Disable;
  I2C_InitStructure.I2C_ClockSpeed          = I2C_SPEED_KHZ * 1000;
  I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;

  /* I2C Initialize */
  I2C_DeInit(I2Cx);
  I2C_Init(I2Cx, &I2C_InitStructure);
  I2C_Cmd(I2Cx, ENABLE);

  return OK;
}



static RET ov7670_write(uint8_t regAddr, uint8_t data)
{
  RET ret;
  do {
    ret = ov7670_i2cWrite(SLAVE_ADDR, regAddr, 1, &data);
  } while (ret != OK);
  return OK;
}

static RET ov7670_read(uint8_t regAddr, uint8_t *data)
{
  RET ret;
  do {
    do {
      ret = ov7670_i2cWrite(SLAVE_ADDR, regAddr, 0, 0);
      if(ret != OK) {I2C_SoftwareResetCmd(I2Cx, ENABLE);I2C_SoftwareResetCmd(I2Cx, DISABLE);}
    } while (ret != OK);
    ret = ov7670_i2cRead(SLAVE_ADDR, 1, data);
    if(ret != OK) {I2C_SoftwareResetCmd(I2Cx, ENABLE);I2C_SoftwareResetCmd(I2Cx, DISABLE);}
  } while (ret != OK);
  return OK;
}

static RET ov7670_i2cWrite(uint8_t slaveAddr, uint8_t regAddr, uint16_t dataNum, uint8_t data[])
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

static RET ov7670_i2cRead(uint8_t slaveAddr, int8_t dataNum, uint8_t data[])
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



