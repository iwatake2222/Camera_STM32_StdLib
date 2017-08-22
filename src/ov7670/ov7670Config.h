/*
 * ov7670Config.h
 *
 *  Created on: 2017/08/13
 *      Author: take-iwiw
 */

#ifndef OV7670_OV7670CONFIG_H_
#define OV7670_OV7670CONFIG_H_

#define SLAVE_ADDR 0x42
#define I2C_SPEED_KHZ 400

#define I2C_TIMEOUT 1000


/*** port map ***/
/* for master clock (need to supply clock to camera module even to use i2c communication) */
#define GPIO_MCLK_CLK           RCC_AHB1Periph_GPIOA
#define GPIO_MCLK_PORT          GPIOA
#define GPIO_MCLK_PIN           GPIO_Pin_8
#define GPIO_MCLK_PIN_SOURCE    GPIO_PinSource8
#define GPIO_MCLK_AF            GPIO_AF_MCO

/* for communication */
#define I2Cx                   I2C2
#define I2C_CLK                RCC_APB1Periph_I2C2
#define GPIO_SCL_CLK           RCC_AHB1Periph_GPIOB
#define GPIO_SDA_CLK           RCC_AHB1Periph_GPIOB

#define GPIO_SCL_PORT          GPIOB
#define GPIO_SCL_PIN           GPIO_Pin_10
#define GPIO_SCL_PIN_SOURCE    GPIO_PinSource10
#define GPIO_SCL_AF            GPIO_AF_I2C2
#define GPIO_SDA_PORT          GPIOB
#define GPIO_SDA_PIN           GPIO_Pin_11
#define GPIO_SDA_PIN_SOURCE    GPIO_PinSource11
#define GPIO_SDA_AF            GPIO_AF_I2C2

/* for dcmi */
#define GPIO_PCLK_CLK          RCC_AHB1Periph_GPIOA
#define GPIO_VS_CLK            RCC_AHB1Periph_GPIOB
#define GPIO_HS_CLK            RCC_AHB1Periph_GPIOA
#define GPIO_D0_CLK            RCC_AHB1Periph_GPIOC
#define GPIO_D1_CLK            RCC_AHB1Periph_GPIOC
#define GPIO_D2_CLK            RCC_AHB1Periph_GPIOC
#define GPIO_D3_CLK            RCC_AHB1Periph_GPIOC
#define GPIO_D4_CLK            RCC_AHB1Periph_GPIOC
#define GPIO_D5_CLK            RCC_AHB1Periph_GPIOB
#define GPIO_D6_CLK            RCC_AHB1Periph_GPIOB
#define GPIO_D7_CLK            RCC_AHB1Periph_GPIOB

#define GPIO_PCLK_PORT          GPIOA
#define GPIO_PCLK_PIN           GPIO_Pin_6
#define GPIO_PCLK_PIN_SOURCE    GPIO_PinSource6

#define GPIO_VS_PORT          GPIOB
#define GPIO_VS_PIN           GPIO_Pin_7
#define GPIO_VS_PIN_SOURCE    GPIO_PinSource7

#define GPIO_HS_PORT          GPIOA
#define GPIO_HS_PIN           GPIO_Pin_4
#define GPIO_HS_PIN_SOURCE    GPIO_PinSource4

#define GPIO_D0_PORT          GPIOC
#define GPIO_D0_PIN           GPIO_Pin_6
#define GPIO_D0_PIN_SOURCE    GPIO_PinSource6

#define GPIO_D1_PORT          GPIOC
#define GPIO_D1_PIN           GPIO_Pin_7
#define GPIO_D1_PIN_SOURCE    GPIO_PinSource7

#define GPIO_D2_PORT          GPIOC
#define GPIO_D2_PIN           GPIO_Pin_8
#define GPIO_D2_PIN_SOURCE    GPIO_PinSource8

#define GPIO_D3_PORT          GPIOC
#define GPIO_D3_PIN           GPIO_Pin_9
#define GPIO_D3_PIN_SOURCE    GPIO_PinSource9

#define GPIO_D4_PORT          GPIOC
#define GPIO_D4_PIN           GPIO_Pin_4
#define GPIO_D4_PIN_SOURCE    GPIO_PinSource4

#define GPIO_D5_PORT          GPIOB
#define GPIO_D5_PIN           GPIO_Pin_6
#define GPIO_D5_PIN_SOURCE    GPIO_PinSource6

#define GPIO_D6_PORT          GPIOB
#define GPIO_D6_PIN           GPIO_Pin_8
#define GPIO_D6_PIN_SOURCE    GPIO_PinSource8

#define GPIO_D7_PORT          GPIOB
#define GPIO_D7_PIN           GPIO_Pin_9
#define GPIO_D7_PIN_SOURCE    GPIO_PinSource9

#define DMA_STREAM               DMA2_Stream7
#define DMA_CHANNEL              DMA_Channel_1
#define DMA_STREAM_CLOCK         RCC_AHB1Periph_DMA2
#define DMA_STREAM_IRQ           DMA2_Stream7_IRQn
#define DMA_STREAM_IRQHANDLER    DMA2_Stream7_IRQHandler

#endif /* OV7670_OV7670CONFIG_H_ */
