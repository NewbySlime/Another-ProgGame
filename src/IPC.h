#ifndef IPC_HEADER
#define IPC_HEADER

#include "stdint.h"


namespace GameUtils{
  class I_ipc{
    public:
      virtual uint32_t read(char* sbuffer, uint32_t readlen){ return 0; }
      virtual uint32_t write(const char* buffer, uint32_t writelen){ return 0; }
  };
}

#endif