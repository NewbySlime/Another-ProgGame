#ifndef COMMUNICATION_HEADER
#define COMMUNICATION_HEADER

#include "../../IPC.h"

#ifdef _WIN64
#include "Windows.h"
#ifndef PIPE_BUFFER_LENGTH
#define PIPE_BUFFER_LENGTH 512
#endif // PIPE_BUFFER_LENGTH
#endif // _WIN64


using namespace GameUtils;

class pipe_communication: public I_ipc{
  public:
    typedef struct{
#ifdef _WIN64
      HANDLE pipe_write;
      HANDLE pipe_read;
#endif
    } pipe_data;

  
  private:
    bool _pipe_ready = false;

    pipe_data _pout_data;
    pipe_data _pin_data;

#ifdef _WIN64
    DWORD _last_error;
#endif

  public:
    pipe_communication();
    ~pipe_communication();

    uint32_t read(char* buffer, uint32_t readlen) override;
    uint32_t write(const char* buffer, uint32_t writelen) override;

    bool get_pipe_data(pipe_data* p_pipe_data);

    bool create_pipe();
    void close_unnesessary_pipe();
    void close_all_pipes();

#ifdef _WIN64
    DWORD get_last_winapi_error();
#endif
};

#endif