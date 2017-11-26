// ============================================================================
// ============================================================================
class processor_t {
  private:


  public:

  uint64_t keepalive;
  int penalidade;
  btb_t *BTB;
  perceptron_t *PCPT;
  twobitcounter_t *twobit;
  
  cache_t *l1;
  cache_t *l2;
  
  stride_t *st;
  sms_t* sms;
  
  uint64_t cycles_spent_memory;
  
  // ====================================================================
  /// Methods
  // ====================================================================
  processor_t();
  void allocate();
  void clock();
  void statistics();
};
