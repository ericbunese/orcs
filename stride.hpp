//
//  stride.hpp
//  orcs
//
//  Created by Eric Eduardo Bunese on 12/11/17.
//  Copyright © 2017 Eric Bunese. All rights reserved.
//

#ifndef stride_hpp
#define stride_hpp


#include <inttypes.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

class stride_t {
private:
  typedef struct stride
  {
    uint64_t pc;
    uint64_t prev;
    uint64_t stride;
    int state;
  }strider_t;

  strider_t **strides;
  
public:
  // ====================================================================
  /// Methods
  // ====================================================================
  stride_t(int nstrides);
  uint64_t stride_request();
};

#endif /* stride_hpp */
