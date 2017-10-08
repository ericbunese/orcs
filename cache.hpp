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

class cache_t {
  private:
    typedef struct cacheline
    {
      uint64_t *tag;
      uint64_t *lru;
      bool *val;
      bool *dir;
    }cacheline_t;
  
    typedef struct cacheset
    {
      cacheline_t **lines;
      int latencia;
    }cacheset_t;
  
    cacheset_t *cache;
    int ass;
    int nlines;

  public:
  // ====================================================================
  /// Methods
  // ====================================================================
  cache_t(int ass, int nlines, int lat);
};


#endif /* cache_hpp */
