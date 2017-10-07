//
//  btb.hpp
//  orcs
//
//  Created by Eric Bunese on 07/10/17.
//  Copyright © 2017 Eric Bunese. All rights reserved.
//

#ifndef btb_hpp
#define btb_hpp

#include <inttypes.h>
#include <stdlib.h>
#include <stdio.h>

class btb_t {
  private:

    typedef struct linha_btb {
      uint64_t tag;
      uint64_t lru;
      uint64_t add;
      bool val;
    }linha_btb_t;
  
    linha_btb_t **btb;
  
    uint64_t btb_queues;
    uint64_t btb_hits;
    uint64_t btb_miss;
    uint64_t btb_pent;
    uint64_t nextPC;

  public:
  // ====================================================================
  /// Methods
  // ====================================================================
  btb_t();
  int btb_search(uint64_t pc, uint64_t opcodeSize);
  int btb_pents();
  void btb_statistics();
};

#endif /* btb_hpp */
