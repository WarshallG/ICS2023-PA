#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>
#include <fcntl.h>
#include <assert.h> 

static int evtdev = -1;
static int fbdev = -1;
static int screen_w = 0, screen_h = 0;

uint32_t NDL_GetTicks() {
  struct timeval tv;
  gettimeofday(&tv,NULL);
  
  uint32_t milli_sec=tv.tv_sec*1000 + tv.tv_usec/1000; //返回毫秒的时间
  
  return milli_sec;
}

int NDL_PollEvent(char *buf, int len) {
  int fp = open("/dev/events",O_RDONLY);
  int ret = read(fp, buf, sizeof(char)*len);
  //printf("ret= %d\n",ret);
  return ret;
}

static int canvas_w, canvas_h,canvas_x = 0, canvas_y = 0;

void NDL_OpenCanvas(int *w, int *h) {
  //printf("in opencanvas\n");
  if (*w == 0 && *h == 0) {
    *w = screen_w;
    *h = screen_h;
  }

  canvas_w = *w;
  canvas_h = *h;

  canvas_x = (screen_w - canvas_w) / 2;
  canvas_y = (screen_h - canvas_h) / 2;

  if (getenv("NWM_APP")) {
    int fbctl = 4;
    fbdev = 5;
    screen_w = *w; screen_h = *h;
    char buf[64];
    int len = sprintf(buf, "%d %d", screen_w, screen_h);
    // let NWM resize the window and create the frame buffer
    write(fbctl, buf, len);
    while (1) {
      // 3 = evtdev
      int nread = read(3, buf, sizeof(buf) - 1);
      if (nread <= 0) continue;
      buf[nread] = '\0';
      if (strcmp(buf, "mmap ok") == 0) break;
    }
    close(fbctl);
  }
}



void NDL_DrawRect(uint32_t *pixels, int x, int y, int w, int h) {
  int fp = open("/dev/fb", O_RDWR);
  
  for (int i = 0; i < h && i + y < canvas_h; i++) {
    int offset = (canvas_y + y + i) * screen_w + (canvas_x + x);
    lseek(fp, offset<<2, SEEK_SET);
    write(fp, pixels + i * w, w<<2);
  }
}

void NDL_OpenAudio(int freq, int channels, int samples) {
}

void NDL_CloseAudio() {
}

int NDL_PlayAudio(void *buf, int len) {
  return 0;
}

int NDL_QueryAudio() {
  return 0;
}

static void extract_info(char *str, char *key, int* value){
  char buffer[258];
  int len = 0;
  for (char* c = str; *c; ++c){
    if(*c != ' '){
      buffer[len++] = *c;
    }
  }
  buffer[len] = '\0';

  sscanf(buffer, "%[a-zA-Z]:%d", key, value);
}

int NDL_Init(uint32_t flags) {
  //printf("Hello from NDL_Init!\n");
  if (getenv("NWM_APP")) {
    evtdev = 3;
  }
  
  char buf[128],tag[32];
  int size;
  
  int fp = open("/proc/dispinfo",0);
  read(fp,buf,sizeof(buf));
  close(fp);
  
  char *token = strtok(buf,"\n");
  
  while(token != NULL){
  	
  	//printf("in while !\n");
  	extract_info(token, tag, &size);
  	
  	if(strcmp(tag,"WIDTH")==0){
  		screen_w = size;
  	}else if(strcmp(tag, "HEIGHT")==0){
  		screen_h = size;
  	}else{
  		printf("Invalid content!\n");
  		assert(0);
  		break;
  	}
  	
  	token = strtok(NULL, "\n");
  	
  }
  
  //printf("In function NDL_Init: w= %d, h= %d\n",screen_w,screen_h);
  
  return 0;
}

void NDL_Quit() {
}
