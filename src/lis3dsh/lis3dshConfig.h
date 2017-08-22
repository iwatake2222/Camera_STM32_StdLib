/*
 * lis3dshConfig.h
 *
 *  Created on: 2017/08/09
 *      Author: take-iwiw
 */

#ifndef LIS3DSH_LIS3DSHCONFIG_H_
#define LIS3DSH_LIS3DSHCONFIG_H_

/*** port map ***/
#define SPI_CLK                RCC_APB2Periph_SPI1
#define GPIO_SCK_CLK           RCC_AHB1Periph_GPIOA
#define GPIO_MOSI_CLK          RCC_AHB1Periph_GPIOA
#define GPIO_MISO_CLK          RCC_AHB1Periph_GPIOA
#define GPIO_CS_CLK            RCC_AHB1Periph_GPIOE
#define GPIO_SCK_PORT          GPIOA
#define GPIO_SCK_PIN           GPIO_Pin_5
#define GPIO_SCK_PIN_SOURCE    GPIO_PinSource5
#define GPIO_SCK_AF            GPIO_AF_SPI1
#define GPIO_MOSI_PORT          GPIOA
#define GPIO_MOSI_PIN           GPIO_Pin_7
#define GPIO_MOSI_PIN_SOURCE    GPIO_PinSource7
#define GPIO_MOSI_AF            GPIO_AF_SPI1
#define GPIO_MISO_PORT          GPIOA
#define GPIO_MISO_PIN           GPIO_Pin_6
#define GPIO_MISO_PIN_SOURCE    GPIO_PinSource6
#define GPIO_MISO_AF            GPIO_AF_SPI1
#define GPIO_CS_PORT          GPIOE
#define GPIO_CS_PIN           GPIO_Pin_3
#define SPIx   SPI1

#endif /* LIS3DSH_LIS3DSHCONFIG_H_ */
