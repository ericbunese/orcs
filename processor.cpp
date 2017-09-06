#include "simulator.hpp"

#define n_entradas_btb 512
#define associatividade_btb 4
#define penalidade_miss_btb 8

// =====================================================================
processor_t::processor_t() {

};

// =====================================================================
void processor_t::allocate() {
	penalidade = 0;

	this->BTB = (tLinha_btb**)malloc(sizeof(tLinha_btb*)*n_entradas_btb);

	for (int i=0;i<n_entradas_btb;++i)
	{
		this->BTB[i] = (tLinha_btb*)malloc(sizeof(tLinha_btb));
		this->BTB[i]->val = 0;
	}
};

// =====================================================================
void processor_t::clock() {

	/// Get the next instruction from the trace
	opcode_package_t new_instruction;

	if (penalidade-->0)
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
			BTBzar(new_instruction);
		}
	}
};

// =====================================================================
void processor_t::statistics() {
	ORCS_PRINTF("######################################################\n");
	ORCS_PRINTF("processor_t\n");

};

void processor_t::BTBzar(opcode_package_t inst) 
{
	uint64_t BTB_set;
	uint64_t BTB_baseLine;
	uint64_t BTB_line;
	tLinha_btb *linha;

	if (inst.opcode_operation == INSTRUCTION_OPERATION_BRANCH)
	{
		BTB_set = (inst.opcode_address >> 2)&127;

		printf("Caiu no set %ld\n", BTB_set);

		// Procura na BTB:
		int hit = 0;
		int oldest = 1;
		uint64_t oldest_lru = 0;
		BTB_baseLine = (BTB_set<<2);
		
		for (int i=0;i<4;++i)
		{
			BTB_line = BTB_baseLine+(uint64_t)i;

			linha = this->BTB[BTB_line];
			if (linha->val)
			{
				if (linha->tag == inst.opcode_address)
				{
					hit = i+1;
				}

				if (linha->lru < oldest_lru)
				{
					oldest_lru = linha->lru;
					oldest = i;
				}
			}
			else oldest = i+1;
		}

		if (hit)
		{
			// Sucesso então, já tava na BTB
			linha = this->BTB[BTB_baseLine+(uint64_t)hit];
			//linha->lru = (orcs_engine.get_global_cycle);
		}
		else
		{
			// Escreve na BTB
			linha = this->BTB[BTB_baseLine+(uint64_t)oldest];
			linha->tag = inst.opcode_address;
			//linha->lru = (orcs_engine.get_global_cycle);
			linha->val = 1;
			penalidade = penalidade_miss_btb;
			printf("ESCREVENDO BTB: set: %ld, linha: %ld\n", BTB_set, BTB_line);
		}
	}
}