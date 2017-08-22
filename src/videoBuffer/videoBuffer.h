/*
 * videoBuffer.h
 *
 *  Created on: 2017/08/13
 *      Author: take-iwiw
 */

#ifndef VIDEOBUFFER_VIDEOBUFFER_H_
#define VIDEOBUFFER_VIDEOBUFFER_H_

// screen size should be the same as display device(e.g. LCD)
#define SCREEN_WIDTH  320
#define SCREEN_HEIGHT 240

// buffer size is a part of one screen size to save memory
#define BUFFER_DIV    4

// use double buffer to perform both updating buffer and transferring to display(DMA) simultaneously
#define BUFFER_NUM    2

#define BUFFER_WIDTH  (SCREEN_WIDTH)
#define BUFFER_HEIGHT (SCREEN_HEIGHT/BUFFER_DIV)
#define BUFFER_PIXEL_NUM   (BUFFER_WIDTH*BUFFER_HEIGHT)



uint16_t* videoBuffer_getDrawBuffer();
void videoBuffer_doneDrawBuffer();
uint16_t* videoBuffer_getDispBuffer();
void videoBuffer_doneDispBuffer();


#endif /* VIDEOBUFFER_VIDEOBUFFER_H_ */
