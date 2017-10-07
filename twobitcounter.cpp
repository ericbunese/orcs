//
//  twobitcounter.cpp
//  orcs
//
//  Created by Eric Bunese on 07/10/17.
//  Copyright © 2017 Eric Bunese. All rights reserved.
//

#include "twobitcounter.hpp"

twobitcounter_t::twobitcounter_t()
{
  this->status = 0;
  this->taken = 0;
}

void twobitcounter_t::P(uint64_t pc, uint64_t opcodeSize)
{
  if (this->status >= taken_status)
  {
    this->taken = 1;
  }
  else
  {
    this->taken = -1;
  }
  this->twobit_queues++;
  this->nextPC = pc+opcodeSize;
}

void twobitcounter_t::update(uint64_t pc)
{
  if (this->taken!=0)
  {
    if ((this->taken==1 && pc != this->nextPC) || (this->taken==-1 && pc == this->nextPC))
    {
      this->twobit_smart++;
      this->status = min(this->status+1, 3);
    }
    else
    {
      this->twobit_dumb++;
      this->twobit_pent++;
      this->status = max(this->status-1, 0);
    }
  }
}

void twobitcounter_t::twobit_statistics()
{
  printf("######################################################\n");
  printf("2BIT COUNTER STATISTICS\n\n");
  printf("2BIT Penalty\t%d\n\n", penalidade_miss_twobit);
  printf("2BIT Queues:\t%lld\n", this->twobit_queues);
  printf("2BIT HITS:\t\t%lld\n", this->twobit_smart);
  printf("2BIT MISS:\t\t%lld\n", this->twobit_dumb);
  printf("2BIT Penalties:\t%lld\n\n", this->twobit_pent);
  printf("2BIT HIT RATIO: \t%5.2f\n", ((double)this->twobit_smart / (double)this->twobit_queues)*100);
  printf("2BIT MISS RATIO: \t%5.2f\n", ((double)this->twobit_dumb / (double)this->twobit_queues)*100);
}

int twobitcounter_t::max(int a, int b)
{
 return (a>b)?a:b;
}

int twobitcounter_t::min(int a, int b)
{
 return (a<b)?a:b;
}
