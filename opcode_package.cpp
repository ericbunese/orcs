#include "simulator.hpp"

// =====================================================================
opcode_package_t::opcode_package_t() {

    /// TRACE Variables
    sprintf(this->opcode_assembly, "N/A");
    this->opcode_operation = INSTRUCTION_OPERATION_NOP;
    this->opcode_address = 0;
    this->opcode_size = 0;

	for (uint32_t i=0; i < 16; i++){
		read_regs[i] = 0;
		write_regs[i] = 0;
	}
	
    this->base_reg = 0;
    this->index_reg = 0;

    this->is_read = false;
    this->read_address = 0;
    this->read_size = 0;

    this->is_read2 = false;
    this->read2_address = 0;
    this->read2_size = 0;

    this->is_write = false;
    this->write_address = 0;
    this->write_size = 0;

    this->branch_type = BRANCH_UNCOND;
    this->is_indirect = false;

    this->is_predicated = false;
    this->is_prefetch = false;

};


