//
//  cache.cpp
//  orcs
//
//  Created by Eric Bunese on 07/10/17.
//  Copyright © 2017 Eric Bunese. All rights reserved.
//

#include "cache.hpp"

cache_t::cache_t(int ass, int nlines, int lat)
{
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

