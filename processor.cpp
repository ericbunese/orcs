#include "simulator.hpp"

#define n_entradas_btb 512
#define associatividade_btb 4
#define penalidade_miss_btb 8
#define penalidade_miss_pcpt 8
#define tamanho_perceptron 62
#define numero_perceptrons 64

// =====================================================================
processor_t::processor_t() {

};

// =====================================================================
void processor_t::allocate() {

	this->good_guesses = 0;
	this->bad_guesses = 0;
	this->penalties = 0;
	this->total_guesses = 0;
	this->penalidade = 0;
	this->presumed_taken = 0;
	this->presumed_nottaken = 0;

	this->PCPT_table = (perceptron_table_t*)malloc(sizeof(perceptron_table_t));
	this->PCPT_table->perceptron_values = (int64_t*)malloc(sizeof(int64_t)*tamanho_perceptron);
	this->PCPT_table->perceptrons = (perceptron_t**)malloc(sizeof(perceptron_t*)*numero_perceptrons);
	this->PCPT_table->update = -1;

	for (int i=0;i<numero_perceptrons;++i)
	{
		this->PCPT_table->perceptrons[i] = (perceptron_t*)malloc(sizeof(perceptron_t));
		printf("%d: %p\n", i, this->PCPT_table->perceptrons[i]);
		init_PCPT(this->PCPT_table->perceptrons[i], i);
	}

	this->PCPT_table->perceptron_values[0] = 1;
	for (int i=1;i<tamanho_perceptron;++i)
	{
		this->PCPT_table->perceptron_values[i] = 0;
	}

	printf("allocation complete\n");
};

// =====================================================================
void processor_t::clock() {

	/// Get the next instruction from the trace
	opcode_package_t new_instruction;

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
		else if (new_instruction.opcode_operation == INSTRUCTION_OPERATION_BRANCH && new_instruction.branch_type == BRANCH_COND)
		{
			perceptron_t *p;
			//Updates previous perceptron
			if (this->PCPT_table->update != -1 && this->PCPT_table->update<numero_perceptrons)
			{
				p = this->PCPT_table->perceptrons[this->PCPT_table->update];
				this->total_guesses++;
			 //printf("Actual PC: %10.0ld\n", new_instruction.opcode_address);
			 if (this->nextPC!=0)
			 {
			  if ((p->taken==1 && this->nextPC!=new_instruction.opcode_address) || (p->taken==-1 && this->nextPC==new_instruction.opcode_address))
			  {
			   H(1);
			   T(p, p->taken);
			   this->good_guesses++;
			   //printf("Perceptron Predicted Correctly.\n");
			  }
			  else
			  {
			   //printf("Perceptron Missed\n");
			   this->bad_guesses++;
			   this->penalties++;
			   penalidade = penalidade_miss_pcpt;
			   H(-1);
			   T(p, p->taken*-1);
			  }
			 }
			 this->PCPT_table->update = -1;
			 //printf("==========\n\n");
			}
			//Executes the Perceptron
			int use = (int)new_instruction.opcode_address % numero_perceptrons;
			p = this->PCPT_table->perceptrons[use];
			//printf("index %d, pointer %p: \n", use, p);
			P(p, new_instruction);
		}
	}
};

// =====================================================================
void processor_t::statistics() {
	ORCS_PRINTF("######################################################\n");
	ORCS_PRINTF("processor_t\n");

	printf("Perceptron Geometry Information:\n");
	printf("Number of Perceptrons: %d\n", numero_perceptrons);
	printf("Perceptron Buffer Size: %d\n", tamanho_perceptron);

 printf("\nPerceptron good guesses: %ld\nPerceptron bad guesses: %ld\nPenalties: %ld cycles\n", this->good_guesses, this->bad_guesses, this->penalties);
	printf("Right guesses: %10.2f\n", ((double)this->good_guesses/(double)this->total_guesses)*100);
	printf("Wrong guesses: %10.2f\n", ((double)this->bad_guesses/(double)this->total_guesses)*100);
	printf("Presumed Taken: %ld\nPresumed Not Taken: %ld\n", this->presumed_taken, this->presumed_nottaken);
};

void processor_t::P(perceptron_t* p, opcode_package_t inst)
{
	uint64_t sum = 0;
	if (p)
	{
		for (int i=1;i<tamanho_perceptron;++i)
		{
		 sum += this->PCPT_table->perceptron_values[i]*p->perceptron_weights[i];
		}

		p->perceptron_output = p->perceptron_weights[0]+sum;
		//printf("Perceptron Output: %ld\n", p->perceptron_output);
		if (p->perceptron_output<0)
		{
		 p->taken = -1;
			this->presumed_nottaken++;
		}
		else
		{
		 p->taken = 1;
			this->presumed_taken++;
		}
		this->nextPC = inst.opcode_address+inst.opcode_size;
		//printf("Next PC:%10.0ld\n", this->nextPC);
		this->PCPT_table->update = p->id;
	}
}

void processor_t::T(perceptron_t* p, int64_t t)
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

void processor_t::H(uint64_t h)
{
	for (int i=tamanho_perceptron-1;i>0;--i)
	{
		this->PCPT_table->perceptron_values[i] = this->PCPT_table->perceptron_values[i-1];
	}
	this->PCPT_table->perceptron_values[1] = h;
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

void processor_t::init_PCPT(perceptron_t* p, int id)
{
	p->perceptron_weights = (int64_t*)malloc(sizeof(int64_t)*tamanho_perceptron);
	p->taken = 0;
	p->id = id;

	for (int i=0;i<tamanho_perceptron;++i)
		 p->perceptron_weights[i] = 0;

	p->perceptron_threshold = 1.93*tamanho_perceptron+14;
}
