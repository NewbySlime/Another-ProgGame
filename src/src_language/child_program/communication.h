#ifndef COMMUNICATION_HEADER
#define COMMUNICATION_HEADER

#include "lib_defines.h"
#include "../../communication_header.h"

using namespace GameUtils::InterProcessCommunication;


enum communication_type{
  comm_type_header,
  comm_type_data
};

// initialize_communication function
typedef struct{
} initialize_communication_parameter;

typedef struct{
} initialize_communication_result;


// communication_read function
typedef struct{
} communication_read_parameter;

typedef struct{
  uint32_t optcode;

  char *buffer;
  uint32_t buffer_length;
} communication_read_result;


// communication_read_clear function
typedef struct{
  communication_read_result* result;
} communication_read_clear_parameter;

typedef struct{
} communication_read_clear_result;


// communication_write function
typedef struct{
  uint32_t optcode;

  const char *data;
  uint32_t data_length;
} communication_write_parameter;

typedef struct{
  uint32_t write_length;
} communication_write_result;


#endif