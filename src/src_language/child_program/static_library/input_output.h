#ifndef INPUT_OUTPUT_HEADER
#define INPUT_OUTPUT_HEADER

#ifdef _WIN64
#include "windows.h"
#endif

typedef struct{
#ifdef _WIN64
  
#endif
} init_io_param;


bool init_io(init_io_param param);



#endif