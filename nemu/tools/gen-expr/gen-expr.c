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

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <assert.h>
#include <string.h>

// this should be enough
static char buf[65536] = {};
static char code_buf[65536 + 128] = {}; // a little larger than `buf`
static char *code_format =
"#include <stdio.h>\n"
"int main() { "
"  unsigned result = %s; "
"  printf(\"%%u\", result); "
"  return 0; "
"}";

uint32_t choose(uint32_t n){
	return rand()%n;
}

void gen(char c){
	char tmp_buf[2]={c,'\0'};
	strcat(buf,tmp_buf);
	return ;
}

static void gen_blank(){
	switch (choose(3)){
		case 0: break;
		case 1: strcat(buf, " "); break;
		case 2: strcat(buf, "  "); break;
		 
	}
}

void gen_num(){
	int num=rand()%100+1;
	char tmp[102];
	tmp[0]='\0';
	sprintf(tmp,"%d",num);
	strcat(buf,tmp);
	return ; 
}

void gen_rand_op(){
	switch (choose (4)){
		case 0: gen('+'); break;
		case 1: gen('-'); break;
		case 2: gen('*'); break;
		case 3: gen('/'); break;
	}
	return ;
}

static void gen_rand_expr(int depth) {
	//buf[0] = '\0';
	if(strlen(buf)>65536-65500 || depth > 10){
		gen('(');
		gen_blank();
		gen_num();
		gen_blank();
		gen(')');
		return ;
	} 
	
	switch (choose(3)){
		case 0: gen_num(); gen_blank();break;
		case 1: gen('('); gen_blank(); gen_rand_expr(depth+1); gen_blank(); gen(')'); break;
		default : gen_rand_expr(depth+1); gen_blank(); gen_rand_op(); gen_blank(); gen_rand_expr(depth+1); break;
	}
}

int main(int argc, char *argv[]) {
  int seed = time(0);
  srand(seed);
  int loop = 1;
  if (argc > 1) {
    sscanf(argv[1], "%d", &loop);
  }
  int i;
  for (i = 0; i < loop; i ++) {
    buf[0]='\0';
    gen_rand_expr(1);

    sprintf(code_buf, code_format, buf);

    FILE *fp = fopen("/tmp/.code.c", "w");
    assert(fp != NULL);
    fputs(code_buf, fp);
    fclose(fp);

    int ret = system("gcc /tmp/.code.c -o /tmp/.expr  -Werror 2> /tmp/.error.txt");
    if (ret != 0) {
    	i--;continue;
    }

    fp = popen("/tmp/.expr", "r");
    assert(fp != NULL);

    //int result;
    //ret = fscanf(fp, "%d", &result);
    //pclose(fp);
    
    uint32_t result = 0u;
    int b = fscanf(fp, "%u", &result);
    pclose(fp);
    if (b != 1){
      i--;
      continue;
    }

    printf("%u %s\n", result, buf);
  }
  return 0;
}
