/*
 * uartTerminalConfig.h
 *
 *  Created on: 2017/08/09
 *      Author: take-iwiw
 */

#ifndef UARTTERMINAL_UARTTERMINALCONFIG_H_
#define UARTTERMINAL_UARTTERMINALCONFIG_H_

/*** config ***/
#define UART_TERMINAL_USE_INTERRUPT
//#define UART_TERMINAL_ECHO_BACK
#define UART_BAUDRATE 921600
//#define UART_BAUDRATE 9600


/*** port map ***/
#define UART_CLK            RCC_APB1Periph_USART2
#define GPIO_TX_CLK         RCC_AHB1Periph_GPIOA
#define GPIO_RX_CLK         RCC_AHB1Periph_GPIOA
#define GPIO_TX_PORT        GPIOA
#define GPIO_TX_PIN         GPIO_Pin_2
#define GPIO_TX_PIN_SOURCE  GPIO_PinSource2
#define GPIO_TX_AF          GPIO_AF_USART2
#define GPIO_RX_PORT        GPIOA
#define GPIO_RX_PIN         GPIO_Pin_3
#define GPIO_RX_PIN_SOURCE  GPIO_PinSource3
#define GPIO_RX_AF          GPIO_AF_USART2
#define UARTx               USART2

#define UART_IRQn          USART2_IRQn
#define UART_IRQHandler    USART2_IRQHandler

#endif /* UARTTERMINAL_UARTTERMINALCONFIG_H_ */
