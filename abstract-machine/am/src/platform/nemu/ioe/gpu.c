#include <am.h>
#include <nemu.h>
#include <klib.h>



#include "../../../riscv/riscv.h"

#define SYNC_ADDR (VGACTL_ADDR + 4)


//#define W ((uint32_t)inl(VGACTL_ADDR)>>16)
//#define H (((uint32_t)inl(VGACTL_ADDR)<<16)>>16)
//Simplified
#define W 400
#define H 300

void __am_gpu_init() {
  //int i;
  //int w=W;
  //int h=H;
  //printf("%d, %d\n", w, h);
  //printf("%d\n",inl(VGACTL_ADDR));
  //uint32_t *fb=(uint32_t *)(uintptr_t)FB_ADDR;
  //for ( i = 0; i < w*h; i++)
  //{
    //fb[i]=i;
  //}
  //outl(SYNC_ADDR,1);
}

void __am_gpu_config(AM_GPU_CONFIG_T *cfg) {
  *cfg = (AM_GPU_CONFIG_T) {
    .present = true, .has_accel = false,
    .width = W, .height = H,
    .vmemsz = 0
  };
}

void __am_gpu_fbdraw(AM_GPU_FBDRAW_T *ctl) {
  //printf("x=%d y=%d w=%d h=%d\n",ctl->x,ctl->y,ctl->w,ctl->h);
  //uint32_t *palette=(uint32_t  *)(uintptr_t)ctl->pixels;
  uint32_t addr=FB_ADDR +(ctl->x<<2)+ctl->y*(W<<2);
  for(int i=0;i<ctl->h;i++){
  	for(int j=0;j<ctl->w;j++){
  		//outl(FB_ADDR+4*((ctl->y+i)*W+ctl->x+j),palette[i*(ctl->w)+j]);
  		outl(addr+i*(W<<2)+(j<<2),*(uint32_t *)(ctl->pixels+((i*ctl->w+j)<<2)));
  	}
  }
 
  
  if (ctl->sync) {
  	//printf("is_sync\n");
    outl(SYNC_ADDR, 1);
  }
}

void __am_gpu_status(AM_GPU_STATUS_T *status) {
  status->ready = true;
}
