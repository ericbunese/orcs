#include "simulator.hpp"

#define n_entradas_btb 512
#define associatividade_btb 4
#define penalidade_miss_btb 8
#define tamanho_perceptron 128

// =====================================================================
processor_t::processor_t() {

};

// =====================================================================
void processor_t::allocate() {

	this->good_guesses = 0;
	this->bad_guesses = 0;
	this->penalties = 0;

	PCPT = (perceptron_t*)malloc(sizeof(perceptron_t));
	PCPT->perceptron_size = tamanho_perceptron;
	PCPT->perceptron_weights = (int64_t*)malloc(sizeof(int64_t)*tamanho_perceptron);
	PCPT->perceptron_values = (int64_t*)malloc(sizeof(int64_t)*tamanho_perceptron);

 	PCPT->update = 0;
	PCPT->perceptron_values[0] = 1;

	for (int64_t i=0;i<tamanho_perceptron;++i)
						PCPT->perceptron_weights[i] = 0;

	PCPT->perceptron_threshold = 1.93*tamanho_perceptron+14;
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
			if (PCPT->update)
			{
				if (this->nextPC!=0)
				{
					if (this->nextPC!=new_instruction.opcode_address)
					{
						printf("Perceptron Missed\n");
						this->bad_guesses++;
						this->penalties++;
						penalidade = penalidade_miss_btb;
						T(1);
					}
					else
					{
						T(-1);
						this->good_guesses++;
						printf("Perceptron Predicted Correctly.\n");
					}
					this->nextPC = 0;
				}
				PCPT->update = 0;
			}

			//BTBzar(new_instruction);
			P(new_instruction);
		}
	}
};

// =====================================================================
void processor_t::statistics() {
	ORCS_PRINTF("######################################################\n");
	ORCS_PRINTF("processor_t\n");

 printf("Perceptron good guesses: %ld\nPerceptron bad guesses: %ld\nPenalties: %ld cycles\n", this->good_guesses, this->bad_guesses, this->penalties);

};

void processor_t::P(opcode_package_t inst)
{
	if (inst.opcode_operation == INSTRUCTION_OPERATION_BRANCH)
	{
		int64_t sum = 0;
		for (int64_t i=1;i<PCPT->perceptron_size;++i)
		{
			sum += PCPT->perceptron_values[i]*PCPT->perceptron_weights[i];
		}
		PCPT->perceptron_output = PCPT->perceptron_weights[0]+sum;
		printf("Perceptron Output: %ld\n", PCPT->perceptron_output);
		PCPT->update = 1;
		this->nextPC = inst.opcode_address+inst.opcode_size;
	}
}

void processor_t::T(int64_t t)
{
	if (sign(PCPT->perceptron_output)!=t || abs(PCPT->perceptron_output)<=0)
	{
		for (int64_t i=0;i<PCPT->perceptron_size;++i)
		{
			PCPT->perceptron_weights[i] = PCPT->perceptron_weights[i]+t*PCPT->perceptron_values[i];
		}
	}
}

int64_t processor_t::sign(int64_t n)
{
	if (n<0)
					return -1;
	else if (n>0)
					return 1;
	return 0;
}

int64_t processor_t::abs(int64_t n)
{
	if (n<0)
					return n*-1;
	return n;
}
