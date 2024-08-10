#ifndef LIB_DATA_HEADER
#define LIB_DATA_HEADER

#include "loader.h"
#include "communication.h"

typedef struct{
  pipe_communication IPC_conn;

#ifdef _WIN64
  PROCESS_INFORMATION child_proc_info;
#endif
} program_data;

#endif