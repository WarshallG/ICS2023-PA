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

#include "sdb.h"

#define NR_WP 32



static WP wp_pool[NR_WP] = {};
static WP *head = NULL, *free_ = NULL;

void init_wp_pool() {
  int i;
  for (i = 0; i < NR_WP; i ++) {
    wp_pool[i].NO = i;
    wp_pool[i].next = (i == NR_WP - 1 ? NULL : &wp_pool[i + 1]);
  }

  head = NULL;
  free_ = wp_pool;
}

/* TODO: Implement the functionality of watchpoint */

WP* new_wp(char* exp){
	if(free_->next==NULL) assert(0);
	WP* new_head;
	new_head=free_;
	free_=free_->next;
	new_head->next=NULL;
	strcpy(new_head->exp,exp);
	bool success=false;
	new_head->value=expr(exp,&success);  //calculate value.
	if(head==NULL){head=new_head;}
	else{
		WP* tmp=head;
		while(tmp->next!=NULL)
		{
			tmp=tmp->next;
		}
		tmp->next=new_head;
	}
	return new_head;
}


void free_wp(WP *wp){
	if(wp==head) head=head->next;
	else{
		WP* p=head;
		while(p->next!=wp)
		{ p=p->next;}
		p->next=wp->next;
	}
	wp->next=free_;  
	free_=wp;  //put the new free wp into free_
	return ;
}


WP* scan_wp(){
  WP *p=head;
  bool success=false;
  while(p!=NULL)
  {
    if (expr(p->exp,&success)!=p->value)
    {
      return p;
    }
    p=p->next;
  }
  return NULL;

}





















