#include <am.h>
#include <klib.h>
#include <klib-macros.h>
#include <stdarg.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

char* int2str(char *str,int d,int digit) {
    for (int i = digit - 1; i >= 0; i--) {
        str[i] = (d % 10) + '0';
        d /= 10;
    }
    str[digit] = '\0';
    return str;
}

int printf(const char *fmt, ...) {
  //panic("Not implemented");
  char buf[10000];
  va_list ap;
  va_start(ap,fmt);
  
  int ret=vsprintf(buf,fmt,ap);
  
  char *read=buf;
  while(*read!='\0'){
  	putch(*read);
  	read++;
  }
  
  va_end(ap);
  return ret;
}

int vsprintf(char *out, const char *fmt, va_list ap) {
  //panic("Not implemented");
  int count=0;
  while(*fmt!='\0'){
  	if(*fmt!='%')  //a normal char , put it into out list and continue.
  	{
  		*out = * fmt;
  		out++; fmt++;
  		count++;
  		continue;
  	}
  	//Is '%'
  	fmt++;
  	switch(*fmt){
  		case 's':
  		{
  			char *s=va_arg(ap,char *);  //read the string in args
  			while(*s!='\0'){
  				*out=*s;
  				s++; out++;
  				count++;
  			}
  			fmt++;
  			break;
  		} 
  		
  		case 'd':
  		{
  			int d=0;
  			d=va_arg(ap,int);
  			int digit = 0;
    		int tmp = d;
    		if(d==0){
    			*out='0';
    			out++;count++;fmt++;
    			break;
    		}
    		while (tmp > 0) {
        		digit++;
        		tmp /= 10;
   		 	}
   		 	char str[digit+2];
  			int2str(str,d,digit);
  			char *head=str;
  			while(*head!='\0'){
  				*out=*head;
  				head++; out++;
  				count++;
  			}
  			fmt++;
  			break;
  		}
  		
  		case 'c':{
  			char c=(char)va_arg(ap,int);
  			*out=c;
  			out++; fmt++;count++;
  			break;
  		}
  		
  		case 'x':{
  			uint32_t n = va_arg(ap,uint32_t);
  			if(n==0){
  				*out='0';
  				out++;fmt++;count++; break;
  			}
  			char hex_num[10];
  			int length=0;
  			uint32_t tmp=n;
  			while(tmp){
  				length++;tmp>>=4;
  			}          // To find the length of n in hex.
  			assert(length>=0 && length<=8); //check the bound of length.
  			for(int i=length-1;n;n>>=4,i--)
  			{
  				char ch;
  				if(n%16<10){ch=n%16+'0';}
  				else{ch=n%16-10+'A';}
  				hex_num[i]=ch;
  			}
  			hex_num[length]='\0';
  			char *s=hex_num;
  			while(*s!='\0'){
  				*out=*s;
  				s++; out++;
  				count++;
  			}
  			fmt++;
  			break;
  		}
  		
  		case 'p':{
  			uint32_t ptr=(uint32_t)va_arg(ap,uintptr_t);
  			int length=0;
  			uint32_t tmp=ptr;
  			while(tmp){
  				length++;tmp>>=4;
  			}
  			char buf_ptr[10];
  			//01234567	
  			for(int i=0;i<8-length;i++)
  				buf_ptr[i]='0';
  			for(int i=7;ptr;ptr>>=4,i--){
				char ch; uint32_t low=ptr%16;
  				if(low<10){ch=low+'0';}
  				else{ch=low-10+'A';}
  				buf_ptr[i]=ch;
  			}
  			buf_ptr[8]='\0';
  			char *s=buf_ptr;
  			while(*s!='\0'){
  				*out=*s;
  				s++; out++;
  				count++;
  			}
  			fmt++;
  			break;
  			
  				
  		}
  		default:{
  			*out=*fmt;
  			out++; fmt++;
  			//assert(0);
  			break;
  		}
  	}
  }
  va_end(ap);
  *out='\0';
  return count;
}

int sprintf(char *out, const char *fmt, ...) {
  //panic("Not implemented");
  va_list ap;
  va_start(ap,fmt);
  
  int ret=vsprintf(out,fmt,ap);
  va_end(ap);
  return ret;
  
}

int snprintf(char *out, size_t n, const char *fmt, ...) {
  panic("Not implemented");
}

int vsnprintf(char *out, size_t n, const char *fmt, va_list ap) {
  panic("Not implemented");
}

#endif
