/*
 * oledSh1106_I2C.h
 *
 * Created: 2016-03-19 7:29:18 PM
 *  Author: take-iwiw
 */ 


#ifndef OLEDSH1106_H_
#define OLEDSH1106_H_

RET oledSh1106_init();
RET oledSh1106_setPos(uint8_t x, uint8_t y);
RET oledSh1106_fillRect(uint8_t on, uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1);
uint8_t* oledSh1106_getBuffer();
RET oledSh1106_refresh();
RET oledSh1106_sendCommand(uint8_t data);
RET oledSh1106_sendDataByte(uint8_t data);
RET oledSh1106_sendDataBurst(uint8_t n, uint8_t data[]);

// for text output
RET oledSh1106_putchar(char c);
RET oledSh1106_setCurPos(uint8_t textX, uint8_t textY);


#endif /* OLEDSH1106_H_ */
