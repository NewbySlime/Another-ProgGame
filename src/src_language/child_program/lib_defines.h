#ifndef LIB_DEFINES_HEADER
#define LIB_DEFINES_HEADER

#define _LIB_GET_FUNCTION_PARAMETER(function_name) \
  function_name##_parameter *_param = (function_name##_parameter*)param; \
  function_name##_result *_result = (function_name##_result*)result;

#endif