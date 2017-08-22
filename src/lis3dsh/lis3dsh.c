/*
 * lis3dsh.c
 *
 *  Created on: 2017/08/05
 *      Author: take-iwiw
 */
#include "stm32f4xx.h"
#include <stdio.h>
#include "../type.h"
#include "../system.h"
#include "./lis3dsh.h"
#include "./lis3dshConfig.h"

/*** Internal Const Values, Macros ***/
#define LIS3DSH_CS_LOW()       GPIO_ResetBits(GPIO_CS_PORT, GPIO_CS_PIN)
#define LIS3DSH_CS_HIGH()      GPIO_SetBits(GPIO_CS_PORT, GPIO_CS_PIN)

/*** Internal Static Variables ***/

/*** Internal Function Declarations ***/
static uint8_t lis3dsh_sendByte(uint8_t byte);
static void lis3dsh_write(uint8_t* pBuffer, uint8_t addr, uint8_t num);
static void lis3dsh_read(uint8_t* pBuffer, uint8_t addr, uint8_t num);


/*** External Function Defines ***/
RET lis3dsh_init()
{
  GPIO_InitTypeDef GPIO_InitStructure;
  SPI_InitTypeDef  SPI_InitStructure;

  /* Enable the SPI periph */
  RCC_APB2PeriphClockCmd(SPI_CLK, ENABLE);

  /* Enable SCK, MOSI and MISO GPIO clocks */
  RCC_AHB1PeriphClockCmd(GPIO_SCK_CLK | GPIO_MOSI_CLK | GPIO_MISO_CLK, ENABLE);

  /* Enable CS  GPIO clock */
  RCC_AHB1PeriphClockCmd(GPIO_CS_CLK, ENABLE);

  GPIO_PinAFConfig(GPIO_SCK_PORT,  GPIO_SCK_PIN_SOURCE,  GPIO_SCK_AF);
  GPIO_PinAFConfig(GPIO_MOSI_PORT, GPIO_MOSI_PIN_SOURCE, GPIO_MOSI_AF);
  GPIO_PinAFConfig(GPIO_MISO_PORT, GPIO_MISO_PIN_SOURCE, GPIO_MISO_AF);

  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_DOWN;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

  /* SPI SCK pin configuration */
  GPIO_InitStructure.GPIO_Pin = GPIO_SCK_PIN;
  GPIO_Init(GPIO_SCK_PORT, &GPIO_InitStructure);

  /* SPI  MOSI pin configuration */
  GPIO_InitStructure.GPIO_Pin =  GPIO_MOSI_PIN;
  GPIO_Init(GPIO_MOSI_PORT, &GPIO_InitStructure);

  /* SPI MISO pin configuration */
  GPIO_InitStructure.GPIO_Pin = GPIO_MISO_PIN;
  GPIO_Init(GPIO_MISO_PORT, &GPIO_InitStructure);

  /* SPI configuration -------------------------------------------------------*/
  SPI_I2S_DeInit(SPIx);
  SPI_InitStructure.SPI_Direction         = SPI_Direction_2Lines_FullDuplex;
  SPI_InitStructure.SPI_DataSize          = SPI_DataSize_8b;
  SPI_InitStructure.SPI_CPOL              = SPI_CPOL_High;
  SPI_InitStructure.SPI_CPHA              = SPI_CPHA_2Edge;
  SPI_InitStructure.SPI_NSS               = SPI_NSS_Soft;
  SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_4;
  SPI_InitStructure.SPI_FirstBit          = SPI_FirstBit_MSB;
  SPI_InitStructure.SPI_CRCPolynomial     = 7;
  SPI_InitStructure.SPI_Mode              = SPI_Mode_Master;
  SPI_Init(SPIx, &SPI_InitStructure);

  /* Enable SPI1  */
  SPI_Cmd(SPIx, ENABLE);

  /* Configure GPIO PIN for Lis Chip select */
  GPIO_InitStructure.GPIO_Pin   = GPIO_CS_PIN;
  GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIO_CS_PORT, &GPIO_InitStructure);

  /* Deselect : Chip Select high */
  LIS3DSH_CS_HIGH();

  /* Configure MEMS: 1600 Hz, XYZ = en */
  uint8_t ctrl = (uint8_t) ( (9 << 4) | (0x07) );

  /* Write value to MEMS CTRL_REG1 register */
  lis3dsh_write(&ctrl, 0x20, 1);

  return OK;
}

RET lis3dsh_readAcc(int32_t out[3])
{
  uint8_t buffer[6];

  lis3dsh_read(buffer, 0x28, 6);
  out[0] = (((int8_t)buffer[1]) << 8) | buffer[0];
  out[1] = (((int8_t)buffer[3]) << 8) | buffer[2];
  out[2] = (((int8_t)buffer[5]) << 8) | buffer[4];

  return OK;
 }


/*** Internal Function Defines ***/
static uint8_t lis3dsh_sendByte(uint8_t data)
{
  while (SPI_I2S_GetFlagStatus(SPIx, SPI_I2S_FLAG_TXE) == RESET);

  /* Send a Byte through the SPI peripheral */
  SPI_I2S_SendData(SPIx, data);

  /* Wait to receive a Byte */
  while (SPI_I2S_GetFlagStatus(SPIx, SPI_I2S_FLAG_TXE) == RESET);

  /* Return the Byte read from the SPI bus */
  return (uint8_t)SPI_I2S_ReceiveData(SPIx);
}

static void lis3dsh_write(uint8_t* pBuffer, uint8_t addr, uint8_t num)
{
  /* Set chip select Low at the start of the transmission */
  LIS3DSH_CS_LOW();

  /* Send the Address of the indexed register */
  lis3dsh_sendByte(addr);
  /* Send the data that will be written into the device (MSB First) */
  while(num >= 1)
  {
    lis3dsh_sendByte(*pBuffer);
    num--;
    pBuffer++;
  }

  /* Set chip select High at the end of the transmission */
  LIS3DSH_CS_HIGH();
}

static void lis3dsh_read(uint8_t* pBuffer, uint8_t addr, uint8_t num)
{
  addr |= (uint8_t)0x80;

  /* Set chip select Low at the start of the transmission */
  LIS3DSH_CS_LOW();

  /* Send the Address of the indexed register */
  lis3dsh_sendByte(addr);

  /* Receive the data that will be read from the device (MSB First) */
  while(num > 0x00)
  {
    /* Send dummy byte (0x00) to generate the SPI clock to LIS302DL (Slave device) */
    *pBuffer = lis3dsh_sendByte(0x00);
    num--;
    pBuffer++;
  }

  /* Set chip select High at the end of the transmission */
  LIS3DSH_CS_HIGH();
}
