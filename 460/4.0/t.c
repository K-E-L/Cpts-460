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

int color;
int shift = 0;
int already = 0;
int key_pressed = 0;
int after_shift = 0;
int done = 0;

// from #6
int bpid, bppid, bfunc, bpriority;

#include "type.h"     // PROC type and constants
#include "string.c"   // string operation functions
#include "vid.c"      // LCD display driver
#include "timer.c"
#include "queue.c"    // list and queue operation functions
#include "kbd.c"      // KBD driver
#include "exceptions.c"
#include "kernel.c"

void copy_vectors(void) {
    extern u32 vectors_start;
    extern u32 vectors_end;
    u32 *vectors_src = &vectors_start;
    u32 *vectors_dst = (u32 *)0;

    while(vectors_src < &vectors_end)
       *vectors_dst++ = *vectors_src++;
}
int kprintf(char *fmt, ...);
void timer_handler();

TIMER *tp[4];
void IRQ_handler()     // handles only KBD insterrupts
{
    int vicstatus, sicstatus;
    int ustatus, kstatus;

    // read VIC status register to find out which interrupt
    vicstatus = VIC_STATUS; // VIC_STATUS=0x10140000=status reg
    sicstatus = SIC_STATUS;  
    //kprintf("vicstatus=%x sicstatus=%x\n", vicstatus, sicstatus);
    if (vicstatus & 0x0010){   /* timer0,1=bit4 */
      if (*(tp[0]->base+TVALUE)==0) /* timer 0 */
        timer_handler(0);
      if (*(tp[1]->base+TVALUE)==0) // timer 1
        timer_handler(1);
    }
    if (vicstatus & 0x0020){   /* timer2,3=bit5 */
      if(*(tp[2]->base+TVALUE)==0) // timer 2
         timer_handler(2);
      if (*(tp[3]->base+TVALUE)==0)  // timer 3
         timer_handler(3);
    }
    if (vicstatus & 0x80000000){
       if (sicstatus & 0x08){
          kbd_handler();
       }
    }
}

int body(bpid, bppid, bfunc, bpriority);

// KBD f == c, s == s, q == q, z == w,

int main()
{ 
   int i; 
   char line[128]; 
   u8 kbdstatus, key, scode;

   color = WHITE;
   row = col = 0;
   fbuf_init();     // initialize LCD display

   VIC_INTENABLE |= (1<<4);  // timer0,1 at VIC.bit4
   VIC_INTENABLE |= (1<<5);  // timer2,3 at VIC.bit5

   timer_init();
   kbd_init();     // initialize KBD driver

   /* enable KBD IRQ */
   VIC_INTENABLE |= 1<<31;  // SIC to VIC's IRQ31
   SIC_ENSET |= 1<<3;       // KBD int=3 on SIC

   kprintf("Welcome to WANIX in Arm\n");
   timer_start(3);                 // start timer 3

   init();   // initialize kernel, create and run P0
   kfork((int)body, 1);   // P0 create P1 in readyQueue
   
   while(1){
     if (readyQueue)
        tswitch();
   }
}
