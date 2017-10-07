// ============================================================================
// ============================================================================
class trace_reader_t {
    private:
        gzFile gzStaticTraceFile;
        gzFile gzDynamicTraceFile;
        gzFile gzMemoryTraceFile;

        /// Control the trace reading
        bool is_inside_bbl;
        uint32_t currect_bbl;
        uint32_t currect_opcode;

        /// Control the static dictionary
        uint32_t binary_total_bbls;     /// Total of BBLs for the static file
        uint32_t *binary_bbl_size;      /// Total of instructions for each BBL
        opcode_package_t **binary_dict; /// Complete dictionary of BBLs and instructions

		uint64_t fetch_instructions;

    public:
        // ====================================================================
        /// Methods
        // ====================================================================
        trace_reader_t();
        ~trace_reader_t();
        void allocate(char *trace_file_name);
        void statistics();

        /// Generate the static dictionary
        void get_total_bbls();
        void define_binary_bbl_size();
        void generate_binary_dict();

        bool trace_string_to_opcode(char *input_string, opcode_package_t *opcode);
        bool trace_next_dynamic(uint32_t *next_bbl);
        bool trace_next_memory(uint64_t *next_address, uint32_t *operation_size, bool *is_read);
        bool trace_fetch(opcode_package_t *m);
};



