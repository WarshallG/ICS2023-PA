#include <common.h>
#include "syscall.h"
#include <sys/time.h>
#include "fs.h"
#include <proc.h>



size_t ramdisk_write(const void *buf, size_t offset, size_t len);
extern void naive_uload(PCB *pcb, const char *filename);

void sys_yield(Context *c){
  yield();
  c->GPRx = 0;
}

void sys_exit(Context *c){
  naive_uload(NULL, "/bin/nterm");
  c->GPRx = 0;
}

void sys_write(Context *c){
	//printf("sys_write\n ");

//    if (c->GPR2 == 1 || c->GPR2 == 2){
//      for (int i = 0; i < c->GPR4; i++){
//  		putch(*(((char *)c->GPR3) + i));
//      }
//      c->GPRx = c->GPR4;
//    }else{
        int ret=fs_write(c->GPR2,(void *)c->GPR3,c->GPR4);
    	c->GPRx=ret;
//    }

}

void sys_brk(Context *c){
	c->GPRx = 0;  //Always can be allocated.
}

void sys_gettimeofday(Context *c){
	struct timeval *tv = (struct timeval *)c->GPR2;  //To get the first arg from context.
	uint64_t time = io_read(AM_TIMER_UPTIME).us;
	
	//注意time返回的是微秒
	tv->tv_sec = (time / 1000000);
	tv->tv_usec = (time % 1000000);
	c->GPRx=0;
}

void sys_open(Context *c){
	//printf("this is sys_open!\n");
	int ret = fs_open((char *)c->GPR2,c->GPR3,c->GPR4);
	c->GPRx = ret;
}

void sys_close(Context *c){
	int ret = fs_close(c->GPR2);
	c->GPRx=ret;
}

void sys_read(Context *c){
	int ret = fs_read(c->GPR2,(void *)c->GPR3,c->GPR4);
	c->GPRx = ret;
}

void sys_lseek(Context *c){
	int ret = fs_lseek(c->GPR2, c->GPR3, c->GPR4);
	c->GPRx = ret;
}

void sys_execve(Context *c){
  const char *fname = (const char *)c->GPR2;
  naive_uload(NULL, fname);
  c->GPRx = 0;
}


inline static void strace(Context* c){
	Log("Strace:\n");
	Log("c->mcause = %d\n",c->mcause);
	Log("c->GPR1 = %d\n",c->GPR1);
	Log("c->GPR2 = %d\n",c->GPR2);
	Log("c->GPR3 = %d\n",c->GPR3);
	Log("c->GPR4 = %d\n",c->GPR4);
	Log("c->GPRx = %d\n",c->GPRx);
	//printf("strace\n");
}


void do_syscall(Context *c) {
  uintptr_t a[4];
  a[0] = c->GPR1;
  //printf("a[0] is : %d\n",a[0]);
  
  
#ifdef STRACE 
	strace(c);
#endif
  

  switch (a[0]) {
  
  	case SYS_yield:
  		sys_yield(c);
  		break;
  		
  	case SYS_exit:
  		sys_exit(c);
  		break;
  		
  	case SYS_write:
  		//printf("Here is SYS_write!\n");
 		sys_write(c);
 		break;
 		
 	case SYS_brk:
 		//printf("Here is SYS_brk!\n");
 		sys_brk(c);
 		break;
 	
 	case SYS_gettimeofday:
 		sys_gettimeofday(c);
 		break;
 	
 	case SYS_open:
 		sys_open(c);
 		break;
 	
 	case SYS_close:
 		sys_close(c);
 		break;
 		
 	case SYS_read:
 		sys_read(c);
 		break;
 	
 	case SYS_lseek:
 		sys_lseek(c);
 		break;

 	case SYS_execve:
      sys_execve(c);
      break;
 		
    default: panic("Unhandled syscall ID = %d", a[0]);
  }
}
