/*
 * gpioConfig.h
 *
 *  Created on: 2017/08/09
 *      Author: take-iwiw
 */

#ifndef GPIO_GPIOCONFIG_H_
#define GPIO_GPIOCONFIG_H_

/*** port map ***/
#define GPIO_LED_CLK         RCC_AHB1Periph_GPIOD
#define GPIO_LED_PIN_1       GPIO_Pin_12
#define GPIO_LED_PIN_2       GPIO_Pin_13
#define GPIO_LED_PIN_3       GPIO_Pin_14
#define GPIO_LED_PIN_4       GPIO_Pin_15
#define GPIO_LED_PORT        GPIOD

#define GPIO_BUTTON_CLK    RCC_AHB1Periph_GPIOA
#define GPIO_BUTTON_PIN    GPIO_Pin_0
#define GPIO_BUTTON_PORT   GPIOA
#define SYSCONFIG_CLK      RCC_APB2Periph_SYSCFG
#define EXTI_LINE          EXTI_Line0
#define EXTI_PORT_SOURCE   EXTI_PortSourceGPIOA
#define EXTI_PIN_SOURCE    EXTI_PinSource0
#define EXTI_IRQn          EXTI0_IRQn
#define EXTI_IRQHandler    EXTI0_IRQHandler


#endif /* GPIO_GPIOCONFIG_H_ */
