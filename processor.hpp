// ============================================================================
// ============================================================================
class processor_t {
    private:


    public:

     typedef struct perceptron
     {
      int64_t *perceptron_weights;
      int64_t perceptron_threshold;
      int64_t perceptron_output;
      int taken;
      int id;
     }perceptron_t;

     typedef struct perceptron_table
     {
      perceptron_t **perceptrons;
      int64_t *perceptron_values;
      int update;
     }perceptron_table_t;

     perceptron_table_t *PCPT_table;
     int active_PCPT;

     typedef struct cacheline
     {
     	uint64_t tag;
     	int valid;
     	int dirty;
     	uint64_t lru;
     }cacheline_t;

     typedef struct cacheset
     {
     	cacheline_t* sets;
     	int associativity;
     }cacheset_t;

     typedef struct cache
     {
     	cacheset* sets;
     	int setnum;
     }cache_t;

    	int penalidade;
     uint64_t good_guesses;
     uint64_t bad_guesses;
     uint64_t penalties;
     uint64_t total_guesses;

     uint64_t nextPC;
		// ====================================================================
		/// Methods
		// ====================================================================
		processor_t();
	    void allocate();
	    void clock();
	    void statistics();
     void P(perceptron_t* p, opcode_package_t inst);
     void T(perceptron_t* p, int64_t t);
     void init_PCPT(perceptron_t* p, int id);
     int64_t sign(int64_t n);
     int64_t abs(int64_t n);
};
