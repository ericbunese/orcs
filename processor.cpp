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

  //Initialize BTB
  this->BTB = new btb_t;

  //Initialize perceptrons
  this->PCPT = new perceptron_t;
  
  //Initialize two bit counter
  this->twobit = new twobitcounter_t;

	//Initialize caches
  this->l1 = new cache_t(4, 256, 1);
};

// =====================================================================
void processor_t::clock() {

	/// Get the next instruction from the trace
	opcode_package_t new_instruction;

  if (--(this->keepalive) <= 0)
  {
    printf("Keep Alive: %lld\n", orcs_engine.global_cycle);
    this->keepalive = 65767;
  }

	if (this->penalidade-->0)
	{
		// Skip
	}
	else
	{
		if (!orcs_engine.trace_reader->trace_fetch(&new_instruction))
		{
			/// If EOF
			orcs_engine.simulator_alive = false;
		}
		else
		{
      if (new_instruction.opcode_operation == INSTRUCTION_OPERATION_BRANCH && new_instruction.branch_type == BRANCH_COND)
      {
        if (this->BTB->btb_search(new_instruction.opcode_address, new_instruction.opcode_size))
        {
          //Retrieve address from BTB
          uint64_t nextPC = this->BTB->btb_nextPC();
        
          this->penalidade = this->PCPT->update(new_instruction.opcode_address);
          this->PCPT->P(new_instruction.opcode_address, nextPC);
        
          this->twobit->update(new_instruction.opcode_address);
          this->twobit->P(nextPC);
        }
        else
        {
          //btb miss, apply penalty.
          this->penalidade = this->BTB->btb_pents();
        }
      }
      else if (new_instruction.opcode_operation == INSTRUCTION_OPERATION_MEM_LOAD || new_instruction.opcode_operation == INSTRUCTION_OPERATION_MEM_STORE)
      {
        //Memory instruction, let's do science.
      }
		}
	}
};

// =====================================================================
void processor_t::statistics() {
	ORCS_PRINTF("######################################################\n");
	ORCS_PRINTF("processor_t\n");
  
  this->BTB->btb_statistics();
  this->PCPT->pcpt_statistics();
  this->twobit->twobit_statistics();
};
