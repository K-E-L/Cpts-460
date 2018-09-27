
// queue.c file
extern PROC *freeList;
// WRITE YOUR OWN functions:

int enqueue(PROC **queue, PROC *p)  // enter q into queue by priority
{
  int SR = int_off();  // IRQ interrupts off, return CPSR
    /* enter p into *queue by priority; PROCs with the same priority by FIFO; */

  PROC *pPrev;
  PROC *pCur = *queue;

  // handles the [1 1]
  if (pCur == 0) {
    p->child = 0;
    *queue = p;
    int_on(SR);          //  restore CPSR 
    return 0;
  }

  // handles the [0 0]
  if (p->priority == 0) {
    pPrev = pCur;
    pCur = pCur->child;
    pPrev->child = p;
    p->child = pCur;
  
    int_on(SR);          //  restore CPSR
    return 0;
  }

  // handles the [2 0]
  if (pCur->pid == 0) {
    p->child = pCur;
    *queue = p;
    int_on(SR);          //  restore CPSR
    return 0;
  }

  // handles the rest
  while (pCur->pid != 0) {
    if (p->pid < pCur->pid) {
      break;
    }
    pPrev = pCur;
    pCur = pCur->child;
  }

  pPrev->child = p;
  p->child = pCur;
  
  int_on(SR);          //  restore CPSR 
}				     

PROC *dequeue(PROC **queue) // remove and return first PROC from queue
{
  int SR = int_off();  // IRQ interrupts off, return CPSR
    /* remove the FISRT element from *queue;  */

  PROC *pTemp = *queue;
  *queue = (*queue)->next;
  
  int_on(SR);          //  restore CPSR
  
  /* return pointer to dequeued PROC; */
  return pTemp;
}

int printList(char *name, PROC *p)  // print list elements
{
   kprintf("%s = ", name);
   while(p){
     kprintf("[%d%d]->", p->pid, p->priority);
     p = p->child;
  }
  kprintf("NULL\n"); 
}

/* sleep(int event) { */
/*   record event value in running PROC.event; */
/*   change running PROC status to SLEEP; */
/*   switch process; */
/* } */

/* wakeup(int event) { */
/*   for each PROC *p do { */
/*       if (p->status == SLEEP && p->event == event) { */
/*         change p->status to READY; */
/*         enter p into readyQueue; */
/*       } */
/*     } */
/* } */

/* int sleep(int event) { */
/*   int SR = int_off(); // disable IRQ and return CPSR */
/*   running->event = event; */
/*   running->status = SLEEP; */
/*   tswitch();     // switch process */
/*   int_on(SR);    // restore original CPSR */
/* } */

/* int wakeup(int event) { */
/*   int SR = int_off();   // disable IRQ and return CPSR */
/*   for each PROC *p do { */
/*       if (p->status == SLEEP && p->event == event) { */
/*         p->status = READY; */
/*         enqueue(&readyQueue, p); */
/*       } */
/*     } */
/*   int_on(SR);      // restore original CPSR */
/* } */

