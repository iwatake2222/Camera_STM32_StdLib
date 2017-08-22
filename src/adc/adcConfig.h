/*
 * adcConfig.h
 *
 *  Created on: 2017/08/09
 *      Author: take-iwiw
 */

#ifndef ADC_ADCCONFIG_H_
#define ADC_ADCCONFIG_H_

/*** config ***/
#define USE_DMA

/*** port map ***/
#define ADCx                     ADC3
#define ADC_CHANNEL              ADC_Channel_11
#define ADCx_CLK                 RCC_APB2Periph_ADC3
#define ADCx_CHANNEL_GPIO_CLK    RCC_AHB1Periph_GPIOC
#define GPIO_PIN                 GPIO_Pin_1
#define GPIO_PORT                GPIOC
#ifdef USE_DMA
#define DMA_CLK                  RCC_AHB1Periph_DMA2
#define DMA_CHANNELx             DMA_Channel_2
#define DMA_STREAMx              DMA2_Stream0
#define ADCx_DR_ADDRESS          ((uint32_t)0x4001224C)
#endif



#endif /* ADC_ADCCONFIG_H_ */
