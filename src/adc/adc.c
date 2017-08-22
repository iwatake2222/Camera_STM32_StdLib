/*
 * adc.c
 *
 *  Created on: 2017/08/05
 *  Author: take-iwiw
 */
#include "stm32f4xx.h"
#include <stdio.h>
#include "../type.h"
#include "../system.h"
#include "./adc.h"
#include "./adcConfig.h"

/*** Internal Const Values, Macros ***/

/*** Internal Static Variables ***/
#ifdef USE_DMA
volatile static uint16_t uhADCxConvertedValue = 0;
#endif

/*** Internal Function Declarations ***/

/*** External Function Defines ***/
RET adc_init()
{
  ADC_InitTypeDef       ADC_InitStructure;
  ADC_CommonInitTypeDef ADC_CommonInitStructure;
  GPIO_InitTypeDef      GPIO_InitStructure;

  ADC_DeInit();

  RCC_AHB1PeriphClockCmd(ADCx_CHANNEL_GPIO_CLK, ENABLE);
  RCC_APB2PeriphClockCmd(ADCx_CLK, ENABLE);

#ifdef USE_DMA
  DMA_InitTypeDef       DMA_InitStructure;
  RCC_AHB1PeriphClockCmd(DMA_CLK, ENABLE);

  /* DMA Stream channel configuration **************************************/
  DMA_InitStructure.DMA_Channel            = DMA_CHANNELx;
  DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)ADCx_DR_ADDRESS;
  DMA_InitStructure.DMA_Memory0BaseAddr    = (uint32_t)&uhADCxConvertedValue;
  DMA_InitStructure.DMA_DIR                = DMA_DIR_PeripheralToMemory;
  DMA_InitStructure.DMA_BufferSize         = 1;
  DMA_InitStructure.DMA_PeripheralInc      = DMA_PeripheralInc_Disable;
  DMA_InitStructure.DMA_MemoryInc          = DMA_MemoryInc_Disable;
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
  DMA_InitStructure.DMA_MemoryDataSize     = DMA_MemoryDataSize_HalfWord;
  DMA_InitStructure.DMA_Mode               = DMA_Mode_Circular;
  DMA_InitStructure.DMA_Priority           = DMA_Priority_High;
  DMA_InitStructure.DMA_FIFOMode           = DMA_FIFOMode_Disable;
  DMA_InitStructure.DMA_FIFOThreshold      = DMA_FIFOThreshold_HalfFull;
  DMA_InitStructure.DMA_MemoryBurst        = DMA_MemoryBurst_Single;
  DMA_InitStructure.DMA_PeripheralBurst    = DMA_PeripheralBurst_Single;
  DMA_Init(DMA_STREAMx, &DMA_InitStructure);
  DMA_Cmd(DMA_STREAMx, ENABLE);
#endif

  /* Configure ADCx Channel pin as analog input ******************************/
  GPIO_InitStructure.GPIO_Pin  = GPIO_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIO_PORT, &GPIO_InitStructure);

  /* ADC Common Init **********************************************************/
  ADC_CommonInitStructure.ADC_Mode             = ADC_Mode_Independent;
  ADC_CommonInitStructure.ADC_Prescaler        = ADC_Prescaler_Div2;
  ADC_CommonInitStructure.ADC_DMAAccessMode    = ADC_DMAAccessMode_Disabled;
  ADC_CommonInitStructure.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_5Cycles;
  ADC_CommonInit(&ADC_CommonInitStructure);

  /* ADCx Init ****************************************************************/
  ADC_InitStructure.ADC_Resolution   = ADC_Resolution_12b;
  ADC_InitStructure.ADC_ScanConvMode = DISABLE;
#ifdef USE_DMA
  ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
#else
  ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;
#endif
  ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;
  ADC_InitStructure.ADC_ExternalTrigConv     = ADC_ExternalTrigConv_T1_CC1;
  ADC_InitStructure.ADC_DataAlign            = ADC_DataAlign_Right;
  ADC_InitStructure.ADC_NbrOfConversion      = 1;
  ADC_Init(ADCx, &ADC_InitStructure);

  /* ADCx regular channel configuration **************************************/
  ADC_RegularChannelConfig(ADCx, ADC_CHANNEL, 1, ADC_SampleTime_56Cycles);

#ifdef USE_DMA
 /* Enable DMA request after last transfer (Single-ADC mode) */
  ADC_DMARequestAfterLastTransferCmd(ADCx, ENABLE);
  /* Enable ADCx DMA */
  ADC_DMACmd(ADCx, ENABLE);
#endif

  /* Enable ADCx */
  ADC_Cmd(ADCx, ENABLE);

  delay(100);

  ADC_SoftwareStartConv(ADCx);

  return OK;
}

uint16_t adc_getValue()
{
#ifdef USE_DMA
  return uhADCxConvertedValue;
#else
  ADC_SoftwareStartConv(ADCx);
  while(ADC_GetFlagStatus(ADCx, ADC_FLAG_EOC) == RESET);
  return ADC_GetConversionValue(ADCx);
#endif

}
