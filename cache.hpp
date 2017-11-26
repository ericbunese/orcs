//
//  cache.hpp
//  orcs
//
//  Created by Eric Bunese on 07/10/17.
//  Copyright © 2017 Eric Bunese. All rights reserved.
//

#ifndef cache_hpp
#define cache_hpp

#include <inttypes.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "stride.hpp"
#include "sms.hpp"

class cache_t {
  private:
    typedef struct cacheline
    {
      uint64_t *tag;
      uint64_t *lru;
      uint64_t *use;
      bool *val;
      bool *dir;
      bool *str;
    }cacheline_t;
  
    typedef struct cacheset
    {
      cacheline_t **lines;
      int latencia;
    }cacheset_t;
  
    cacheset_t *cache;
    int ass;
    int nlines;
    uint64_t wait_cycles;
    char *name;
  
    uint64_t cache_ques;
    uint64_t cache_hits;
    uint64_t cache_miss;

  public:
  // ====================================================================
  /// Methods
  // ====================================================================
  cache_t(char* name, int ass, int nlines, int lat);
  bool cache_search(uint64_t address, uint64_t cc, bool is_write, stride_t *st, sms_t *sms, uint64_t pc);
  int cache_getLatencia();
  void cache_statistics();
  uint64_t cache_getWait();
  void cache_load(uint64_t address, uint64_t cc, bool pf);
};


#endif /* cache_hpp */
