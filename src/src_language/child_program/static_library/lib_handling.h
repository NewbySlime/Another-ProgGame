#ifndef LIB_HANDLING_HEADER
#define LIB_HANDLING_HEADER

#ifdef _WIN64
#include "Windows.h"
#endif

enum library_function_enum{
  initialize_communication,
  communication_read,
  communication_read_clear,
  communication_write,

  __library_function_enum_length
};


bool bypass_call_library_function(library_function_enum func, void *parameter, void *result);
bool call_library_function(library_function_enum func, void *parameter, void *result);

#endif