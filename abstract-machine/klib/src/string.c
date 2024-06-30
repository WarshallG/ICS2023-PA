#include <klib.h>
#include <klib-macros.h>
#include <stdint.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

size_t strlen(const char *s) {
  //panic("Not implemented");
  size_t len=0;
  while(*s!='\0'){
  	s++;
  	len++;
  }
  return len;
}

char *strcpy(char *dst, const char *src) {
  //panic("Not implemented");
  char *ans=dst;
  while(*src!='\0'){
  	*dst=*src;
  	dst++;src++;
  }
  *dst='\0';
  return ans;
}

char *strncpy(char *dst, const char *src, size_t n) {
  //panic("Not implemented");
  int i;
  for(i=0; i<n && src[i]!='\0'; i++)
  	dst[i]=src[i];
  for(; i<n; i++)
  	dst[i]='\0';
  return dst;
  
}

char *strcat(char *dst, const char *src) {
  //panic("Not implemented");
  size_t dst_len=strlen(dst);
  size_t i=0;
  while(src[i]!='\0'){
  	dst[dst_len+i]=src[i];
  	i++;
  }
  dst[dst_len+i]='\0';
  return dst;
}

int strcmp(const char *s1, const char *s2) {
  //panic("Not implemented");
  size_t i;
  for (i=0; s1[i]!='\0' && s2[i]!='\0'; i++)
  {
  	if((int)(s1[i])<(int)(s2[i]))
  		return -1;
  	else if ((int)(s1[i])>(int)(s2[i]))
  		return 1;
  }
  return (int)(s1[i])-(int)(s2[i]);
  
}

int strncmp(const char *s1, const char *s2, size_t n) {
  //panic("Not implemented");
  size_t i;
  for(i=0;i<n && s1[i]!='\0' && s2[i]!='\0';i++){
  	if ((int)(s1[i])<(int)(s2[i]))
  		return -1;
  	else if ((int)(s1[i])>(int)(s2[i]))
  		return 1;
  }
  if(i==n)         //If i reaches n.
  	return 0;
  return (int)(s1[i])-(int)(s2[i]);
}

void *memset(void *s, int c, size_t n) {
  //panic("Not implemented");
  unsigned char* ans=(unsigned char *)s;
  for (int i=0;i<n;i++)
  {
  	ans[i]=(unsigned char)c;
  }
  return s;
}

void *memmove(void *dst, const void *src, size_t n) {
  //panic("Not implemented");
  char *tmp_dst=(char *)dst;
  char *tmp_src=(char *)src;
  if(dst<src){  //The address of dst is smaller than src
  	for (int i=0;i<n;i++){
  		tmp_dst[i]=tmp_src[i];
  	}
  }else if(dst>src){
  	for(int i=n-1; i>=0;i--){
  		tmp_dst[i]=tmp_src[i];
  	}
  }
  
  return dst;
  
}

void *memcpy(void *out, const void *in, size_t n) {
  //panic("Not implemented");
  assert(out!=NULL);
  assert(in!=NULL);
  
  char *tmp_out=(char *)out;
  char *tmp_in =(char *)in;
  for (int i=0;i<n;i++)
  {
	tmp_out[i]=tmp_in[i];
  }
  return out;
}

int memcmp(const void *s1, const void *s2, size_t n) {
  //panic("Not implemented");
  char *tmp_s1=(char *)s1;
  char *tmp_s2=(char *)s2;
  size_t i;
  for(i=0;i<n;i++)
  {
  	if(tmp_s1[i]<tmp_s2[i])
  		return -1;
  	if(tmp_s1[i]>tmp_s2[i])
  		return 1;
  }
  if(i==n)
  	return 0;
  return (int)(tmp_s1[i])-(int)(tmp_s2[i]);
}

#endif
