//
//  sms.hpp
//  orcs
//
//  Created by Eric Bunese on 26/11/2017.
//  Copyright © 2017 Eric Bunese. All rights reserved.
//

#ifndef sms_hpp
#define sms_hpp

#include <inttypes.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define tam_t 32
#define tam_pht 256

class sms_t {
private:
  typedef struct filterT
  {
    uint64_t base;
    int off;
    uint64_t pc;
    uint64_t lru;
    bool val;
  }filterT_t;
  
  typedef struct accT
  {
    uint64_t base;
    uint64_t pc;
    uint64_t lru;
    bool *pat;
    bool val;
  }accT_t;
  
  typedef struct phT
  {
    uint64_t base;
    uint64_t pc;
    uint64_t lru;
    bool *pat;
    int str;
    bool val;
  }phT_t;
  
  filterT_t** filterTable;
  accT_t** accTable;
  phT_t** phTable;
  
  uint64_t requests_made;
  uint64_t guesses_taken;
  
  int offset_address(uint64_t address);
  uint64_t base_address(uint64_t address);
  uint64_t sms_request(uint64_t pc, uint64_t cc);
  void sms_acc(uint64_t pc, uint64_t address, uint64_t cc);
  void sms_filter(uint64_t pc, uint64_t address, uint64_t cc);
  void sms_addAcc(uint64_t pc, uint64_t base, int off1, int off2, uint64_t cc);
  void sms_addPht(accT_t* fromAcc, uint64_t cc);
  uint64_t sms_index(uint64_t pc);
  
public:
  // ====================================================================
  /// Methods
  // ====================================================================
  uint64_t useful_guesses;
  sms_t();
  uint64_t sms_query(uint64_t pc, uint64_t address, uint64_t cc);
  void sms_cleanup(uint64_t pc, uint64_t cc);
  void sms_statistics();
};

#endif /* sms_hpp */
