#ifndef OPT_CODE_DATA_HEADER
#define OPT_CODE_DATA_HEADER
// This code contains data for each opt_code, that can be used between game and child program

#ifdef _WIN64
#include "windows.h"
#endif

#include "stdint.h"


typedef struct{
  uint32_t program_ID;
#ifdef _WIN64
  HANDLE query_mutex;
#endif
} opcd_child_init_data;

#endif