//
//  sms.cpp
//  orcs
//
//  Created by Eric Bunese on 26/11/2017.
//  Copyright © 2017 Eric Bunese. All rights reserved.
//

#include "sms.hpp"

//Initialize Structure
sms_t::sms_t()
{
  this->requests_made = 0;
  this->guesses_taken = 0;

  this->filterTable = (filterT_t**)malloc(sizeof(filterT_t*)*tam_t);
  this->accTable = (accT_t**)malloc(sizeof(accT_t*)*tam_t);

  for (int i=0;i<tam_t;++i)
  {
    this->filterTable[i] = (filterT_t*)malloc(sizeof(filterT_t));
    this->filterTable[i]->base = 0;
    this->filterTable[i]->off = 0;
    this->filterTable[i]->pc = 0;
    this->filterTable[i]->lru = 0;
    this->filterTable[i]->val = false;
    this->accTable[i] = (accT_t*)malloc(sizeof(accT_t));
    this->accTable[i]->base = 0;
    this->accTable[i]->pc = 0;
    this->accTable[i]->lru = 0;
    this->accTable[i]->pat = (bool*)malloc(sizeof(bool)*64);
    this->accTable[i]->val = false;

    for (int j=0;j<64;++j)
    {
      this->accTable[i]->pat[j] = false;
    }
  }

  this->phTable = (phT_t**)malloc(sizeof(phT_t*)*tam_pht);

  for (int i=0;i<tam_pht;++i)
  {
    this->phTable[i] = (phT_t*)malloc(sizeof(phT_t));
    this->phTable[i]->base = 0;
    this->phTable[i]->pc = 0;
    this->phTable[i]->lru = 0;
    this->phTable[i]->pat = 0;
    this->phTable[i]->pat = (bool*)malloc(sizeof(bool)*64);
    this->phTable[i]->str = 0;
    this->phTable[i]->val = false;

    for (int j=0;j<64;++j)
    {
      this->phTable[i]->pat[j] = false;
    }
  }
}

//Helper Method to get block number
int sms_t::offset_address(uint64_t address)
{
  return (((address & 1048576) >> 14) & 64);
}

//Helper Method to get base address (without block number)
uint64_t sms_t::base_address(uint64_t address)
{
  uint64_t aux = address & 1048576;
  return address - aux;
}

//Helper Method to get a prefetch address.
uint64_t sms_t::sms_request(uint64_t pc, uint64_t cc)
{
 bool stop = false;
 for (int i=0;i<tam_pht && !stop;++i)
 {
  if (this->phTable[i]->pc==pc)
  {
    if (this->phTable[i]->val)
    {
      this->phTable[i]->lru = cc;
      //search next streamed block
      bool bit;
      
      if (this->phTable[i]->str >= 64)
          this->phTable[i]->str = 0;
      
      for (int j=this->phTable[i]->str;j<64;++j)
      {
        bit = (this->phTable[i]->pat[j]) & 1;
        if (bit)
        {
          this->phTable[i]->str = j;
          return this->phTable[i]->base + (64 *j);
        }
      }
    }
    else
    {
      return 0;
    }
  }
 }
 return 0;
}

//Helper Method to search Accumulation Table for addresses - also triggers a search in filter table if necessary.
void sms_t::sms_acc(uint64_t pc, uint64_t address, uint64_t cc)
{
  uint64_t lru = cc;
  int use = 0;
  bool stop = false, found = false;
  
  //Search for entry in Accumulation Table
  for (int i=0;i<tam_t && !stop;++i)
  {
    if (this->accTable[i]->val)
    {
      if (this->accTable[i]->pc == pc)
      {
        this->accTable[i]->lru = cc;
        use = i;
        stop = true;
        found = true;
      }
      else
      {
        if (this->accTable[i]->lru < lru)
        {
          lru = this->accTable[i]->lru;
          use = i;
        }
      }
    }
    else
    {
      use = i;
      stop = true;
    }
  }
  
  //Test search
  if (found)
  {
    int bit = this->offset_address(address);
    this->accTable[use]->pat[bit] = true;
  }
  else
  {
    this->sms_filter(pc, address, cc);
  }
}

//Helper Method to search Filter Table for addresses - also calls a TRIGGER ACCESS if necessary. (AUTO)
void sms_t::sms_filter(uint64_t pc, uint64_t address, uint64_t cc)
{
  uint64_t lru = cc;
  int use = 0;
  bool stop = false, found = false;
  
  //Search for entry in Filter Table
  for (int i=0;i<tam_t;++i)
  {
    if (this->filterTable[i]->val)
    {
      if (this->filterTable[i]->pc == pc)
      {
        this->filterTable[i]->lru = cc;
        use = i;
        stop = true;
        found = true;
      }
      else
      {
        if (this->filterTable[i]->lru < lru)
        {
          lru = this->filterTable[i]->lru;
          use = i;
        }
      }
    }
    else
    {
      use = i;
      stop = true;
    }
  }
  
  if (found)
  {
    int off = this->offset_address(address);
    if (off != this->filterTable[use]->off)
    {
      //Insert at acc table
      this->sms_addAcc(pc, this->base_address(address), this->filterTable[use]->off, off, cc);
      this->filterTable[use]->val = false;
    }
  }
  else
  {
    //TRIGGER ACCESS
    this->filterTable[use]->pc = pc;
    this->filterTable[use]->base = this->base_address(address);
    this->filterTable[use]->lru = cc;
    this->filterTable[use]->off = this->offset_address(address);
    this->filterTable[use]->val = true;
  }
}

////Helper Method to add to Accumulation Table (AUTO)
void sms_t::sms_addAcc(uint64_t pc, uint64_t base, int off1, int off2, uint64_t cc)
{
  uint64_t lru = cc;
  int use = 0;
  bool stop = false;
  
  //Search for entry in Accumulation Table
  for (int i=0;i<tam_t && !stop;++i)
  {
    if (this->accTable[i]->val)
    {
      if (this->accTable[i]->lru < lru)
      {
        lru = this->accTable[i]->lru;
        use = i;
      }
    }
    else
    {
      use = i;
      stop = true;
    }
  }
  
  //Insert in Accumulation Table
  this->accTable[use]->pc = pc;
  this->accTable[use]->base = base;
  this->accTable[use]->lru = cc;
  this->accTable[use]->val = true;
  for (int i=0;i<64;++i)
  {
    if (i!=off1 && i!=off2)
        this->accTable[use]->pat[i] = false;
    else this->accTable[use]->pat[i] = true;
  }
}

//Helper Method to add to PHT.
void sms_t::sms_addPht(accT_t* fromAcc, uint64_t cc)
{
  uint64_t lru = cc;
  bool stop = false;
  int use = 0;
  
  for (int i=0;i<tam_pht && !stop;++i)
  {
    if (this->phTable[i]->val)
    {
      if (this->phTable[i]->lru < lru)
      {
        lru = this->phTable[i]->lru;
        use = i;
      }
    }
    else
    {
      use = i;
      stop = true;
    }
  }
  
  this->phTable[use]->val = true;
  this->phTable[use]->lru = cc;
  this->phTable[use]->str = 0;
  this->phTable[use]->base = fromAcc->base;
  this->phTable[use]->pc = fromAcc->pc;
  
  for (int i=0;i<64;++i)
  {
    this->phTable[use]->pat[i] = fromAcc->pat[i];
  }
}

// ====================================================================
/// API
// ====================================================================
uint64_t sms_t::sms_query(uint64_t pc, uint64_t address, uint64_t cc)
{
  this->requests_made++;
  uint64_t pf = this->sms_request(pc, cc);
  if (pf > 0)
  {
    this->guesses_taken++;
    return pf;
  }
  else
  {
    this->sms_acc(pc, address, cc);
    return 0;
  }
}

void sms_t::sms_cleanup(uint64_t pc, uint64_t cc)
{
  for (int i=0;i<tam_t;++i)
  {
    if (this->filterTable[i]->pc == pc)
    {
      this->filterTable[i]->val = false;
      return;
    }
    
    if (this->accTable[i]->pc == pc)
    {
      this->sms_addPht(this->accTable[i], cc);
      this->accTable[i]->val = false;
      return;
    }
  }
}

void sms_t::sms_statistics()
{
  printf("######################################################\n");
  printf("SPATIAL MEMORY STREAMING STATISTICS\n\n");
  printf("Requests Made: \t\t%lld\n", this->requests_made);
  printf("Guesses Taken: \t\t%lld\n", this->guesses_taken);
}
