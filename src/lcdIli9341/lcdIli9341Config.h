/*
 * lcdIli9341Config.h
 *
 *  Created on: 2017/08/10
 *      Author: take-iwiw
 */

#ifndef LCDILI9341_LCDILI9341_CONFIG_H_
#define LCDILI9341_LCDILI9341_CONFIG_H_

#define LCD_ILI9342_WIDTH  320
#define LCD_ILI9342_HEIGHT 240

#define BIT_WIDTH_16
//#define BIT_SWAP_D0_D8    // if LCD_D[0:7] = FSMC_D{8:15] and LCD_D[8:15] = FSMC_D{0:7]

#ifdef BIT_SWAP_D0_D8
#define LCD_ILI9342_COLOR_RED    0x00f8
#define LCD_ILI9342_COLOR_GREEN  0xe007
#define LCD_ILI9342_COLOR_BLUE   0x1f00
#else
#define LCD_ILI9342_COLOR_RED    0xf800
#define LCD_ILI9342_COLOR_GREEN  0x07e0
#define LCD_ILI9342_COLOR_BLUE   0x001f
#endif

/*** port map ***/
#define GPIO_RS_CLK        RCC_AHB1Periph_GPIOD
#define GPIO_CS_CLK        RCC_AHB1Periph_GPIOD
#define GPIO_WR_CLK        RCC_AHB1Periph_GPIOD
#define GPIO_RD_CLK        RCC_AHB1Periph_GPIOD
#define GPIO_D0_CLK        RCC_AHB1Periph_GPIOD
#define GPIO_D4_CLK        RCC_AHB1Periph_GPIOE
#define GPIO_D8_CLK        RCC_AHB1Periph_GPIOE
#define GPIO_D13_CLK       RCC_AHB1Periph_GPIOD

#define GPIO_RS_PORT          GPIOD
#define GPIO_RS_PIN           GPIO_Pin_11
#define GPIO_RS_PIN_SOURCE    GPIO_PinSource11
#define FSMC_Ax               16            // use A16 as RS

#define GPIO_CS_PORT          GPIOD
#define GPIO_CS_PIN           GPIO_Pin_7
#define GPIO_CS_PIN_SOURCE    GPIO_PinSource7
#define FSMC_NEx              1               // use subbank 1

#define GPIO_WR_PORT          GPIOD
#define GPIO_WR_PIN           (GPIO_Pin_5)
#define GPIO_WR_PIN_SOURCE    GPIO_PinSource5

#define GPIO_RD_PORT          GPIOD
#define GPIO_RD_PIN           (GPIO_Pin_4)
#define GPIO_RD_PIN_SOURCE    GPIO_PinSource4

#define GPIO_D0_PORT          GPIOD
#define GPIO_D0_PIN           (GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_14 | GPIO_Pin_15)
#define GPIO_D0_PIN_SOURCE    GPIO_PinSource14
#define GPIO_D1_PIN_SOURCE    GPIO_PinSource15
#define GPIO_D2_PIN_SOURCE    GPIO_PinSource0
#define GPIO_D3_PIN_SOURCE    GPIO_PinSource1

#define GPIO_D4_PORT          GPIOE
#define GPIO_D4_PIN           (GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10)
#define GPIO_D4_PIN_SOURCE    GPIO_PinSource7
#define GPIO_D5_PIN_SOURCE    GPIO_PinSource8
#define GPIO_D6_PIN_SOURCE    GPIO_PinSource9
#define GPIO_D7_PIN_SOURCE    GPIO_PinSource10

#define GPIO_D8_PORT          GPIOE
#define GPIO_D8_PIN           (GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15)
#define GPIO_D8_PIN_SOURCE    GPIO_PinSource11
#define GPIO_D9_PIN_SOURCE    GPIO_PinSource12
#define GPIO_D10_PIN_SOURCE    GPIO_PinSource13
#define GPIO_D11_PIN_SOURCE    GPIO_PinSource14
#define GPIO_D12_PIN_SOURCE    GPIO_PinSource15

#define GPIO_D13_PORT          GPIOD
#define GPIO_D13_PIN           (GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10)
#define GPIO_D13_PIN_SOURCE    GPIO_PinSource8
#define GPIO_D14_PIN_SOURCE    GPIO_PinSource9
#define GPIO_D15_PIN_SOURCE    GPIO_PinSource10

#define DMA_STREAM               DMA2_Stream1
#define DMA_CHANNEL              DMA_Channel_1
#define DMA_STREAM_CLOCK         RCC_AHB1Periph_DMA2
#define DMA_STREAM_IRQ           DMA2_Stream1_IRQn
#define DMA_IT_TCIF              DMA_IT_TCIF1
#define DMA_STREAM_IRQHANDLER    DMA2_Stream1_IRQHandler

#endif /* LCDILI9341_LCDILI9341_CONFIG_H_ */
