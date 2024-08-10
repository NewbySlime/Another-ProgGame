#ifndef GAME_CODE_DATA_HEADER
#define GAME_CODE_DATA_HEADER
// this code contains data for each game_code, that can be used between game and child program

#include "stdint.h"


typedef struct{
  uint32_t buffer_length;
  char buffer[];
} gcd_console_input_buffer;

typedef struct{

} gcd_console_input_notify_file;

typedef struct{
  uint32_t buffer_length;
  char buffer[];
} gcd_console_output_buffer;

typedef struct{

} gcd_console_output_notify_file;

#endif