// ============================================================================
// ============================================================================
class processor_t {
    private:    
    
    
    public:

		typedef struct linha_btb
		{
			uint64_t tag;
			uint64_t lru;
			uint64_t add;
			int val;
		}tLinha_btb;

    	tLinha_btb **BTB;
    	int penalidade;
		// ====================================================================
		/// Methods
		// ====================================================================
		processor_t();
	    void allocate();
	    void clock();
	    void statistics();
	    void BTBzar(opcode_package_t inst);
};
