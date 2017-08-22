/*
 * uartTerminal.h
 *
 *  Created on: 2017/08/03
 *      Author: take-iwiw
 */

#ifndef UARTTERMINAL_H_
#define UARTTERMINAL_H_

RET uartTerminal_init();
RET uartTerminal_send(uint8_t data);
uint8_t uartTerminal_recv();
RET uartTerminal_recvTry(uint8_t *data);

#endif /* UARTTERMINAL_H_ */
