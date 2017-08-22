/*
 * main.c
 *
 *  Created on: 2017/08/05
 *      Author: take-iwiw
 */
#include "stm32f4xx.h"
#include <stdio.h>
#include "./type.h"
#include "./system.h"
#include "./debugMonitor/debugMonitor.h"
#include "./gpio/gpio.h"
#include "./adc/adc.h"
#include "./pwm/pwm.h"
#include "./lis3dsh/lis3dsh.h"
#include "./oledSh1106/oledSh1106.h"
#include "./lcdIli9341/lcdIli9341.h"
#include "./videoBuffer/videoBuffer.h"
#include "./ov7670/ov7670.h"

int main(void)
{
  SysTick_Config(SystemCoreClock / 1000);   // 1msec
  retarget_init();
  gpio_init();
  pwm_init();
  adc_init();
  lis3dsh_init();
//  oledSh1106_init();
  lcdIli9341_init();
  ov7670_init();

  printf("Hello world\n");

  // automatically send image data to LCD
  ov7670_config(OV7670_MODE_QVGA_RGB565);
  ov7670_startCap(OV7670_CAP_CONTINUOUS);


  while(1){
    debugMonitorDo();
//    colorLines();
//    imageFromUart();
  }

}





void imageFromUart()
{
  uint8_t data0, data1;
  while(1) {
    data0 = uartTerminal_recv();
    data1 = uartTerminal_recv();
      uint16_t color = (data1 << 8) | data0;
      *((uint16_t*)(lcdIli9341_getDrawAddress())) = color;
  }
}

void colorLines()
{
  uint16_t *buffer;
  RET ret;

  uint32_t start = getSysTickCount();
  uint16_t color[] = {0x00f8, 0xe007, 0x1f00, 0x00f8 + 0xe007};

  for( uint32_t i = 0; i < BUFFER_DIV; i++) {
    /* draw buffer */
    do{buffer = videoBuffer_getDrawBuffer();}while(buffer==0);
    for( uint32_t b = 0; b < BUFFER_WIDTH * BUFFER_HEIGHT; b++) *buffer++ = color[i];
    videoBuffer_doneDrawBuffer();
    /* display buffer */
    do{buffer = videoBuffer_getDispBuffer();}while(buffer==0);
    do{ret = lcdIli9341_refreshDisplay(0, buffer, BUFFER_PIXEL_NUM);}while(ret!=OK);
  }

//  printf("%d fps\n", 1000 / (getSysTickCount()-start));
}
