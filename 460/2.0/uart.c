/********************************************************************
Copyright 2010-2017 K.C. Wang, <kwang@eecs.wsu.edu>
This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
********************************************************************/
#include <stdarg.h>

#define DR   0x00
#define FR   0x18

#define RXFE 0x10
#define TXFF 0x20

typedef struct uart{
  char *base;
  int n;
}UART;

UART uart[4];

int uart_init()
{
  int i; UART *up;

  /* for (i=0; i<4; i++){ */
  /*   up = &uart[i]; */
  /*   up->base = (char *)(0x101F1000 + i*0x1000); */
  /*   /\* up->base = (char *)(0x1000A000 + i*0x1000); *\/ */
  /*   up->n = i; */
  /* } */

  up = &uart[0];
  uart[0].base = (char *)(0x1000A000);
  up->n = 0;
  
  up = &uart[1];
  uart[1].base = (char *)(0x1000B000);
  up->n = 1;

  up = &uart[2];
  uart[2].base = (char *)(0x1000C000);
  up->n = 2;

  up = &uart[3];
  uart[3].base = (char *)(0x10009000); // uart3 at 0x10009000
  up->n = 3;
}

int ugetc(UART *up)
{
  while (*(up->base + FR) & RXFE);
  return *(up->base + DR);
}

int uputc(UART *up, char c)
{
  while(*(up->base + FR) & TXFF);
  *(up->base + DR) = c;
}

int ugets(UART *up, char *s)
{
  while ((*s = (char)ugetc(up)) != '\r'){
    uputc(up, *s);
    s++;
  }
 *s = 0;
}

int uprints(UART *up, char *s)
{
  while(*s)
    uputc(up, *s++);
}

int hasString(char *s) {
  while(*s) {
    if (*s == '%') {
      *s++;
      if (*s == 's') {
        return 1;
      }
    }
    *s++;
  }
}

int hasInt(char *s) {
  while(*s) {
    if (*s == '%') {
      *s++;
      if (*s == 'd') {
        return 1;
      }
    }
    *s++;
  }
}

void u_print_two_digits(UART *up, int num) {
  int ten, tens, ones, divided;
  ten = 10;
  ones = num;
  divided = 0;
  
  if (num < 9) {
    uputc(up, num + '0');
    return;
  }

  while (ones > 9) {
    ones -= 10;    
  }

  tens = num - ones;

  while (tens > 0) {
    divided++;
    tens -= 10;
  }
  
  uputc(up, divided + '0');
  uputc(up, ones + '0');
  uprints(up, "\n\r");
}

int calculateSum(UART *up, int *v) {
  int sum = 0;
  int i;
  for(i = 0; i < 10; i++) {
    sum += v[i];
  }

  return sum;
}

void fuprints(UART *up, char *fmt, ...) {
  va_list valist;
  char *fmt_temp = fmt;

  va_start(valist, fmt);



  while(*fmt_temp) {
    if (*fmt_temp == '%') {
      if(hasString(fmt)) {
        char *arg_temp = va_arg(valist, char*);
        uprints(up, arg_temp);
     }

      else if(hasInt(fmt)) {
        int arg_temp = va_arg(valist, int);
        u_print_two_digits(up, arg_temp);
      }

      *fmt_temp++;
      *fmt_temp++;
    }
    else {
      uputc(up, *fmt_temp++);      
    }
  }

  /* if(mystrcmp(fmt, "%s") == 0) { */
  /*   uprints(up, temp); */
  /* } */

  
  /* while(*fmt) */
  /*   uputc(up, *fmt++); */
}
