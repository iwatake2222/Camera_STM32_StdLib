/*
 * videoBuffer.c
 *
 *  Created on: 2017/08/13
 *      Author: take-iwiw
 */
#include "stm32f4xx.h"
#include <stdio.h>
#include "../type.h"
#include "../system.h"
#include "./videoBuffer.h"

/*** Internal Const Values, Macros ***/
typedef enum {
  FREE = 0,
  DRAWING,
  READY_TO_DISPLAY,
  DISPLAYING,
}BUFFER_STATUS;

/*** Internal Static Variables ***/
uint16_t s_drawBuffer[BUFFER_NUM][BUFFER_WIDTH * BUFFER_HEIGHT];
BUFFER_STATUS s_bufferStatus[BUFFER_NUM] = {0};
int8_t s_bufferIndexDraw = -1;
int8_t s_bufferIndexDisp = -1;

/*** Internal Function Declarations ***/

/*** External Function Defines ***/
uint16_t* videoBuffer_getDrawBuffer()
{
  int8_t index = (s_bufferIndexDraw + 1) % BUFFER_NUM;
  if(s_bufferStatus[index] != FREE) return 0;
  for(uint8_t buffer = 0; buffer < BUFFER_NUM; buffer++) {
    if(s_bufferStatus[buffer] == DRAWING) return 0;  // do not allow to draw 2> buffers
  }
  s_bufferIndexDraw = index;
  s_bufferStatus[index] = DRAWING;
  return s_drawBuffer[index];
}

void videoBuffer_doneDrawBuffer()
{
  s_bufferStatus[s_bufferIndexDraw] = READY_TO_DISPLAY;
}

uint16_t* videoBuffer_getDispBuffer()
{
  int8_t index = (s_bufferIndexDisp + 1) % BUFFER_NUM;
  if(s_bufferStatus[index] != READY_TO_DISPLAY) return 0;
  for(uint8_t buffer = 0; buffer < BUFFER_NUM; buffer++) {
    if(s_bufferStatus[buffer] == DISPLAYING) return 0;  // do not allow to display 2> buffers
  }
  s_bufferIndexDisp = index;
  s_bufferStatus[index] = DISPLAYING;
  return s_drawBuffer[index];
}

void videoBuffer_doneDispBuffer()
{
  s_bufferStatus[s_bufferIndexDisp] = FREE;
}
