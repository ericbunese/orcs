#include "simulator.hpp"

// =====================================================================
orcs_engine_t::orcs_engine_t() {

};

// =====================================================================
void orcs_engine_t::allocate() {
	this->trace_reader = new trace_reader_t;
	this->processor = new processor_t;
};

