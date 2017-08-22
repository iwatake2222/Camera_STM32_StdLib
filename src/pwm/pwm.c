/*
 * pwm.c
 *
 *  Created on: 2017/08/04
 *      Author: take-iwiw
 */

#include "stm32f4xx.h"
#include <stdio.h>
#include "../type.h"
#include "../system.h"
#include "./pwm.h"
#include "./pwmConfig.h"

RET pwm_init()
{
  GPIO_InitTypeDef GPIO_InitStructure;

  /* TIM clock enable */
  RCC_APB1PeriphClockCmd(TIM_CLK, ENABLE);

  /* GPIO clock enable */
  RCC_AHB1PeriphClockCmd(GPIO_CLK, ENABLE);

  /* GPIO Configuration */
  GPIO_InitStructure.GPIO_Pin   = GPIO_PIN;
  GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL ;
  GPIO_Init(GPIO_PORT, &GPIO_InitStructure);

  /* Connect TIM pins to AF */
  GPIO_PinAFConfig(GPIO_PORT, TIM_SOURCE, TIM_AF);

  return OK;
}

RET pwm_setDuty(uint8_t duty)
{
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
  TIM_OCInitTypeDef  TIM_OCInitStructure;
  uint16_t PrescalerValue = 0;
  /* Compute the prescaler value */
  PrescalerValue = (uint16_t) ((SystemCoreClock /2) / 21000000) - 1;

  /* Time base configuration */
  TIM_TimeBaseStructure.TIM_Period        = 665;
  TIM_TimeBaseStructure.TIM_Prescaler     = PrescalerValue;
  TIM_TimeBaseStructure.TIM_ClockDivision = 0;
  TIM_TimeBaseStructure.TIM_CounterMode   = TIM_CounterMode_Up;

  TIM_TimeBaseInit(TIMx, &TIM_TimeBaseStructure);

  /* PWM1 Mode configuration */
  TIM_OCInitStructure.TIM_OCMode      = TIM_OCMode_PWM1;
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
  TIM_OCInitStructure.TIM_Pulse       = (665 * duty) / 100;
  TIM_OCInitStructure.TIM_OCPolarity  = TIM_OCPolarity_High;

  TIM_OC1Init(TIMx, &TIM_OCInitStructure);
  TIM_OC1PreloadConfig(TIMx, TIM_OCPreload_Enable);

  TIM_OC2Init(TIMx, &TIM_OCInitStructure);
  TIM_OC2PreloadConfig(TIMx, TIM_OCPreload_Enable);

  TIM_OC3Init(TIMx, &TIM_OCInitStructure);
  TIM_OC3PreloadConfig(TIMx, TIM_OCPreload_Enable);

  TIM_OC4Init(TIMx, &TIM_OCInitStructure);
  TIM_OC4PreloadConfig(TIMx, TIM_OCPreload_Enable);

  TIM_ARRPreloadConfig(TIMx, ENABLE);

  /* TIM enable counter */
  TIM_Cmd(TIMx, ENABLE);

  return OK;
}
