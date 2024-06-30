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

#include <isa.h>
#include <cpu/cpu.h>
#include <readline/readline.h>
#include <readline/history.h>
#include "sdb.h"
#include "memory/vaddr.h"
//#include "utils.h"

static int is_batch_mode = false;

void init_regex();
void init_wp_pool();
static void print_lack_of_args_info(); //print some infomation of the error

/* We use the `readline' library to provide more flexibility to read from stdin. */
static char* rl_gets() {
  static char *line_read = NULL;

  if (line_read) {
    free(line_read);
    line_read = NULL;
  }

  line_read = readline("(nemu) ");

  if (line_read && *line_read) {
    add_history(line_read);
  }

  return line_read;
}

static void print_lack_of_args_info(){
  printf("The function is lack of args.\n");
  return ;
}

static int cmd_c(char *args) {
  cpu_exec(-1);
  return 0;
}


static int cmd_q(char *args) {
  nemu_state.state=NEMU_QUIT;
  return -1;
}

static int cmd_si(char *args){
  uint64_t n=1;
  if(args !=NULL){
    sscanf(args,"%ld",&n);
  }
  cpu_exec(n);
  return 0;
}

//Print program status
static int cmd_info(char *args){
  if(args==NULL){
  	print_lack_of_args_info();
  	return 0;
  }
  else if (strcmp("r",args)==0){
	isa_reg_display();
	return 0;
  }
  else if (strcmp("w",args)==0){
  	return 0;
  }
  else{
  	//printf(ANSI_FMT("Undefined operation\n",ANSI_RED));
  	//set the color of the error information red.
  	//printf("\x1B[31m%s is an undefined operation\x1B[0m\n",args);
  	printf(ANSI_FMT("%s is an undefined operation\n",ANSI_FG_RED),args);
  }
  return 0;
}

#define REGISTER_LEN 4
static int cmd_x(char *args){
	if(args==NULL){
		print_lack_of_args_info();
		return 0;
	}
	char *arg0=strtok(NULL," ");
	int n=atoi(arg0);
	//printf("%d\n",n);
	if(args==NULL){
		print_lack_of_args_info();
		return 0;
	}
	char *arg1=strtok(NULL," ");
	//printf("%s\n",arg1);
	char *mem_addr=strtok(arg1,"x");
	if(arg1==NULL){
		print_lack_of_args_info();
	}
	mem_addr=strtok(NULL,"x");
	//printf("%s\n",mem_addr);
	vaddr_t addr=0;
	sscanf(mem_addr,"%08x",&addr);
	
	for(int i=1;i<=n;i++)
	{
		printf(ANSI_FMT("REGISTER 0x%08x :",ANSI_FG_RED),addr);
		printf(" 0x%08x\n",vaddr_read(addr,REGISTER_LEN));
		addr+=REGISTER_LEN;
	}

	return 0;
}

static int cmd_p(char *args){
	bool success;
	word_t ans=expr(args,&success);
	//printf("ok\n");
	if(!success){
		printf(ANSI_FMT("make_token failed!\n",ANSI_FG_RED));
	}
	else{
		printf("The value of ");
		printf(ANSI_FMT("%s",ANSI_FG_YELLOW),args);
		printf(" is: ");
		printf(ANSI_FMT("%d\n",ANSI_FG_YELLOW),ans);
		
	}
	return 0;
}

static int cmd_help(char *args);

static struct {
  const char *name;
  const char *description;
  int (*handler) (char *);
} cmd_table [] = {
  { "help", "Display information about all supported commands", cmd_help },
  { "c", "Continue the execution of the program", cmd_c },
  { "q", "Exit NEMU", cmd_q },
  { "si","Step through N instructions",cmd_si},
  { "info","Print register status or print watching point info",cmd_info},
  { "x","Scan Memory",cmd_x},
  { "p","eval the expression",cmd_p},

  /* TODO: Add more commands */

};

#define NR_CMD ARRLEN(cmd_table)

static int cmd_help(char *args) {
  /* extract the first argument */
  char *arg = strtok(NULL, " ");
  //printf("%s\n",arg);
  int i;

  if (arg == NULL) {
    /* no argument given */
    for (i = 0; i < NR_CMD; i ++) {
      printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
    }
  }
  else {
    for (i = 0; i < NR_CMD; i ++) {
      if (strcmp(arg, cmd_table[i].name) == 0) {
        printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
        return 0;
      }
    }
    printf("Unknown command '%s'\n", arg);
  }
  return 0;
}

void sdb_set_batch_mode() {
  is_batch_mode = true;
}

void sdb_mainloop() {
#ifdef CONFIG_BATCH_MODE  
  sdb_set_batch_mode();
#endif
  if (is_batch_mode) {
  	//printf("This is batch mode!\n");
    cmd_c(NULL);
    return;
  }

  for (char *str; (str = rl_gets()) != NULL; ) {
    char *str_end = str + strlen(str);

    /* extract the first token as the command */
    char *cmd = strtok(str, " ");
    if (cmd == NULL) { continue; }

    /* treat the remaining string as the arguments,
     * which may need further parsing
     */
    char *args = cmd + strlen(cmd) + 1;
    if (args >= str_end) {
      args = NULL;
    }

#ifdef CONFIG_DEVICE
    extern void sdl_clear_event_queue();
    sdl_clear_event_queue();
#endif

    int i;
    for (i = 0; i < NR_CMD; i ++) {
      if (strcmp(cmd, cmd_table[i].name) == 0) {
        if (cmd_table[i].handler(args) < 0) { return; }
        break;
      }
    }

    if (i == NR_CMD) { printf("Unknown command '%s'\n", cmd); }
  }
}

void init_sdb() {
  /* Compile the regular expressions. */
  init_regex();

  /* Initialize the watchpoint pool. */
  init_wp_pool();
}
