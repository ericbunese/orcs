#include "simulator.hpp"

#define n_entradas_btb 512
#define associatividade_btb 4
#define penalidade_miss_btb 8
#define penalidade_miss_pcpt 8
#define tamanho_perceptron 62
#define numero_perceptrons 64

// =====================================================================
processor_t::processor_t() {
  printf("Program Initialized.\nPlease stand by...\n");
};

// =====================================================================
void processor_t::allocate() {
  //Intialize local variables
	this->penalidade = 0;
  this->keepalive = 0;
  this->cycles_spent_memory = 0;

  //Initialize BTB
  this->BTB = new btb_t;

  //Initialize perceptrons
  this->PCPT = new perceptron_t;
  
  //Initialize two bit counter
  this->twobit = new twobitcounter_t;

	//Initialize caches
  char *l1name = (char*)malloc(sizeof(char)*3);
  l1name[0] = 'L';
  l1name[1] = '1';
  l1name[2] = '\0';
  char *l2name = (char*)malloc(sizeof(char)*3);
  l2name[0] = 'L';
  l2name[1] = '2';
  l2name[2] = '\0';
  this->l1 = new cache_t(l1name, 4, 256, 1);
  this->l2 = new cache_t(l2name, 8, 15625, 4);
  free(l1name);
  free(l2name);
};

// =====================================================================
void processor_t::clock() {

	/// Get the next instruction from the trace
	opcode_package_t new_in;

  if (this->keepalive <= 0)
  {
    printf("Keep Alive: %lld\n", orcs_engine.global_cycle);
    this->keepalive = 65767<<10;
  }
  else this->keepalive--;

	if (this->penalidade-->0)
	{
		// Skip
	}
	else
	{
		if (!orcs_engine.trace_reader->trace_fetch(&new_in))
		{
			/// If EOF
			orcs_engine.simulator_alive = false;
		}
		else
		{
      if (new_in.opcode_operation == INSTRUCTION_OPERATION_BRANCH && new_in.branch_type == BRANCH_COND)
      {
        if (this->BTB->btb_search(new_in.opcode_address, new_in.opcode_size))
        {
          //Retrieve address from BTB
          uint64_t nextPC = this->BTB->btb_nextPC();
        
          this->penalidade = this->PCPT->update(new_in.opcode_address);
          this->PCPT->P(new_in.opcode_address, nextPC);
        
          this->twobit->update(new_in.opcode_address);
          this->twobit->P(nextPC);
        }
        else
        {
          //btb miss, apply penalty.
          this->penalidade = this->BTB->btb_pents();
        }
      }
      // =====================================================================
      // MEMORY AND CACHE ACCESS
      // =====================================================================
      int lat = 0;
      if (new_in.is_read) // Memory access for read 1
      {
        if (this->l1->cache_search(new_in.read_address, orcs_engine.global_cycle, false))
        {
        }
        else
        {
          if (this->l2->cache_search(new_in.read_address, orcs_engine.global_cycle, false))
          {
          }
          else
          {
            //DRAM Latency
            lat += 200;
          }
          //L2 Latency
          lat += this->l2->cache_getLatencia();
        }
        //L1 Latency
        lat += this->l1->cache_getLatencia();
      }
      // =====================================================================
      if (new_in.is_read2) // Memory access for read 2
      {
        if (this->l1->cache_search(new_in.read2_address, orcs_engine.global_cycle, false))
        {
        }
        else
        {
          if (this->l2->cache_search(new_in.read2_address, orcs_engine.global_cycle, false))
          {
          }
          else
          {
            //DRAM Latency
            lat += 200;
          }
          //L2 Latency
          lat += this->l2->cache_getLatencia();
        }
        //L1 Latency
        lat += this->l1->cache_getLatencia();
      }
      // =====================================================================
      if (new_in.is_write) // Memory access for write
      {
        if (this->l1->cache_search(new_in.write_address, orcs_engine.global_cycle, true))
        {
        }
        else
        {
          if (this->l2->cache_search(new_in.write_address, orcs_engine.global_cycle, true))
          {
          }
          else
          {
            //DRAM Latency
            lat += 200;
          }
          //L2 Latency
          lat += this->l2->cache_getLatencia();
        }
        //L1 Latency
        lat += this->l1->cache_getLatencia();
      }
      this->penalidade = lat;
      this->cycles_spent_memory += lat;
		}
	}
};

// =====================================================================
void processor_t::statistics() {
	ORCS_PRINTF("######################################################\n");
	ORCS_PRINTF("processor_t\n");
  
  ORCS_PRINTF("Global Cycle Count:\t%lld\n", orcs_engine.global_cycle);
  ORCS_PRINTF("Cycle: \t\t%5.2f\n", ((double)orcs_engine.global_cycle/(double)200000000)*100);
  ORCS_PRINTF("Cycles spent on memory fetch: \t%lld\n", this->cycles_spent_memory);
  
  this->BTB->btb_statistics();
  this->PCPT->pcpt_statistics();
  this->twobit->twobit_statistics();
  this->l1->cache_statistics();
  this->l2->cache_statistics();
};
