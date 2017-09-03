#include "simulator.hpp"

// =====================================================================
trace_reader_t::trace_reader_t() {

};

// =====================================================================
trace_reader_t::~trace_reader_t() {
    gzclose(gzStaticTraceFile);
    gzclose(gzDynamicTraceFile);
    gzclose(gzMemoryTraceFile);
};

// =====================================================================
void trace_reader_t::allocate(char *trace_file) {

    char file_name[TRACE_LINE_SIZE];

    // =================================================================
    /// Open the Static Trace File
    // =================================================================
    file_name[0] = '\0';
    snprintf(file_name, sizeof(file_name), "%s.tid%d.stat.out.gz", trace_file, 0);
    DEBUG_PRINTF("Static File = %s => READY !\n", file_name);
    this->gzStaticTraceFile = gzopen(file_name, "ro");    /// Open the .gz file
    ERROR_ASSERT_PRINTF(gzStaticTraceFile != NULL, "Could not open the static file.\n%s\n", file_name);
    DEBUG_PRINTF("Static File = %s => READY !\n", file_name);

    // =================================================================
    /// Open the Dynamic Trace File
    // =================================================================
    file_name[0] = '\0';
    snprintf(file_name, sizeof(file_name), "%s.tid%d.dyn.out.gz", trace_file, 0);
    this->gzDynamicTraceFile = gzopen(file_name, "ro");    /// Open the .gz group
    ERROR_ASSERT_PRINTF(this->gzDynamicTraceFile != NULL, "Could not open the dynamic file.\n%s\n", file_name);
    DEBUG_PRINTF("Dynamic File = %s => READY !\n", file_name);

    // =================================================================
    /// Open the Memory Trace File
    // =================================================================
    file_name[0] = '\0';
    snprintf(file_name, sizeof(file_name), "%s.tid%d.mem.out.gz", trace_file, 0);
    this->gzMemoryTraceFile = gzopen(file_name, "ro");    /// Open the .gz group
    ERROR_ASSERT_PRINTF(this->gzMemoryTraceFile != NULL, "Could not open the memory file.\n%s\n", file_name);
    DEBUG_PRINTF("Memory File = %s => READY !\n", file_name);

    /// Set the trace_reader controls
    this->is_inside_bbl = false;
    this->currect_bbl = 0;
    this->currect_opcode = 0;



	/// Obtain the number of BBLs
	this->get_total_bbls();

	/// Allocate the vector of BBL sizes
	this->binary_bbl_size = new uint32_t[this->binary_total_bbls];
	ERROR_ASSERT_PRINTF(this->binary_bbl_size != NULL, "Could not allocate memory\n");
	/// Initialize
	for (uint32_t bbl = 0; bbl < this->binary_total_bbls; bbl++) {
		this->binary_bbl_size[bbl] = 0;
	};

	/// Define the size of each specific BBL
	this->define_binary_bbl_size();

	/// Create the opcode for each BBL
	this->binary_dict = new opcode_package_t*[this->binary_total_bbls];
	ERROR_ASSERT_PRINTF(this->binary_dict != NULL, "Could not allocate memory\n");
	for (uint32_t bbl = 1; bbl < this->binary_total_bbls; bbl++) {
		this->binary_dict[bbl] = new opcode_package_t[this->binary_bbl_size[bbl]];
		ERROR_ASSERT_PRINTF(this->binary_dict[bbl] != NULL, "Could not allocate memory\n");
	}

	this->generate_binary_dict();
};

// =====================================================================
void trace_reader_t::get_total_bbls() {
    char file_line[TRACE_LINE_SIZE] = "";
    bool file_eof = false;
    uint32_t bbl = 0;
    this->binary_total_bbls = 0;

    gzclearerr(this->gzStaticTraceFile);
    gzseek(this->gzStaticTraceFile, 0, SEEK_SET);   /// Go to the Begin of the File
    file_eof = gzeof(this->gzStaticTraceFile);      /// Check is file not EOF
    ERROR_ASSERT_PRINTF(!file_eof, "Static File Unexpected EOF.\n")

    while (!file_eof) {
        gzgets(this->gzStaticTraceFile, file_line, TRACE_LINE_SIZE);
        file_eof = gzeof(this->gzStaticTraceFile);

        if (file_line[0] == '@') {
            bbl = (uint32_t)strtoul(file_line + 1, NULL, 10);
            this->binary_total_bbls++;
            ERROR_ASSERT_PRINTF(bbl == this->binary_total_bbls, "Expected sequenced bbls.\n")
        }
    }

    this->binary_total_bbls++;
};

// =====================================================================
void trace_reader_t::define_binary_bbl_size() {
    char file_line[TRACE_LINE_SIZE] = "";
    bool file_eof = false;
    uint32_t bbl = 0;

    gzclearerr(this->gzStaticTraceFile);
    gzseek(this->gzStaticTraceFile, 0, SEEK_SET);   /// Go to the Begin of the File
    file_eof = gzeof(this->gzStaticTraceFile);      /// Check is file not EOF
    ERROR_ASSERT_PRINTF(!file_eof, "Static File Unexpected EOF.\n")

    while (!file_eof) {
        gzgets(this->gzStaticTraceFile, file_line, TRACE_LINE_SIZE);
        file_eof = gzeof(this->gzStaticTraceFile);

        if (file_line[0] == '\0' || file_line[0] == '#') {     /// If Comment, then ignore
            continue;
        }
        else if (file_line[0] == '@') {
            bbl++;
            binary_bbl_size[bbl] = 0;
        }
        else {
            binary_bbl_size[bbl]++;
        }
    }
};

// =====================================================================
void trace_reader_t::generate_binary_dict() {
    char file_line[TRACE_LINE_SIZE] = "";
    bool file_eof = false;
    uint32_t BBL = 0;                           /// Actual BBL (Index of the Vector)
    uint32_t opcode = 0;
    opcode_package_t NewOpcode;                 /// Actual Opcode

    gzclearerr(this->gzStaticTraceFile);
    gzseek(this->gzStaticTraceFile, 0, SEEK_SET);  /// Go to the Begin of the File
    file_eof = gzeof(this->gzStaticTraceFile);      /// Check is file not EOF
    ERROR_ASSERT_PRINTF(!file_eof, "Static File Unexpected EOF.\n")

    while (!file_eof) {
        gzgets(this->gzStaticTraceFile, file_line, TRACE_LINE_SIZE);
        file_eof = gzeof(this->gzStaticTraceFile);

        DEBUG_PRINTF("Read: %s\n", file_line);
        if (file_line[0] == '\0' || file_line[0] == '#') {     /// If Comment, then ignore
            continue;
        }
        else if (file_line[0] == '@') {                       /// If New BBL
            DEBUG_PRINTF("BBL %u with %u instructions.\n", BBL, opcode); /// Debug from previous BBL
            opcode = 0;

            BBL = (uint32_t)strtoul(file_line + 1, NULL, 10);
            ERROR_ASSERT_PRINTF(BBL < this->binary_total_bbls, "Static has more BBLs than previous analyzed static file.\n");
        }
        else {                                                  /// If Inside BBL
            DEBUG_PRINTF("Opcode %u = %s", opcode, file_line);
            this->trace_string_to_opcode(file_line, &this->binary_dict[BBL][opcode]);
            ERROR_ASSERT_PRINTF(this->binary_dict[BBL][opcode].opcode_address != 0, "Static trace file generating opcode address equal to zero.\n")
            opcode++;
        }
    }
};

// =====================================================================
/// Convert Static Trace line into Instruction
/// Field #:  01 |   02   |   03  |  04   |   05  |  06  |  07    |  08   |  09  |  10   |  11  |  12   |  13   |  14   |  15      |  16        | 17
/// Type:    Asm | Opcode | Inst. | Inst. | #Read | Read | #Write | Write | Base | Index | Is   | Is    | Is    | Cond. | Is       | Is         | Is
///          Cmd | Number | Addr. | Size  | Regs  | Regs | Regs   | Regs  | Reg. | Reg.  | Read | Read2 | Write | Type  | Indirect | Predicated | Pfetch
///
/// Static File Example:
///
/// #
/// # Compressed Trace Generated By Pin to SiNUCA
/// #
/// @1
/// MOV 8 4345024 3 1 12 1 19 12 0 1 3 0 0 0 0 0
/// ADD 1 4345027 4 1 12 2 12 34 0 0 3 0 0 0 0 0 0
/// TEST 1 4345031 3 2 19 19 1 34 0 0 3 0 0 0 0 0 0
/// JNZ 7 4345034 2 2 35 34 1 35 0 0 4 0 0 0 1 0 0
/// @2
/// CALL_NEAR 9 4345036 5 2 35 15 2 35 15 15 0 1 0 0 1 0 0 0
///
bool trace_reader_t::trace_string_to_opcode(char *input_string, opcode_package_t *opcode) {
    char *sub_string = NULL;
    char *tmp_ptr = NULL;
    uint32_t sub_fields, count, i;
    count = 0;

    for (i = 0; input_string[i] != '\0'; i++) {
        count += (input_string[i] == ' ');
    }
    ERROR_ASSERT_PRINTF(count >= 13, "Error converting Text to Instruction (Wrong  number of fields %d), input_string = %s\n", count, input_string)

    sub_string = strtok_r(input_string, " ", &tmp_ptr);
    strcpy(opcode->opcode_assembly, sub_string);

    sub_string = strtok_r(NULL, " ", &tmp_ptr);
    opcode->opcode_operation = instruction_operation_t(strtoul(sub_string, NULL, 10));

    sub_string = strtok_r(NULL, " ", &tmp_ptr);
    opcode->opcode_address = strtoull(sub_string, NULL, 10);

    sub_string = strtok_r(NULL, " ", &tmp_ptr);
    opcode->opcode_size = strtoul(sub_string, NULL, 10);

    /// Number of Read Registers
    sub_string = strtok_r(NULL, " ", &tmp_ptr);
    sub_fields = strtoul(sub_string, NULL, 10);

    for (i = 0; i < sub_fields; i++) {
        sub_string = strtok_r(NULL, " ", &tmp_ptr);
        opcode->read_regs[i] = strtoul(sub_string, NULL, 10);
    }

    /// Number of Write Registers
    sub_string = strtok_r(NULL, " ", &tmp_ptr);
    sub_fields = strtoul(sub_string, NULL, 10);

    for (i = 0; i < sub_fields; i++) {
        sub_string = strtok_r(NULL, " ", &tmp_ptr);
        opcode->write_regs[i] = strtoul(sub_string, NULL, 10);
    }

    sub_string = strtok_r(NULL, " ", &tmp_ptr);
    opcode->base_reg = strtoull(sub_string, NULL, 10);

    sub_string = strtok_r(NULL, " ", &tmp_ptr);
    opcode->index_reg = strtoull(sub_string, NULL, 10);


    sub_string = strtok_r(NULL, " ", &tmp_ptr);
    opcode->is_read = (sub_string[0] == '1');

    sub_string = strtok_r(NULL, " ", &tmp_ptr);
    opcode->is_read2 = (sub_string[0] == '1');

    sub_string = strtok_r(NULL, " ", &tmp_ptr);
    opcode->is_write = (sub_string[0] == '1');

    sub_string = strtok_r(NULL, " ", &tmp_ptr);
    opcode->branch_type = branch_t(strtoull(sub_string, NULL, 10));

    sub_string = strtok_r(NULL, " ", &tmp_ptr);
    opcode->is_indirect = (sub_string[0] == '1');

    sub_string = strtok_r(NULL, " ", &tmp_ptr);
    opcode->is_predicated = (sub_string[0] == '1');

    sub_string = strtok_r(NULL, " ", &tmp_ptr);
    opcode->is_prefetch = (sub_string[0] == '1');

    return OK;
};


// =====================================================================
bool trace_reader_t::trace_next_dynamic(uint32_t *next_bbl) {
    static char file_line[TRACE_LINE_SIZE];
    file_line[0] = '\0';

    bool valid_dynamic = false;
    *next_bbl = 0;

    while (!valid_dynamic) {
        /// Obtain the next trace line
        if (gzeof(this->gzDynamicTraceFile)) {
            return FAIL;
        }
        char *buffer = gzgets(this->gzDynamicTraceFile, file_line, TRACE_LINE_SIZE);
        if (buffer == NULL) {
            return FAIL;
        }

        /// Analyze the trace line
        if (file_line[0] == '\0' || file_line[0] == '#') {
            DEBUG_PRINTF("Dynamic trace line (empty/comment): %s\n", file_line);
            continue;
        }
        else if (file_line[0] == '$') {
            DEBUG_PRINTF("Dynamic trace line (synchronization): %s\n", file_line);
            continue;
        }
        else {
            /// BBL is always greater than 0
            /// If strtoul==0 the line could not be converted.
            DEBUG_PRINTF("Dynamic trace line: %s\n", file_line);

            *next_bbl = strtoul(file_line, NULL, 10);
            ERROR_ASSERT_PRINTF(*next_bbl != 0, "The BBL from the dynamic trace file should not be zero. Dynamic line %s\n", file_line);

			valid_dynamic = true;
        }
    }
    return OK;
};

// =====================================================================
/// Convert Dynamic Memory Trace line into Instruction Memory Operands
/// Field #:    1  |  2   |    3    |  4
/// Type:      R/W | R/W  | Memory  | BBL
///            Op. | Size | Address | Number
///
/// Memory File Example:
///
/// #
/// # Compressed Trace Generated By Pin to SiNUCA
/// #
/// W 8 140735291283448 1238
/// W 8 140735291283440 1238
/// W 8 140735291283432 1238
// =====================================================================
bool trace_reader_t::trace_next_memory(uint64_t *mem_address, uint32_t *mem_size, bool *mem_is_read) {
    static char file_line[TRACE_LINE_SIZE];
    file_line[0] = '\0';

    bool valid_memory = false;

    while (!valid_memory) {
        /// Obtain the next trace line
        if (gzeof(this->gzMemoryTraceFile)) {
            return FAIL;
        }
        char *buffer = gzgets(this->gzMemoryTraceFile, file_line, TRACE_LINE_SIZE);
        if (buffer == NULL) {
            return FAIL;
        }

        /// Analyze the trace line
        if (file_line[0] == '\0' || file_line[0] == '#') {
            DEBUG_PRINTF("Memory trace line (empty/comment): %s\n", file_line);
            continue;
        }
        else {
            char *sub_string = NULL;
            char *tmp_ptr = NULL;
            uint32_t count = 0, i = 0;
            while (file_line[i] != '\0') {
                count += (file_line[i] == ' ');
                i++;
            }
            ERROR_ASSERT_PRINTF(count == 3, "Error converting Text to Memory (Wrong  number of fields %d)\n", count)
            DEBUG_PRINTF("Memory trace line: %s\n", file_line);

            sub_string = strtok_r(file_line, " ", &tmp_ptr);
            *mem_is_read = strcmp(sub_string, "R") == 0;

            sub_string = strtok_r(NULL, " ", &tmp_ptr);
            *mem_size = strtoull(sub_string, NULL, 10);

            sub_string = strtok_r(NULL, " ", &tmp_ptr);
            *mem_address = strtoull(sub_string, NULL, 10);

            sub_string = strtok_r(NULL, " ", &tmp_ptr);
            valid_memory = true;
        }
    }
    return OK;
};

// =====================================================================
bool trace_reader_t::trace_fetch(opcode_package_t *m) {

    opcode_package_t NewOpcode;
    bool success;
    uint32_t new_BBL;

    // =================================================================
    /// Fetch new BBL inside the dynamic file.
    // =================================================================
    if (!this->is_inside_bbl) {
        success = this->trace_next_dynamic(&new_BBL);
        if (success) {
            this->currect_bbl = new_BBL;
            this->currect_opcode = 0;
            this->is_inside_bbl = true;
        }
        else {
            ORCS_PRINTF("End of dynamic simulation trace\n");
            return FAIL;
        }
    }

    // =================================================================
    /// Fetch new INSTRUCTION inside the static file.
    // =================================================================
    NewOpcode = this->binary_dict[this->currect_bbl][this->currect_opcode];
    DEBUG_PRINTF("BBL:%u  OPCODE:%u = %s\n",this->currect_bbl, this->currect_opcode, NewOpcode.opcode_assembly);

    this->currect_opcode++;
    if (this->currect_opcode >= this->binary_bbl_size[this->currect_bbl]) {
        this->is_inside_bbl = false;
        this->currect_opcode = 0;
    }

    // =================================================================
    /// Add SiNUCA information
    // =================================================================
    *m = NewOpcode;

    // =========================================================================
    /// If it is LOAD/STORE -> Fetch new MEMORY inside the memory file
    // =========================================================================
    bool mem_is_read;
    if (m->is_read) {
        trace_next_memory(&m->read_address, &m->read_size, &mem_is_read);
        ERROR_ASSERT_PRINTF(mem_is_read == true, "Expecting a read from memory trace\n");
    }

    if (m->is_read2) {
        trace_next_memory(&m->read_address, &m->read_size, &mem_is_read);
        ERROR_ASSERT_PRINTF(mem_is_read == true, "Expecting a read2 from memory trace\n");
    }

    if (m->is_write) {
        trace_next_memory(&m->read_address, &m->read_size, &mem_is_read);
        ERROR_ASSERT_PRINTF(mem_is_read == false, "Expecting a write from memory trace\n");
    }

	this->fetch_instructions++;
    return OK;
};

// =====================================================================
void trace_reader_t::statistics() {
	ORCS_PRINTF("######################################################\n");
	ORCS_PRINTF("trace_reader_t\n");
	ORCS_PRINTF("fetch_instructions:%lu\n", (unsigned long)this->fetch_instructions);

};
