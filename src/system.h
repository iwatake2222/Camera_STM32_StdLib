/*
 * system.h
 *
 *  Created on: 2017/08/09
 *      Author: take-iwiw
 */

#ifndef SYSTEM_H_
#define SYSTEM_H_

void retarget_init();
uint32_t getSysTickCount();
void delay(uint32_t wait);

#endif /* SYSTEM_H_ */
