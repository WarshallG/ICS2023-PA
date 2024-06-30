#include <common.h>
#include <string.h>
#include <stdio.h>

#if defined(MULTIPROGRAM) && !defined(TIME_SHARING)
# define MULTIPROGRAM_YIELD() yield()
#else
# define MULTIPROGRAM_YIELD()
#endif

#define NAME(key) \
  [AM_KEY_##key] = #key,

static const char *keyname[256] __attribute__((used)) = {
  [AM_KEY_NONE] = "NONE",
  AM_KEYS(NAME)
};

size_t serial_write(const void *buf, size_t offset, size_t len) {
  char *str=(char *)buf;
  for(int i=0;i<len;i++){
  	putch(str[i]);
  }
  return len;
}

size_t events_read(void *buf, size_t offset, size_t len) {
  AM_INPUT_KEYBRD_T kbd = io_read(AM_INPUT_KEYBRD);
  if(kbd.keycode == AM_KEY_NONE) return 0;
  
  char str[1024];
  if(kbd.keydown){
  	strcpy(str,"kd ");
  }else{
  	strcpy(str,"ku ");
  }
  
  strcat(str,keyname[kbd.keycode]);
  size_t str_len = strlen(str);
  if(len < str_len){
  	printf("In [event.c][events_read]:  events_read error: need len of %d, but len is %d",str_len,len);
  	assert(0);
  }else{
  	strcpy(buf,str);
  }
  //printf("kbd is read successfully!\n");
  return str_len;
}

char* int_to_str(char *str,int d) {
	if(d==0){
		str[0]='0';
		str[1]='\0';
		return str;
	}
	int tmp = d;
	int digit = 0;
	while (tmp > 0) {
		digit++;
		tmp /= 10;
 	}
    for (int i = digit - 1; i >= 0; i--) {
        str[i] = (d % 10) + '0';
        d /= 10;
    }
    str[digit] = '\0';
    return str;
}

size_t dispinfo_read(void *buf, size_t offset, size_t len) {

  int w = io_read(AM_GPU_CONFIG).width;
  int h = io_read(AM_GPU_CONFIG).height;
  
  char str1[64],str2[64];
  int_to_str(str1,w);
  int_to_str(str2,h);
  
  char tmp[128];
  
  strcpy(tmp,"WIDTH:");  strcat(tmp,str1); 
  strcat(tmp,"\n");
  strcat(tmp,"HEIGHT:"); strcat(tmp,str2);

  int str_len=strlen(tmp);
  
  if(len>str_len){
  	len=str_len;
  }
  
  //printf("str_len = %d, len = %d\n",str_len,len);
  
  strncpy(buf, tmp, len+1);  //注意要+1
  
  //printf("buf is %s\n",(char *)buf);
  return len+1;
}

#define MMIO_BASE 0xa0000000
#define FB_ADDR   (MMIO_BASE   + 0x1000000)

static void fb_copy(uint32_t dest, void *buf, int size){
  uint32_t *src = buf, *dst = (uint32_t *)(FB_ADDR + dest);
  for (int i = 0; i < size >> 2; i++, src++, dst++){
    *dst = *src;
  }
  //char *c_src = (char *)buf, *c_dst = (char *)dst;
  //for (int i = 0; i < (size & 3); i++){
    //c_dst[i] = c_src[i];
  //}
}

size_t fb_write(const void *buf, size_t offset, size_t len) {
  uintptr_t *ptr;
  ptr = (uintptr_t *)(&buf);

  fb_copy(offset, (void *)*ptr, len);
  io_write(AM_GPU_FBDRAW, 0, 0, NULL, 0, 0, true);
  
  return len;
}

void init_device() {
  Log("Initializing devices...");
  ioe_init();
}
