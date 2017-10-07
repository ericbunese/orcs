//
//  perceptron.hpp
//  orcs
//
//  Created by Eric Bunese on 07/10/17.
//  Copyright © 2017 Eric Bunese. All rights reserved.
//

#ifndef perceptron_hpp
#define perceptron_hpp

#include <inttypes.h>
#include <stdlib.h>
#include <stdio.h>

#define tamanho_perceptron 62
#define numero_perceptrons 64
#define penalidade_miss_pcpt 8

class perceptron_t {
  private:

    typedef struct perceptron_s
     {
      int64_t *perceptron_weights;
      int64_t perceptron_threshold;
      int64_t perceptron_output;
      int taken;
      int id;
     }perceptron_s_t;

     typedef struct perceptron_table
     {
      perceptron_s_t **perceptrons;
      int64_t *perceptron_values;
      int update;
     }perceptron_table_t;
  
     perceptron_table_t *PCPT_table;
     //int active_PCPT;
  
     int penalidade;
     uint64_t pcpt_smart;
     uint64_t pcpt_dumb;
     uint64_t pcpt_pent;
     uint64_t pcpt_queues;
     uint64_t pcpt_taken;
     uint64_t pcpt_notta;
     uint64_t nextPC;
  
  public:
  // ====================================================================
  /// Methods
  // ====================================================================
  perceptron_t();
  void init_PCPT(perceptron_s_t* p, int id);
  int update(uint64_t pc);
  void P(uint64_t pc, uint64_t opcodeSize);
  void T(perceptron_s_t* p, int64_t t);
  void H(uint64_t h);
  void pcpt_statistics();
  int64_t sign(int64_t n);
  int64_t abs(int64_t n);
};

#endif /* perceptron_hpp */
