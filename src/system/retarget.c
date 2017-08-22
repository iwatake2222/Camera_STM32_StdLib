/*
 * retarget.c
 *
 *  Created on: 2017/07/30
 *      Author: take-iwiw
 */

#include <stdio.h>
#include "../type.h"
#include "../system.h"
#include "../uartTerminal/uartTerminal.h"
#include "../oledSh1106/oledSh1106.h"

void retarget_init()
{
  uartTerminal_init();

  setbuf(stdin, NULL);
  setbuf(stdout, NULL);
  setbuf(stderr, NULL);
}


#ifdef __GNUC__
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */

PUTCHAR_PROTOTYPE
{
  uartTerminal_send(ch);
//  oledSh1106_putchar(ch);
  return 1;
}

#ifdef __GNUC__
#define GETCHAR_PROTOTYPE int __io_getchar(void)
#else
#define GETCHAR_PROTOTYPE int fgetc(FILE *f)
#endif /* __GNUC__ */

GETCHAR_PROTOTYPE
{
  return uartTerminal_recv();
}



