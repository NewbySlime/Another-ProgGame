#ifndef I_CHILD_RESPONDENT_HEADER
#define I_CHILD_RESPONDENT_HEADER


namespace GameUtils{
  class I_child_respondent{
    public:
      void on_header_received(const header_struct* header_comm, const void* buffer);
  };
}

#endif