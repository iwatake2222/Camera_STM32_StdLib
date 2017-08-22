/*
 * lcdIli9341.h
 *
 *  Created on: 2017/08/10
 *      Author: take-iwiw
 */

#ifndef LCDILI9341_LCDILI9341_H_
#define LCDILI9341_LCDILI9341_H_

RET lcdIli9341_init();

void lcdIli9341_setArea(uint16_t xStart, uint16_t yStart, uint16_t xEnd, uint16_t yEnd);
void lcdIli9341_drawRect(uint16_t xStart, uint16_t yStart, uint16_t width, uint16_t height, uint16_t color);
RET lcdIli9341_refreshDisplay(uint8_t isNewFrame, uint16_t* pBuffer, uint32_t num);
uint16_t* lcdIli9341_getDrawAddress();

#endif /* LCDILI9341_LCDILI9341_H_ */
