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
  
  for (int i=0;i<nstrides;++i)
  {
    this->strides[i] = (strider_t*)malloc(sizeof(strider_t));
  }
}

uint64_t stride_t::stride_request()
{
  return 0;
}
