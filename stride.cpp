//
//  stride.cpp
//  orcs
//
//  Created by Eric Eduardo Bunese on 12/11/17.
//  Copyright © 2017 Eric Bunese. All rights reserved.
//

#include "stride.hpp"

stride_t::stride_t(int nstrides)
{
  this->strides = (strider_t**)malloc(sizeof(strider_t*)*nstrides);
  this->requests_made = 0;
  this->proper_strides = 0;
  this->useful_strides = 0;
  
  for (int i=0;i<nstrides;++i)
  {
    this->strides[i] = (strider_t*)malloc(sizeof(strider_t));
    this->strides[i]->lru = 0;
    this->strides[i]->pc = 0;
    this->strides[i]->prev = 0;
    this->strides[i]->stride = 0;
    this->strides[i]->state = -1;
  }
}

uint64_t stride_t::stride_request(uint64_t pc, uint64_t address, uint64_t cc)
{
  int oldest = 0, reset = 0;
  uint64_t lru = 0;
  
  this->requests_made++;
  
  for (int i=0;i<16;++i)
  {
    if (this->strides[i]->pc!=pc)
    {
      if (this->strides[i]->state!=-1)
      {
        if (this->strides[i]->lru<lru)
        {
          oldest = i;
          reset = 1;
          lru = this->strides[i]->lru;
        }
      }
      else
      {
        oldest = i;
        reset = 0;
        break;
      }
    }
    else
    {
      oldest = i;
      reset = 0;
      break;
    }
  }
  
  
  if (reset || this->strides[oldest]->state==-1)
  {
    this->strides[oldest]->state = 0;
    this->strides[oldest]->prev = 0;
    this->strides[oldest]->stride = 0;
  }
  
  this->strides[oldest]->pc = pc;
  this->strides[oldest]->lru = cc;
  
  if (this->strides[oldest]->prev+this->strides[oldest]->stride == address)
  {
    if (this->strides[oldest]->state>=1)
    {
      //printf("Prefetcher %d loads address %lld with stride %lld for pc %lld\n", oldest, pc+this->strides[oldest]->stride, this->strides[oldest]->stride, pc);
      this->proper_strides++;
      return pc+this->strides[oldest]->stride;
    }
    else
    {
      //printf("Prefetcher %d is at state %d for pc %lld\n", oldest, this->strides[oldest]->state, pc);
      this->strides[oldest]->state++;
    }
  }
  else if (this->strides[oldest]->prev!=0)
  {
    this->strides[oldest]->stride = address - this->strides[oldest]->prev;
    this->strides[oldest]->state = 0;
    //printf("Prefetcher Set %d is at state %d for pc %lld\n", oldest, this->strides[oldest]->state, pc);
  }
  
  this->strides[oldest]->prev = address;
  return 0;
}

void stride_t::stride_statistics()
{
  printf("######################################################\n");
  printf("STRIDE PREFETCHER STATISTICS\n\n");
  printf("Requests Made: \t\t%lld\n", this->requests_made);
  printf("Strides Detected: \t%lld\n", this->proper_strides);
  printf("Useful prefetches: \t%lld\n", this->useful_strides);
}
