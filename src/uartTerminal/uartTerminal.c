/*
 * uartTerminal.c
 *
 *  Created on: 2017/08/02
 *      Author: take-iwiw
 */
#include "stm32f4xx.h"
#include <stdio.h>
#include "../type.h"
#include "../system.h"
#include "./uartTerminal.h"
#include "./uartTerminalConfig.h"

/*** Internal Const Values, Macros ***/

/*** Internal Static Variables ***/
#ifdef UART_TERMINAL_USE_INTERRUPT
#define UART_TERMINAL_BUFFER_SIZE_TX 16
#define UART_TERMINAL_BUFFER_SIZE_RX 16
static volatile uint8_t s_bufferTx[UART_TERMINAL_BUFFER_SIZE_TX];
static volatile uint8_t s_bufferRx[UART_TERMINAL_BUFFER_SIZE_RX];
static volatile uint8_t s_bufferTxWp = 0;
static volatile uint8_t s_bufferTxRp = 0;
static volatile uint8_t s_bufferRxWp = 0;
static volatile uint8_t s_bufferRxRp = 0;
#endif

/*** Internal Function Declarations ***/

/*** External Function Defines ***/
RET uartTerminal_init()
{
  /* config clock */
  RCC_APB1PeriphClockCmd(UART_CLK, ENABLE);
  RCC_AHB1PeriphClockCmd(GPIO_TX_CLK | GPIO_RX_CLK, ENABLE);

  /* config gpio and function */
  GPIO_InitTypeDef  GPIO_InitStructure;
  GPIO_InitStructure.GPIO_Pin   = GPIO_TX_PIN;
  GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_Init(GPIO_TX_PORT, &GPIO_InitStructure);
  GPIO_InitStructure.GPIO_Pin   = GPIO_RX_PIN;
  GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_Init(GPIO_RX_PORT, &GPIO_InitStructure);
  GPIO_PinAFConfig(GPIO_TX_PORT , GPIO_TX_PIN_SOURCE , GPIO_TX_AF);
  GPIO_PinAFConfig(GPIO_RX_PORT , GPIO_RX_PIN_SOURCE , GPIO_RX_AF);

  /* config uart */
  USART_InitTypeDef USART_InitStructure;
  USART_StructInit(&USART_InitStructure);
  USART_InitStructure.USART_BaudRate = UART_BAUDRATE;
  USART_Init(UARTx, &USART_InitStructure);
  USART_Cmd(UARTx, ENABLE);

#ifdef UART_TERMINAL_USE_INTERRUPT
  /* config NVIC */
  NVIC_InitTypeDef NVIC_InitStructure;
  NVIC_InitStructure.NVIC_IRQChannel                   = UART_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority        = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd                = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
  USART_ITConfig(UARTx, USART_IT_RXNE, ENABLE);
#endif

  return OK;
}

RET uartTerminal_send(uint8_t data)
{
#ifdef UART_TERMINAL_USE_INTERRUPT
//  USART_ITConfig(USART2, USART_IT_TXE, DISABLE);
  while (((s_bufferTxWp + 1) & (UART_TERMINAL_BUFFER_SIZE_TX - 1)) == s_bufferTxRp);
  s_bufferTx[s_bufferTxWp++] = data;
  s_bufferTxWp &= UART_TERMINAL_BUFFER_SIZE_TX - 1;
  USART_ITConfig(UARTx, USART_IT_TXE, ENABLE);
  return OK;
#else
  while (USART_GetFlagStatus(UARTx, USART_FLAG_TXE) == RESET);
  USART_SendData(UARTx, data);
  while (USART_GetFlagStatus(UARTx, USART_FLAG_TXE) == RESET);
  return OK;
#endif
}

uint8_t uartTerminal_recv()
{
#ifdef UART_TERMINAL_USE_INTERRUPT
  while (s_bufferRxWp == s_bufferRxRp);
  uint8_t data = s_bufferRx[s_bufferRxRp++];
  s_bufferRxRp &= UART_TERMINAL_BUFFER_SIZE_RX - 1;
  USART_ITConfig(UARTx, USART_IT_RXNE, ENABLE);
  return data;
#else
  while (USART_GetFlagStatus(UARTx, USART_FLAG_RXNE) == RESET);
  return USART_ReceiveData(UARTx);
#endif
}

RET uartTerminal_recvTry(uint8_t *data)
{
#ifdef UART_TERMINAL_USE_INTERRUPT
  if (s_bufferRxWp == s_bufferRxRp)
    return NO_DATA;
  *data = s_bufferRx[s_bufferRxRp++];
  s_bufferRxRp &= UART_TERMINAL_BUFFER_SIZE_RX - 1;
  USART_ITConfig(UARTx, USART_IT_RXNE, ENABLE);
  return OK;
#else
  if (USART_GetFlagStatus(UARTx, USART_FLAG_RXNE) == RESET) {
    return NO_DATA;
  } else {
    *data = USART_ReceiveData(UARTx);
    return OK;
  }
#endif
}

#ifdef UART_TERMINAL_USE_INTERRUPT
void UART_IRQHandler(void)
{
  if (USART_GetITStatus(UARTx, USART_IT_TXE) != RESET) {
    if (s_bufferTxWp != s_bufferTxRp) {
      USART_SendData(UARTx, s_bufferTx[s_bufferTxRp++]);
      s_bufferTxRp &= UART_TERMINAL_BUFFER_SIZE_TX - 1;
    } else {
      USART_ITConfig(UARTx, USART_IT_TXE, DISABLE);
    }
  }

  if (USART_GetITStatus(USART2, USART_IT_RXNE) != RESET) {
    if (((s_bufferRxWp + 1) & (UART_TERMINAL_BUFFER_SIZE_RX - 1)) == s_bufferRxRp) {
      // over flow
      USART_ITConfig(UARTx, USART_IT_RXNE, DISABLE);
    } else {
      s_bufferRx[s_bufferRxWp++] = USART_ReceiveData(UARTx);
#ifdef UART_TERMINAL_ECHO_BACK
      putchar(s_bufferRx[s_bufferRxWp-1]);
#endif
      s_bufferRxWp &= UART_TERMINAL_BUFFER_SIZE_RX - 1;
    }
  }
}
#endif
