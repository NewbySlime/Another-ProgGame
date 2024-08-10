#ifndef ROBOT_LOG_HEADER
#define ROBOT_LOG_HEADER


#include "stdint.h"

namespace Game{
  namespace File{
    namespace StrictSize{
      const uint8_t __file_start_code_length = 4;
      const uint8_t __file_start_code[__file_start_code_length] = {'\x1b', '\x00', 'S', '\n'};

      const uint8_t __file_end_code_length = 4;
      const uint8_t __file_end_code[__file_end_code_length] = {'\x1b', '\x00', 'E', '\n'};
    }
  }
}

#endif