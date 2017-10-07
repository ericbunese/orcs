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
  // ====================================================================
  /// Methods
  // ====================================================================
  processor_t();
  void allocate();
  void clock();
  void statistics();
};
