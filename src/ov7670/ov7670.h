/*
 * ov7670.h
 *
 *  Created on: 2017/08/13
 *      Author: take-iwiw
 */

#ifndef OV7670_OV7670_H_
#define OV7670_OV7670_H_

#define OV7670_MODE_QVGA_RGB565 0
#define OV7670_MODE_QVGA_YUV    1


#define OV7670_CAP_CONTINUOUS   0
#define OV7670_CAP_SINGLE_FRAME 1

RET ov7670_init();
RET ov7670_config(uint32_t mode);
RET ov7670_startCap(uint32_t cont);
RET ov7670_stopCap();

#endif /* OV7670_OV7670_H_ */
