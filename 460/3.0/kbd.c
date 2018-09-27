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

#include "keymap"
#define KCNTL 0x00 // 7-6- 5(0=AT) 4=RXIntEn 3=TxIntEn
#define KSTAT 0x04 // 7-6=TxE 5=TxBusy 4= RXFull 3=RxBusy
#define KDATA 0x08 // data register;
#define KCLK  0x0C // clock divisor register; (not used)
#define KISTA 0x10 // interrupt status register; (not used)

typedef volatile struct kbd{ // base = 0x10006000
  char *base;     // base address of KBD, as char *
  char buf[128];  // input buffer
  int head, tail, data, room;   // control variables
}KBD;

volatile KBD kbd;   // KBD data structure

int kbd_init()
{
  KBD *kp = &kbd;
  kp->base = (char *)0x10006000;
  *(kp->base + KCNTL) = 0x14;  // 00010100=INTenable, Enable
  *(kp->base + KCLK)  = 8;   // PL051 manual says a value 0 to 15
  kp->head = kp->tail = 0;   // counters
  kp->data = 0; kp->room = 128;   // index to buffer
}

void kbd_handler()  // KBD interrupt handler in C
{
  u8 scode, c;
  KBD *kp = &kbd;
  color = YELLOW;   // int color in vid.c file

  scode = *(kp->base + KDATA); // read scan code in data register

  /* printf("key_pressed: %d\n", key_pressed); */

  if (scode & 0x80) return;

  if (scode & 0x80) { // ignore key releases
    /* printf("key released\n"); */
    if (shift == 1) {
      if (key_pressed == 1) {
        shift = 0;
        after_shift = 1;
        printf("shift off\n");
      }
    }
    key_pressed--;
    return;
  }

  if (after_shift) {
    after_shift = 0;
    return;
  }

  if(already) {
    already = 0;
    return;
  }

  if(key_pressed < 2) {
    key_pressed++;    
  }
  if (scode == 0x2A || scode == 0x36) {
    printf("shift on\n");
    c = unsh[scode];                  // map scan code to ascii
    shift = 1;
  }
  else {
    already = 1;
    printf("scode: %x\n", scode);
    if (shift == 0) { 
      c = unsh[scode];                // map scan code to ascii
    }
    else {
      c = sh[scode];                  // map scan code to ascii
    }
    /* if (c >= 'a' && c <= 'z') */
    printf("kbd interrupt: c=%s %c\n", c, c);
  }
  kp->buf[kp->head++] = c;  // enter key into circular buf[]
  kp->head %= 128;
  kp->data++; kp->room--; // update counters
}

int kgetc()  // main program calls kgetc() to return a char
{
  char c;
  KBD *kp = &kbd;

  unlock();  // enable IRQ interrupts
  while(kp->data == 0);  // wait for data; READONLY

  lock();   // disable IRQ interrupts
  c = kp->buf[kp->tail++];   // get a c and update tail index
  kp->tail %= 128;
  kp->data--; kp->room++;  // update with interrupts OFF
  unlock();   // enable IRQ interrupts

  
  return c;
}

int kgets(char s[ ])   // get a string from KBD
{
  char c;
  while( (c = kgetc()) != '\r'){
    *s++ = c;
  }
  *s = 0;
  return strlen(s);
}
