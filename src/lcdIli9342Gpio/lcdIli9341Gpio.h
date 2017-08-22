/*
 * lcdIli9341Gpio.h
 *
 *  Created on: 2017/08/10
 *      Author: take-iwiw
 */

#ifndef LCDILI9341_GPIO_LCDILI9341_GPIO_H_
#define LCDILI9341_GPIO_LCDILI9341_GPIO_H_

RET lcdIli9341Gpio_init();
void lcdIli9341Gpio_blockWrite(uint16_t Xstart, uint16_t Ystart, uint16_t Xend, uint16_t Yend);
void lcdIli9341Gpio_dispColor(uint16_t Xstart, uint16_t Ystart, uint16_t Xend, uint16_t Yend, uint16_t color);


#endif /* LCDILI9341_GPIO_LCDILI9341_GPIO_H_ */
