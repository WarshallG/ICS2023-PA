#include<stdio.h>
#include<sys/time.h>
#include<unistd.h>
#include<NDL.h>

int main()
{
	uint32_t start_time, end_time;
	uint32_t interval = 500;
	int i=1;
	NDL_Init(0);
	
	
	start_time = NDL_GetTicks();
	
	while(1){
		
		end_time = NDL_GetTicks();
		
		
		while(end_time - start_time < interval){
			end_time = NDL_GetTicks();
		}	
		
		printf("Hello from timer, the %d times\n",i);
		i=(i+1)%100000;
		interval += 500;
	}
	
	return 0;

}  
