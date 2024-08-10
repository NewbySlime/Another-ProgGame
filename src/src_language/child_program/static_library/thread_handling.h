#ifndef THREAD_HANDLING_HEADER
#define THREAD_HANDLING_HEADER

#include "stdint.h"

#ifdef _WIN64
#include "windows.h"
#endif


struct thread_data{
  public:
#ifdef _WIN64
    HANDLE thread_handle;
    DWORD _error_code;
#endif
};

struct thread_meta_info{
  public:
    uint32_t stack_size;
};

typedef void (*thread_function)();


thread_data create_thread(const thread_meta_info* meta_info, thread_function start_func);

// not including main thread
void suspend_all_thread();
void resume_all_thread();


#endif