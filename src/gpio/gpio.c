/*
 * gpio.c
 *
 *  Created on: 2017/08/04
 *      Author: take-iwiw
 */

#include "stm32f4xx.h"
#include <stdio.h>
#include "../type.h"
#include "../system.h"
#include "./gpio.h"
#include "./gpioConfig.h"

RET gpio_init()
{
  /* Enable the LED Clock */
  RCC_AHB1PeriphClockCmd(GPIO_LED_CLK, ENABLE);

  /* Configure LED pins as input */
  GPIO_InitTypeDef  GPIO_InitStructure;
  GPIO_InitStructure.GPIO_Pin   = GPIO_LED_PIN_1 | GPIO_LED_PIN_2 | GPIO_LED_PIN_3 | GPIO_LED_PIN_4;
  GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIO_LED_PORT, &GPIO_InitStructure);


  /* Enable the BUTTON Clock */
  RCC_AHB1PeriphClockCmd(GPIO_BUTTON_CLK, ENABLE);
  RCC_APB2PeriphClockCmd(SYSCONFIG_CLK, ENABLE);

  /* Configure Button pin as input */
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_InitStructure.GPIO_Pin  = GPIO_BUTTON_PIN;
  GPIO_Init(GPIO_BUTTON_PORT, &GPIO_InitStructure);

  /* Connect Button EXTI Line to Button GPIO Pin */
  SYSCFG_EXTILineConfig(EXTI_PORT_SOURCE, EXTI_PIN_SOURCE);

  /* Configure Button EXTI line */
  EXTI_InitTypeDef EXTI_InitStructure;
  EXTI_InitStructure.EXTI_Line    = EXTI_LINE;
  EXTI_InitStructure.EXTI_Mode    = EXTI_Mode_Interrupt;
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  EXTI_Init(&EXTI_InitStructure);

  /* Enable and set Button EXTI Interrupt to the lowest priority */
  NVIC_InitTypeDef NVIC_InitStructure;
  NVIC_InitStructure.NVIC_IRQChannel                   = EXTI_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority        = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd                = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

  return OK;
}

RET gpio_on()
{
  GPIO_SetBits(GPIO_LED_PORT, GPIO_LED_PIN_1 | GPIO_LED_PIN_2 | GPIO_LED_PIN_3 | GPIO_LED_PIN_4);
  return OK;
}

RET gpio_off()
{
  GPIO_ResetBits(GPIO_LED_PORT, GPIO_LED_PIN_1 | GPIO_LED_PIN_2 | GPIO_LED_PIN_3 | GPIO_LED_PIN_4);
  return OK;
}

void EXTI_IRQHandler(void)
{
  if(EXTI_GetITStatus(EXTI_LINE) != RESET)
  {
    /* Toggle LED1 */
    printf("EXTI0");

    /* Clear the EXTI line 0 pending bit */
    EXTI_ClearITPendingBit(EXTI_LINE);
  }
}

