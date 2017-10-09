//
//  btb.cpp
//  orcs
//
//  Created by Eric Bunese on 07/10/17.
//  Copyright © 2017 Eric Bunese. All rights reserved.
//

#include "btb.hpp"
#define n_entradas_btb 512
#define associatividade_btb 4
#define penalidade_miss_btb 8

btb_t::btb_t()
{
  this->btb_hits = 0;
  this->btb_miss = 0;
  this->btb_pent = 0;
  this->nextPC = 0;
  
  this->btb = (linha_btb_t**)malloc(sizeof(linha_btb_t*)*n_entradas_btb);

  for (int i=0;i<n_entradas_btb;++i)
	{
		this->btb[i] = (linha_btb_t*)malloc(sizeof(linha_btb_t));
		this->btb[i]->val = 0;
	}
}

int btb_t::btb_search(uint64_t pc, uint64_t opcodeSize)
{
 uint64_t npc;
 uint64_t set;
 uint64_t bas;
 uint64_t lin;
 uint64_t lru = 0;
 linha_btb_t *cur;
 int hit=-1, oldest = 0;
 
 set = (pc >> 2) & 127;
 npc = pc + opcodeSize;
 bas = (set << 2);
 
 for (int i=0;i<4;++i)
 {
   lin = bas+(uint64_t)i;
   cur = this->btb[lin];
   
   if (cur->val)
   {
     if (cur->tag == pc)
     {
       hit = i;
     }
     
     if (cur->lru < lru)
     {
       lru = cur->lru;
       oldest = i;
     }
   }
   else
   {
     oldest = i;
   }
 }

 this->btb_queues++;
 
 if (hit!=-1)
 {
   cur = this->btb[bas+(uint64_t)hit];
   this->btb_hits++;
   this->nextPC = cur->add;
   return 1;
 }
 else
 {
   this->btb_miss++;
   cur = this->btb[bas+(uint64_t)oldest];
   cur->tag = pc;
   cur->val = true;
   cur->add = npc;
   this->nextPC = npc;
   this->btb_pent += penalidade_miss_btb;
   return 0;
 }
}

uint64_t btb_t::btb_nextPC()
{
  return this->nextPC;
}

int btb_t::btb_pents()
{
  return penalidade_miss_btb;
}

void btb_t::btb_statistics()
{
  printf("######################################################\n");
  printf("BTB STATISTICS\n\n");
  printf("BTB SIZE:\t%d\n", n_entradas_btb);
  printf("BTB WAYS:\t%d\n", associatividade_btb);
  printf("BTB PLTY:\t%d\n\n", penalidade_miss_btb);
  printf("BTB QUES:\t%ld\n", this->btb_queues);
  printf("BTB HITS:\t%ld\n", this->btb_hits);
  printf("BTB MISS:\t%ld\n", this->btb_miss);
  printf("BTB PENT:\t%ld\n\n", this->btb_pent);
  printf("BTB HIT RATIO: \t\t%5.2f\n", ((double)this->btb_hits / (double)this->btb_queues)*100);
  printf("BTB MISS RATIO: \t%5.2f\n", ((double)this->btb_miss / (double)this->btb_queues)*100);
}
