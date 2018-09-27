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

#include "defines.h"  // LCD, Timer and UART addresses
#include "string.c"   // strcmp, strlen, etc

char *tab = "0123456789ABCDEF";
int color;
int shift = 0;
int already = 0;
int key_pressed = 0;
int after_shift = 0;
int done = 0;

u32 cpsr;

#include "timer.c"          // timer handler file
#include "vid.c"            // LCD driver file
#include "interrupts.c"
#include "kbd.c"

void copy_vectors(void) {    // copy vector table in ts.s to 0x0
    extern u32 vectors_start;
    extern u32 vectors_end;
    u32 *vectors_src = &vectors_start;
    u32 *vectors_dst = (u32 *)0;

    while(vectors_src < &vectors_end)
       *vectors_dst++ = *vectors_src++;
}
int kprintf(char *fmt, ...);
void timer_handler();

// IRQ interrupts handler entry point
/* void __attribute__((interrupt)) IRQ_handler(); */
/* timer0 base=0x101E2000; timer1 base=0x101E2020 */
/* timer3 base=0x101E3000; timer1 base=0x101E3020 */
/* currentValueReg=0x04 */
TIMER *tp[4];

void IRQ_handler() // IRQ interrupt handler in C
{

    int vicstatus, sicstatus; 
    int ustatus, kstatus;

    // read VIC status registers to determine interrupt source
    vicstatus = VIC_STATUS;
    sicstatus = SIC_STATUS;
    /* kprintf("vicstatus=%x sicstatus=%x\n", vicstatus, sicstatus);  */
    if (getcpsr() != cpsr) {
      if (!done) {
        kprintf("Previous cpsr: %x\n", cpsr);
        kprintf("Previous mode is SVC mode\n");

        cpsr = getcpsr();
        kprintf("IRQ handler: CPU in IRQ mode cpsr: %x\n", cpsr);
        done = 1;
      }
    }
    
    // VIC status BITs: timer0,1=4, uart0=13, uart1=14, SIC=31: KBD at 3
    
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
    
    /* if (vicstatus & (1<<4)){   // timer0,1=bit4 */
    /*   if (*(tp[0]->base+TVALUE)==0) // timer 0 */
    /*      timer_handler(0); */
    /*   if (*(tp[1]->base+TVALUE)==0) */
    /*      timer_handler(1); */
    /* } */
    /* if (vicstatus & (1<<5)){   // timer2,3=bit5 */
    /*    if(*(tp[2]->base+TVALUE)==0) */
    /*      timer_handler(2); */
    /*    if (*(tp[3]->base+TVALUE)==0) */
    /*      timer_handler(3); */
    /* } */
    /* if (vicstatus & (1<<4)){   // timer0,1=bit4 */
    /*      timer_handler(0); */
    /* } */
    /* if (vicstatus & (1<<31)){ */
    /*   if (sicstatus & (1<<3)){ */
    /*       kbd_handler(); */
    /*    } */
    /* } */
}

int main()
{
   int i; 
   char line[128];

   color = YELLOW; // int color in vid.c file
   row = col = 0; 
   fbuf_init();         // initialize LCD driver

   /* enable timer0,1, uart0,1 SIC interrupts */
   VIC_INTENABLE |= (1<<4);  // timer0,1 at VIC.bit4
   VIC_INTENABLE |= (1<<5);  // timer2,3 at VIC.bit5

   VIC_INTENABLE |= (1<<31); // SIC to VIC's IRQ31

   /* enable KBD IRQ */
   SIC_ENSET = 1<<3;     // KBD int=3 on SIC
   SIC_PICENSET = 1<<3;  // KBD int=3 on SIC

   cpsr = getcpsr();
   kprintf("CPU is in SVC mode\ncpsr: %x\n", cpsr);
 
   kprintf("C3.2 start: test timer KBD drivers by interrupts\n");
   timer_init();
   kbd_init(); // initialize keyboard driver
   
   /* for (i=0; i<4; i++){        /\* start all 4 timers *\/ */
   /*    tp[i] = &timer[i]; */
   /*    timer_start(i); */
   /* } */
   timer_start(3);                 // start timer 3
   while(1){
     color = CYAN;
     kprintf("Enter a line from KBD\n");
     kgets(line);
     color = CYAN;
     kprintf("line = %s\n", line);
   }
}
