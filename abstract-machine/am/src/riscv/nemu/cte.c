#include <am.h>
#include <riscv/riscv.h>
#include <klib.h>
#include <arch/riscv.h>


static Context* (*user_handler)(Event, Context*) = NULL;

Context* __am_irq_handle(Context *c) {
  if (user_handler) {
    Event ev = {0};
    //printf("c->mcause: %d\n",c->mcause); //From this test, we know c->mcause is 11
    //printf("In cte.c : c->GPR1 = %d\n",c->GPR1);
    
    switch (c->mcause) {
      case 11: {
      	if(c->GPR1==-1){
      		ev.event = EVENT_YIELD;
      	}else{
      		ev.event = EVENT_SYSCALL;
      	}
      	break;
      
      }
      default: ev.event = EVENT_ERROR; break;
      //default : ev.event = EVENT_SYSCALL; break;
    }

    c = user_handler(ev, c);
    assert(c != NULL);
  }

  return c;
}

extern void __am_asm_trap(void);

bool cte_init(Context*(*handler)(Event, Context*)) {
  // initialize exception entry
  //printf("We are before csrw inst!\n");
  asm volatile("csrw mtvec, %0" : : "r"(__am_asm_trap));
  
  //printf("csrw has been implemented!\n");
  
  // register event handler
  user_handler = handler;

  return true;
}

Context *kcontext(Area kstack, void (*entry)(void *), void *arg) {
  return NULL;
}

void yield() {
  //printf("In function yield\n");
#ifdef __riscv_e
  //printf("define __riscv_e\n");
  asm volatile("li a5, -1; ecall");
#else
  //printf("not define __riscv_e\n");
  asm volatile("li a7, -1; ecall");
#endif
}

bool ienabled() {
  return false;
}

void iset(bool enable) {
}
