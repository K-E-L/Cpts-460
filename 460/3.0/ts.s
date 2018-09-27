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

.text
.code 32

.global reset
.global vectors_start, vectors_end
.global lock, unlock

.global getcpsr
.global getspsr
        
reset:	
vectors_start:
        // vector table: same as before
  LDR PC, reset_handler_addr
  LDR PC, undef_handler_addr
  LDR PC, swi_handler_addr
  LDR PC, prefetch_abort_handler_addr
  LDR PC, data_abort_handler_addr
  B .
  LDR PC, irq_handler_addr
  LDR PC, fiq_handler_addr

reset_handler_addr:          .word reset_handler
undef_handler_addr:          .word undef_handler
swi_handler_addr:            .word swi_handler
prefetch_abort_handler_addr: .word prefetch_abort_handler
data_abort_handler_addr:     .word data_abort_handler
irq_handler_addr:            .word irq_handler
fiq_handler_addr:            .word fiq_handler

vectors_end:

reset_handler:  // entry point of program
  LDR sp, =svc_stack   /* set SVC mode stack */
  BL copy_vectors /* copy vector table to address 0 */
  MRS r0, cpsr    /* get Program Status Register */
  BIC r1, r0, #0x1F   
  ORR r1, r1, #0x12
  MSR cpsr, r1  /* go in IRQ mode */
  LDR sp, =irq_stack /* set IRQ stack */
  BIC r0, r0, #0x80 /* Enable IRQs */
  MSR cpsr, r0 /* go back in Supervisor mode */
  BL main  /* jump to main */
  B .

.align 4
irq_handler:

  sub	lr, lr, #4   // adjust lr
  stmfd	sp!, {r0-r10, fp, ip, lr} // save regs in IRQ stack

  bl	IRQ_handler   // call IRQ_handler() in C

  ldmfd	sp!, {r0-r10, fp, ip, pc}^  // return
lock:   // mask out IRQ interrupts
	mrs r0, cpsr
	orr r0, r0, #0x80    // set I bit means MASK out IRQ interrupts
	msr cpsr, r0
	mov pc, lr
unlock:   // mask in IRQ interrupts
	mrs r0, cpsr
	bic r0, r0, #0x80   // clr I bit means MASK in IRQ interrupts
	msr cpsr, r0
	mov pc, lr
	
getcpsr:
        mrs r0, cpsr
        mov pc, lr

getspsr:
        mrs r0, spsr
        mov pc, lr


.end
