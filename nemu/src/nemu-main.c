/***************************************************************************************
* Copyright (c) 2014-2022 Zihao Yu, Nanjing University
*
* NEMU is licensed under Mulan PSL v2.
* You can use this software according to the terms and conditions of the Mulan PSL v2.
* You may obtain a copy of Mulan PSL v2 at:
*          http://license.coscl.org.cn/MulanPSL2
*
* THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
* EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
* MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
*
* See the Mulan PSL v2 for more details.
***************************************************************************************/

#include <common.h>
#include "./monitor/sdb/sdb.h"

void init_monitor(int, char *[]);
void am_init_monitor();
void engine_start();
int is_exit_status_bad();

void test_expr();

int main(int argc, char *argv[]) {
  /* Initialize the monitor. */
#ifdef CONFIG_TARGET_AM
  am_init_monitor();
#else
  init_monitor(argc, argv);
#endif

  //test_expr();
  /* Start engine. */
  engine_start();

  return is_exit_status_bad();
}



#define FILE_PATH "./src/input"

void test_expr(){
	int cnt=0;
	char buf[65535];
	FILE *fp=fopen(FILE_PATH,"r");
	//printf("%d\n",fp==NULL);
	assert(fp!=NULL);
	
	char *input=fgets(buf,ARRLEN(buf),fp);
	while(input != NULL){
		uint32_t ans=0;  //the type of ans is uint32_t, which is meant to be consistent with the type of cmd_p function .
		input[strlen(input)-1]='\0'; //delete the '\n' at the end of the input string .
		char *tmp=strtok(input," ");
		sscanf(tmp,"%u",&ans);
		char *expression=input+strlen(tmp)+1;  //get the expression
		bool success=false;
		uint32_t my_ans=expr(expression,&success);  // call the function expr to get my_ans .
		printf("Have passed %d tests.\n",cnt);
		assert(my_ans==ans);  // check the answer 
		cnt++;
		input=fgets(buf,ARRLEN(buf),fp);  // get the next input .
	}
	
	return ;
}
