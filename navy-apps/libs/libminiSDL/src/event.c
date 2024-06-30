#include <NDL.h>
#include <SDL.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>

#define keyname(k) #k,

static const char *keyname[] = {
  "NONE",
  _KEYS(keyname)
};

#define keyname_size (sizeof(keyname) / sizeof(char *))

static uint8_t key_state[keyname_size] = {0};

int SDL_PushEvent(SDL_Event *ev) {
  assert(0);
  return 0;
}

int SDL_PollEvent(SDL_Event *event) {
  char buf[64];
  if(!NDL_PollEvent(buf,sizeof(buf))){
  	return 0;
  }
  
  char tag[8], key[32];
  
  sscanf(buf,"%s %s", tag, key);
  
  if(strcmp("kd",tag) == 0){
  	event->type = SDL_KEYDOWN;
  }else if (strcmp("ku",tag) == 0){
  	event->type = SDL_KEYUP;
  }else{
  	printf("In [event.c][SDL_WaitEvent]: Invalid tag!\n");
  	assert(0);
  }
  
  //printf("%s,%s\n",tag,key);
  
  for(int i=0; i<keyname_size; i++){
  	if(strcmp(keyname[i],key) == 0){
  		event->key.keysym.sym = i;
  		break;
  	}
  }
  
  switch(event->type){
  	case SDL_KEYDOWN:
  		key_state[event->key.keysym.sym] = 1;
  		break;
  	case SDL_KEYUP:
  		key_state[event->key.keysym.sym] = 0;
  		break;
  }
  
  return 1;
}

int SDL_WaitEvent(SDL_Event *event) {
  char buf[64];
  while(!NDL_PollEvent(buf,sizeof(buf))){}
  //printf("%s\n",buf);
  char tag[8], key[32];
  
  sscanf(buf,"%s %s", tag, key);
  
  if(strcmp("kd",tag) == 0){
  	event->type = SDL_KEYDOWN;
  }else if (strcmp("ku",tag) == 0){
  	event->type = SDL_KEYUP;
  }else{
  	printf("In [event.c][SDL_WaitEvent]: Invalid tag!\n");
  	assert(0);
  }
  
  //printf("%s,%s\n",tag,key);
  
  for(int i=0; i<keyname_size; i++){
  	if(strcmp(keyname[i],key) == 0){
  		event->key.keysym.sym = i;
  		break;
  	}
  }
  
    switch(event->type){
  	case SDL_KEYDOWN:
  		key_state[event->key.keysym.sym] = 1;
  		break;
  	case SDL_KEYUP:
  		key_state[event->key.keysym.sym] = 0;
  		break;
  }
  
  return 1;
}

int SDL_PeepEvents(SDL_Event *ev, int numevents, int action, uint32_t mask) {
  return 0;
}

uint8_t* SDL_GetKeyState(int *numkeys) {
  SDL_Event ev;

  if (numkeys)
    *numkeys = keyname_size;

  return key_state;
}
