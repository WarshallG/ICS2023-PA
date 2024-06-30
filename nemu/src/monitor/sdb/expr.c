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

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <regex.h>
#include <string.h>
#include "memory/vaddr.h"

enum {
  TK_NOTYPE = 256, TK_EQ,
  TK_DEC_INT,TK_HEX,TK_REG,
  TK_NOTEQ,TK_AND,TK_NEGTIVE,
  TK_DEREF,
};    /* Add more token types */

static struct rule {
  const char *regex;
  int token_type;
} rules[] = {
  {"0x[0-9,a-f,A-F]+",TK_HEX},//hex
  {" +", TK_NOTYPE},    // spaces
  {"==", TK_EQ},        // equal
  //{"\\$[0-9a-z]+",TK_REG},//register
  {"\\$(\\$0|ra|sp|gp|tp|t0|t1|t2|s0|s1|a0|a1|a2|a3|a4|a5|a6|a7|s2|s3|s4|s5|s6|s7|s8|s9|s10|s11|t3|t4|t5|t6|pc)",TK_REG},   //register
  {"\\(", '('},         // left parenthesis 
  {"\\)", ')'},		// right parenthesis 
  {"\\+", '+'},         // plus
  {"\\-", '-'}, 	// as you know...
  {"\\*", '*'},
  {"\\/", '/'},
  {"!=",TK_NOTEQ},
  {"&&",TK_AND},
  {"[0-9]+",TK_DEC_INT}, // decimal integer
};

#define NR_REGEX ARRLEN(rules)

static regex_t re[NR_REGEX] = {};

/* Rules are used for many times.
 * Therefore we compile them only once before any usage.
 */
void init_regex() {
  int i;
  char error_msg[128];
  int ret;

  for (i = 0; i < NR_REGEX; i ++) {
    ret = regcomp(&re[i], rules[i].regex, REG_EXTENDED);
    if (ret != 0) {
      regerror(ret, &re[i], error_msg, 128);
      panic("regex compilation failed: %s\n%s", error_msg, rules[i].regex);
    }
  }
}

typedef struct token {
  int type;
  char str[32];
} Token;

static Token tokens[1000] __attribute__((used)) = {};
static int nr_token __attribute__((used))  = 0;  //指示已经被识别出的token数目

static bool make_token(char *e) {
  int position = 0;
  int i;
  regmatch_t pmatch;

  nr_token = 0;

  while (e[position] != '\0') {
    /* Try all rules one by one. */
    for (i = 0; i < NR_REGEX; i ++) {
      if (regexec(&re[i], e + position, 1, &pmatch, 0) == 0 && pmatch.rm_so == 0) {
        char *substr_start = e + position;
        int substr_len = pmatch.rm_eo;

        Log("match rules[%d] = \"%s\" at position %d with len %d: %.*s",
            i, rules[i].regex, position, substr_len, substr_len, substr_start);

        position += substr_len;

        /* TODO: Now a new token is recognized with rules[i]. Add codes
         * to record the token in the array `tokens'. For certain types
         * of tokens, some extra actions should be performed.
         */

        switch (rules[i].token_type) {
          case TK_NOTYPE:break;
          case TK_HEX:
                tokens[nr_token].type=TK_HEX;
                memcpy(tokens[nr_token].str, e + position - substr_len+2, (substr_len-2) * sizeof(char));
                tokens[nr_token].str[substr_len-2]='\0';
          	nr_token++;
          	break;
          case TK_REG:
          	tokens[nr_token].type=TK_REG;
          	memcpy(tokens[nr_token].str, e + position - substr_len +1,(substr_len-1)*sizeof(char));
          	tokens[nr_token].str[substr_len-1]='\0';
          	nr_token++;
          	break;
          	
          case TK_EQ:
          	tokens[nr_token].type=TK_EQ;
          	tokens[nr_token].str[0]='='; tokens[nr_token].str[1]='=';
          	nr_token++;
          	break;
          case TK_NOTEQ:
          	tokens[nr_token].type=TK_NOTEQ;
          	tokens[nr_token].str[0]='!'; tokens[nr_token].str[1]='=';
          	nr_token++;
          	break;
          case TK_AND:
           	tokens[nr_token].type=TK_AND;
           	tokens[nr_token].str[0]='&'; tokens[nr_token].str[1]='&';
           	nr_token++;
           	break;
          case TK_DEC_INT:
          	tokens[nr_token].type=TK_DEC_INT;    //set the type
          	memcpy(tokens[nr_token].str, e + position - substr_len, (substr_len) * sizeof(char));//set the str
          	tokens[nr_token].str[substr_len]='\0';
          	nr_token++;
          	break;
          	
          default:
          	tokens[nr_token].type=rules[i].token_type;
          	tokens[nr_token].str[0]=(char)rules[i].token_type;
          	nr_token++;
          	break;
        }

        break;
      }
    }

    if (i == NR_REGEX) {
      printf("no match at position %d\n%s\n%*.s^\n", position, e, position, "");
      return false;
    }
  }

  return true;
}


static word_t eval(int p,int q);
static bool check_parentheses(int p,int q);
static int  find_main_op(int p,int q);
static bool certain_type(int type);


word_t expr(char *e, bool *success) {
  if (!make_token(e)) {
    *success = false;
    return 0;
  }

  /* TODO: Insert codes to evaluate the expression. */
  //TODO();
  
  //rejudge the type of each token.
  for(int i=0;i<nr_token;i++){
    if(tokens[i].type=='*' && (i==0 || certain_type(tokens[i-1].type))){
    	tokens[i].type=TK_DEREF;
    }
    else if(tokens[i].type=='-' && (i==0 || certain_type(tokens[i-1].type))){
    	tokens[i].type=TK_NEGTIVE;
    }
  } 
  
  printf(ANSI_FMT("make tokens successfully\n",ANSI_FG_RED));
  *success=true;
  word_t ans=eval(0,nr_token-1);
  if(!*success){
    printf(ANSI_FMT("errors in eval function\n",ANSI_FG_RED));
    return 0;
  }

  return ans;
}



static word_t eval(int p,int q){
  if (p>q){
    return 0;
  } 
  else if(p==q){
    if(tokens[p].type!=TK_DEC_INT && tokens[p].type!=TK_HEX &&tokens[p].type!=TK_REG){
      printf(ANSI_FMT("The single expression is invalid\n",ANSI_FG_RED));
      return 0;
    } 
    else if(tokens[p].type==TK_DEC_INT){	  //the single token is decimal int.
      word_t ans=0;
      sscanf(tokens[p].str,"%d",&ans);
      return ans; 
    }
    else if(tokens[p].type==TK_HEX){		 //the single token is hex.
      word_t ans=0;
      sscanf(tokens[p].str,"%x",&ans);
      return ans;
    }
    else if(tokens[p].type==TK_REG){ 		 //the single token is a register.	
      bool success=false;
      return isa_reg_str2val(tokens[p].str,&success);  	//get the value of the register.
    }
   }
   else if(check_parentheses(p,q)){
      return eval(p+1,q-1);  			//remove the parentheses outside.   
   }
   else{
     int main_op=0;
     main_op=find_main_op(p,q); 
     //printf("main_op pos is: %d , and the token is :%d\n",main_op,tokens[main_op].type);
     if(tokens[main_op].type==TK_DEREF){  
     	word_t val=eval(p+1,q);
     	return vaddr_read(val,4);
     }
     else if(tokens[main_op].type==TK_NEGTIVE){
        int val=eval(p+1,q);
        return -val;
     }
     else{
       word_t val1=eval(p,main_op-1);
       word_t val2=eval(main_op+1,q);
       switch (tokens[main_op].type){
         case '+': return val1+val2; break;
         case '-': return val1-val2; break;
         case '*': return val1*val2; break;
         case '/':

	   return val1/val2; break;
	 case TK_EQ: return val1==val2; break;
	 case TK_AND: return val1 && val2; break;
	 case TK_NOTEQ: return val1 != val2; break;
	 default: assert(0);
       }
     }
     
   }
   return 0;
}


#define MY_STACK_SIZE 10000
static bool check_stack_overflow(int top){
	if(top>=MY_STACK_SIZE){
		printf(ANSI_FMT("STACK OVERFLOW ERROR!\n",ANSI_FG_RED));
		return true;
	}
	else {return false;}
}


static bool is_match(int p,int q)   //Determine if parentheses match
{
  char stack[MY_STACK_SIZE];
  int pointer =p;  //the pointer,point at the expression
  int top=0;   //the top of the stack
  stack[top]=tokens[p].type;
  while(pointer<=q){
  	if (tokens[pointer].type=='('){
  		stack[top]='(';
  		if (check_stack_overflow(top)){return false;}
  		top++;
  	}
  	else if(tokens[pointer].type==')'){
  		if(top<=0||stack[top-1]!='('){
  			return false;
  		}
  		else{top--;}
  	}
  	pointer++;
  }
  return true;
}

static bool check_parentheses(int p,int q){
  if(!is_match(p+1,q-1)){

    return false;
  }
  else if (tokens[p].type=='(' && tokens[q].type==')') //If the parentheses on the both end exist and match.
  {   
    //printf(ANSI_FMT("The parentheses exist and match\n",ANSI_FG_RED));
    return true;
  }
  else{return false;}
}

static int get_priority(int type){
   switch(type){
     case TK_AND: return 1;
     case TK_EQ: return 2;
     case TK_NOTEQ: return 2;
     case '+': return 3;
     case '-': return 3;
     case '*': return 4;
     case '/': return 4;
     case TK_DEREF: return 5;
     case TK_NEGTIVE: return 5;
     default: return 0;
   }
   return 0;
}


static int find_main_op(int p,int q){
  int pointer=p;
  int ans=0;
  int tmp_type=-1;
  int cnt=0;
  while(pointer<=q){
    switch(tokens[pointer].type){
      case '(' : cnt++; break;
      case ')' : cnt--; break;
      case TK_DEC_INT : break;
      case TK_HEX: break;
      case TK_REG: break;
      default:
        if(cnt==0){
          if(tmp_type==-1){tmp_type=tokens[pointer].type; ans=pointer;}
          if(tokens[pointer].type==TK_DEREF || tokens[pointer].type==TK_NEGTIVE){
          	if(get_priority(tokens[pointer].type)<get_priority(tmp_type)){
          		tmp_type=tokens[pointer].type;
            		ans=pointer;
          	}
          }
          else if(get_priority(tokens[pointer].type)<=get_priority(tmp_type)) //the priority of the current op is lower.
          {

            tmp_type=tokens[pointer].type;
            ans=pointer;
          }
        }

    }
    pointer++;
  }
  
  return ans;
   
}


static bool certain_type(int type){
  switch(type){
    case '+': return true;
    case '-': return true;
    case '*': return true;
    case '/': return true;
    case '(': return true;
    case ')': return false;
    case TK_EQ: return true;
    case TK_DEC_INT: return false;
    case TK_HEX: return false;
    case TK_REG: return false;
    case TK_DEREF: return true;
    case TK_NOTEQ: return true;
    case TK_AND: return true;
    case TK_NEGTIVE: return true;
    default: assert(0); break;
  }
  return false;
}


