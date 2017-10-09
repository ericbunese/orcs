//
//  twobitcounter.hpp
//  orcs
//
//  Created by Eric Bunese on 07/10/17.
//  Copyright © 2017 Eric Bunese. All rights reserved.
//

#ifndef twobitcounter_hpp
#define twobitcounter_hpp
#define penalidade_miss_twobit 8
#define taken_status 2

#include <inttypes.h>
#include <stdlib.h>
#include <stdio.h>

class twobitcounter_t {
  private:
  int status;
  int taken;
  uint64_t twobit_smart;
  uint64_t twobit_dumb;
  uint64_t twobit_pent;
  uint64_t twobit_queues;
  uint64_t nextPC;

  public:
  // ====================================================================
  /// Methods
  // ====================================================================
  twobitcounter_t();
  void P(uint64_t npc);
  void update(uint64_t pc);
  int max(int a, int b);
  int min(int a, int b);
  void twobit_statistics();
};

#endif /* twobitcounter_hpp */
