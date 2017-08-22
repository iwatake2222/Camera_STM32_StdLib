/*
 * sysTick.c
 *
 *  Created on: 2017/08/04
 *      Author: take-iwiw
 */
#include "stm32f4xx.h"
#include <stdio.h>
#include "../type.h"
#include "../system.h"

static uint32_t s_sysTickCnt = 0;

void SysTick_Handler(void)
{
  s_sysTickCnt++;
}

uint32_t getSysTickCount()
{
  return s_sysTickCnt;
}


void delay(uint32_t wait)
{
  uint32_t start = getSysTickCount();
  uint32_t end;
  end = (uint32_t)(start + wait);
  if (start <= end) {
    while(end >= getSysTickCount());
  } else {
    while(start <= getSysTickCount());
    while(end >= getSysTickCount());
  }
}
