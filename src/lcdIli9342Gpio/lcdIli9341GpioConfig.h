/*
 * lcdIli9341GpioConfig.h
 *
 *  Created on: 2017/08/10
 *      Author: take-iwiw
 */

#ifndef LCDILI9341_GPIO_LCDILI9341_GPIO_CONFIG_H_
#define LCDILI9341_GPIO_LCDILI9341_GPIO_CONFIG_H_

#define LCD_ILI9342_GPIO_WIDTH  320
#define LCD_ILI9342_GPIO_HEIGHT 240

#define LCD_ILI9342_GPIO_COLOR_RED    0xf800
#define LCD_ILI9342_GPIO_COLOR_GREEN  0x07e0
#define LCD_ILI9342_GPIO_COLOR_BLUE   0x001f

/*** port map ***/
#define GPIO_RESET_CLK     RCC_AHB1Periph_GPIOD
#define GPIO_RS_CLK        RCC_AHB1Periph_GPIOD
#define GPIO_CS_CLK        RCC_AHB1Periph_GPIOD
#define GPIO_D0_CLK        RCC_AHB1Periph_GPIOD
#define GPIO_D4_CLK        RCC_AHB1Periph_GPIOE
#define GPIO_WR_CLK        RCC_AHB1Periph_GPIOD
#define GPIO_RD_CLK        RCC_AHB1Periph_GPIOD

#define GPIO_RESET_PORT       GPIOD
#define GPIO_RESET_PIN        GPIO_Pin_10

#define GPIO_RS_PORT          GPIOD
#define GPIO_RS_PIN           GPIO_Pin_11

#define GPIO_CS_PORT          GPIOD
#define GPIO_CS_PIN           GPIO_Pin_7

#define GPIO_D0_PORT          GPIOD
#define GPIO_D0_PIN           (GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_14 | GPIO_Pin_15)

#define GPIO_D4_PORT          GPIOE
#define GPIO_D4_PIN           (GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10)

#define GPIO_WR_PORT          GPIOD
#define GPIO_WR_PIN           (GPIO_Pin_5)

#define GPIO_RD_PORT          GPIOD
#define GPIO_RD_PIN           (GPIO_Pin_4)

#endif /* LCDILI9341_GPIO_LCDILI9341_GPIO_CONFIG_H_ */
