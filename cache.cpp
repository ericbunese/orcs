//
//  cache.cpp
//  orcs
//
//  Created by Eric Bunese on 07/10/17.
//  Copyright © 2017 Eric Bunese. All rights reserved.
//

#include "cache.hpp"

cache_t::cache_t(char*name, int ass, int nlines, int lat)
{
  this->name = (char*)malloc(sizeof(char)*(strlen(name)+1));
  strcpy(this->name, name);
  
  this->cache_ques = 0;
  this->cache_hits = 0;
  this->cache_miss = 0;
  
  this->cache = (cacheset_t*)malloc(sizeof(cacheset_t));
  this->cache->latencia = lat;
  this->cache->lines = (cacheline_t**)malloc(sizeof(cacheline_t*)*nlines);
  this->ass = ass;
  this->nlines = nlines;
  
  for (int i=0;i<nlines;++i)
  {
    this->cache->lines[i] = (cacheline_t*)malloc(sizeof(cacheline_t));
    this->cache->lines[i]->tag = (uint64_t*)malloc(sizeof(uint64_t)*ass);
    this->cache->lines[i]->lru = (uint64_t*)malloc(sizeof(uint64_t)*ass);
    this->cache->lines[i]->val = (bool*)malloc(sizeof(bool)*ass);
    this->cache->lines[i]->dir = (bool*)malloc(sizeof(bool)*ass);
    for (int j=0;j<ass;++j)
    {
      this->cache->lines[i]->tag[j] = 0;
      this->cache->lines[i]->lru[j] = 0;
      this->cache->lines[i]->val[j] = 0;
      this->cache->lines[i]->dir[j] = 0;
    }
  }
}

bool cache_t::cache_search(uint64_t address, uint64_t cc, bool is_write)
{
  //printf("====================\n====================\n");
  //printf("Cache %s search for address: %lld\n", this->name, address);
  uint64_t index, tag, lru = 0;
  cacheline_t *line;
  int oldest = 0;
  
  this->cache_ques++;
  
  //Index the cache line
  tag = (address >> 6);
  index = (tag & 1023) % this->nlines;
  //printf("Tag is %lld, index is %lld\n", tag, index);
  
  line = this->cache->lines[index];
  
  //Associative-set search
  for (int i=0;i<this->ass;++i)
  {
    if (line->val[i])
    {
      if (line->tag[i] == tag)
      {
        this->cache_hits++;
        //printf("Found at associative set %d\n", i);
        line->lru[i] = cc;
        line->dir[i] = (is_write || line->dir[i]);
        return true;
      }
      
      if (lru > line->lru[i])
      {
        oldest = i;
        lru = line->lru[i];
      }
    }
  }
  
  //@TODO ver sobre writeback e tal - aqui rolou uma saída
  if (line->val[oldest] && line->dir[oldest])
  {
    //WriteBack
  }
  
  //printf("Not found, written at associative set %d\n", oldest);
  this->cache_miss++;
  
  line->val[oldest] = true;
  line->dir[oldest] = true;
  line->lru[oldest] = cc;
  line->tag[oldest] = tag;
  return false;
}

int cache_t::cache_getLatencia()
{
  return this->cache->latencia;
}

void cache_t::cache_statistics()
{
  printf("######################################################\n");
  printf("CACHE %s STATISTICS\n\n", this->name);
  printf("Num lines:\t%d\n", this->nlines);
  printf("Associativity:\t\t%d\n", this->ass);
  printf("Cache Penalty\t%d\n\n", this->cache->latencia);
  printf("Cache Queues:\t%ld\n", this->cache_ques);
  printf("Cache HITS:\t\t%ld\n", this->cache_hits);
  printf("Cache MISS:\t\t%ld\n", this->cache_miss);
  printf("Cache HIT RATIO: \t%5.2f\n", ((double)this->cache_hits / (double)this->cache_ques)*100);
  printf("Cache MISS RATIO: \t%5.2f\n", ((double)this->cache_miss / (double)this->cache_ques)*100);
}
