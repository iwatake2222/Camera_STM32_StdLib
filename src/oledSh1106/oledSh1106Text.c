/*
 * oledSh1106Text.c
 *
 * Created: 2016-03-19 7:59:04 PM
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

/*** Internal Static Variables ***/
// x, y is pixel
static uint8_t s_currentPosX = TEXT_AREA_ORIGIN_X;
static uint8_t s_currentPosY = TEXT_AREA_ORIGIN_Y;

/*** Internal Function Declarations ***/
static void oledSh1106_drawChar(char c);

/*** External Function Defines ***/
RET oledSh1106_setCurPos(uint8_t textX, uint8_t textY)
{
	s_currentPosX = TEXT_AREA_ORIGIN_X + textX * (FONT_WIDTH * FONT_DISPLAY_SIZE);
	s_currentPosY = TEXT_AREA_ORIGIN_Y + textY * (FONT_HEIGHT * FONT_DISPLAY_SIZE);
	if( s_currentPosX > TEXT_AREA_END_X ) {
		s_currentPosX = TEXT_AREA_ORIGIN_X;
	}
	if( s_currentPosY > TEXT_AREA_END_Y ) {
		s_currentPosY = TEXT_AREA_ORIGIN_Y;
	}

  return OK;
}

RET oledSh1106_putchar(char c)
{
	uint8_t endX = s_currentPosX + FONT_WIDTH * FONT_DISPLAY_SIZE;
	uint8_t endY = s_currentPosY + FONT_HEIGHT * FONT_DISPLAY_SIZE;
	if(c == '\r' || c == '\n') {
	  endX = TEXT_AREA_END_X + 1;
	}
	if( endX > TEXT_AREA_END_X ) {
		s_currentPosX = TEXT_AREA_ORIGIN_X;
		endX = s_currentPosX + FONT_WIDTH * FONT_DISPLAY_SIZE;
		s_currentPosY += FONT_HEIGHT * FONT_DISPLAY_SIZE;
		endY = s_currentPosY + FONT_HEIGHT * FONT_DISPLAY_SIZE;
		if( endY > TEXT_AREA_END_Y ) {
			s_currentPosY = TEXT_AREA_ORIGIN_Y;
			endY = s_currentPosY + FONT_HEIGHT * FONT_DISPLAY_SIZE;
		}
		oledSh1106_fillRect(0, TEXT_AREA_ORIGIN_X, s_currentPosY, TEXT_AREA_END_X, endY);
	}

	oledSh1106_setPos(s_currentPosX, s_currentPosY);
  if(c == '\r' || c == '\n') {
    return OK;
  }
	oledSh1106_drawChar(c);

	s_currentPosX = endX;

  return OK;
}

static void oledSh1106_drawChar(char c) {
  extern unsigned char font[];
	for (uint8_t i =0; i<FONT_WIDTH; i++ ) {	
		for(uint8_t sizeX = 0; sizeX < FONT_DISPLAY_SIZE; sizeX++){			
			uint8_t line = *(font+(c*FONT_WIDTH)+i);
			oledSh1106_sendDataByte(line);
			continue;
//			uint8_t data = 0;
//			uint8_t dataDigit = 0;
//			for(uint8_t lineDigit = 0; lineDigit < 8; )	{
//				for(uint8_t sizeY = 0; sizeY < FONT_DISPLAY_SIZE; sizeY++) {
//					dataDigit++;
//					data >>= 1;
//					if (line & 0x01) data+=0x80;
//				}
//				line >>= 1;
//				lineDigit++;
//				if(dataDigit==8) {
//				  oledSsd1306_sendDataByte(data);
//					data = 0;
//					dataDigit = 0;
//				}
//			}
		}
	}
}
