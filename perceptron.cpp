//
//  perceptron.cpp
//  orcs
//
//  Created by Eric Bunese on 07/10/17.
//  Copyright © 2017 Eric Bunese. All rights reserved.
//

#include "perceptron.hpp"

perceptron_t::perceptron_t()
{
  this->penalidade = 0;
  this->PCPT_table = (perceptron_table_t*)malloc(sizeof(perceptron_table_t));
	this->PCPT_table->perceptron_values = (int64_t*)malloc(sizeof(int64_t)*tamanho_perceptron);
	this->PCPT_table->perceptrons = (perceptron_s_t**)malloc(sizeof(perceptron_s_t*)*numero_perceptrons);
	this->PCPT_table->update = -1;
  
  for (int i=0;i<numero_perceptrons;++i)
	{
		this->PCPT_table->perceptrons[i] = (perceptron_s_t*)malloc(sizeof(perceptron_s_t));
		init_PCPT(this->PCPT_table->perceptrons[i], i);
	}

	this->PCPT_table->perceptron_values[0] = 1;
	for (int i=1;i<tamanho_perceptron;++i)
	{
		this->PCPT_table->perceptron_values[i] = 0;
	}
}

// ====================================================================
/// Methods
// ====================================================================

int perceptron_t::update(uint64_t pc)
{
  perceptron_s_t *p;
  if (this->PCPT_table->update != -1 && this->PCPT_table->update<numero_perceptrons)
  {
    p = this->PCPT_table->perceptrons[this->PCPT_table->update];
    this->pcpt_queues++;
    
    if (this->nextPC!=0)
    {
      if ((p->taken==1 && this->nextPC!=pc) || (p->taken==-1 && this->nextPC==pc))
      {
        H(1);
        T(p, p->taken);
        this->pcpt_smart++;
        this->penalidade = 0;
      }
      else
      {
        this->pcpt_dumb++;
        this->pcpt_pent++;
        this->penalidade = penalidade_miss_pcpt;
        H(-1);
        T(p, p->taken*-1);
      }
    }
    this->PCPT_table->update = -1;
  }
  return this->penalidade;
}

//Makes the perceptron decision
void perceptron_t::P(uint64_t pc, uint64_t npc)
{
  perceptron_s_t *p;
  
  p = this->PCPT_table->perceptrons[(int) pc % numero_perceptrons];
  
	uint64_t sum = 0;
	if (p)
	{
		for (int i=1;i<tamanho_perceptron;++i)
		{
		 sum += this->PCPT_table->perceptron_values[i]*p->perceptron_weights[i];
		}

		p->perceptron_output = p->perceptron_weights[0]+sum;
    
		if (p->perceptron_output<0)
		{
      p->taken = -1;
			this->pcpt_notta++;
		}
		else
		{
      p->taken = 1;
			this->pcpt_taken++;
		}
		//this->nextPC = pc+opcodeSize;
    this->nextPC = npc;
		this->PCPT_table->update = p->id;
	}
}

//Trains the perceptron according to the result
void perceptron_t::T(perceptron_s_t* p, int64_t t)
{
	int64_t n;
	if (p)
	{
		if (sign(p->perceptron_output)!=t || abs(p->perceptron_output)<=0)
		{
		 for (int64_t i=1;i<tamanho_perceptron;++i)
		 {
		  n = p->perceptron_weights[i]+t*this->PCPT_table->perceptron_values[i];
		  if (n<=255 && n>=-256)
		  	p->perceptron_weights[i] = n;
		 }
		}
	}
}

//Adds a value to the global history
void perceptron_t::H(uint64_t h)
{
	for (int i=tamanho_perceptron-1;i>0;--i)
	{
		this->PCPT_table->perceptron_values[i] = this->PCPT_table->perceptron_values[i-1];
	}
	this->PCPT_table->perceptron_values[1] = h;
}

//Initializes the perceptron
void perceptron_t::init_PCPT(perceptron_s_t* p, int id)
{
  p->perceptron_weights = (int64_t*)malloc(sizeof(int64_t)*tamanho_perceptron);
	p->taken = 0;
	p->id = id;

	for (int i=0;i<tamanho_perceptron;++i)
		 p->perceptron_weights[i] = 0;

	p->perceptron_threshold = 1.93*tamanho_perceptron+14;
}

//Perceptron Statistics
void perceptron_t::pcpt_statistics()
{
  printf("######################################################\n");
  printf("PERCEPTRON STATISTICS\n\n");
  printf("Num Perceptrons:\t%d\n", numero_perceptrons);
  printf("History Lenght:\t\t%d\n", tamanho_perceptron);
  printf("Perceptron Penalty\t%d\n\n", penalidade_miss_pcpt);
  printf("PCPT Queues:\t%ld\n", this->pcpt_queues);
  printf("PCPT HITS:\t\t%ld\n", this->pcpt_smart);
  printf("PCPT MISS:\t\t%ld\n", this->pcpt_dumb);
  printf("PCPT Penalties:\t%ld\n\n", this->pcpt_pent);
  printf("PCPT HIT RATIO: \t%5.2f\n", ((double)this->pcpt_smart / (double)this->pcpt_queues)*100);
  printf("PCPT MISS RATIO: \t%5.2f\n", ((double)this->pcpt_dumb / (double)this->pcpt_queues)*100);
}

// ====================================================================
/// Helper Methods
// ====================================================================
int64_t perceptron_t::sign(int64_t n)
{
	if (n<0)
		return -1;
	else if (n>0)
		return 1;
	return 0;
}

int64_t perceptron_t::abs(int64_t n)
{
	if (n<0)
		return n*-1;
	return n;
}
