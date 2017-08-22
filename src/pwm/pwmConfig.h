/*
 * pwmConfig.h
 *
 *  Created on: 2017/08/09
 *      Author: take-iwiw
 */

#ifndef PWM_PWMCONFIG_H_
#define PWM_PWMCONFIG_H_

/*** port map ***/
#define TIM_CLK        RCC_APB1Periph_TIM4
#define GPIO_CLK       RCC_AHB1Periph_GPIOD
#define GPIO_PIN       GPIO_Pin_12
#define GPIO_PORT      GPIOD
#define TIM_SOURCE     GPIO_PinSource12
#define TIM_AF         GPIO_AF_TIM4
#define TIMx           TIM4

#endif /* PWM_PWMCONFIG_H_ */
