#ifndef COMMUNICATION_HEADER_HEADER
#define COMMUNICATION_HEADER_HEADER

#include "stdint.h"
#include "memory.h" 


// if optcode in the header has HEADER_OPT_CODE, the game then gives the header to language_loader first, then process it itself
// by this sequence, between language_loader and child program can use own code and buffer (if possible), which then should be translated to game standard
// this code will tell the game that the child process had to notify internal stuff (error, quit, or something)
#define HEADER_OPT_CODE_BITFLAG 0x80000000



namespace GameUtils{
  namespace InterProcessCommunication{
    const uint8_t __headercode_length = 4;
    const uint8_t __headercode[__headercode_length] = {'H', 'C', 'D', '\x1b'};

    enum GameCodeEnum{
      // when child program writes to standard output
      gce_console_output_buffer = 0x01,
      gce_console_output_notify_file = 0x02,

      // when game gives input from player to child program
      gce_console_input_buffer = 0x03,
      gce_console_input_notify_file = 0x04
    };

    enum OptCodeEnum {
      // this can be processed by the language_loader (if not blocking) or by the game
      opce_child_ready = HEADER_OPT_CODE_BITFLAG | 0x01,

      // this code is sent by the language_loader, not by the game
      opce_child_init_data = HEADER_OPT_CODE_BITFLAG | 0x02,
      opce_child_string_init_data = HEADER_OPT_CODE_BITFLAG | 0x03
    };

    struct header_struct{
      public:
        const uint8_t _reserved[__headercode_length] = {0,0,0,0};
        
        // if data sent from main program, this variable is ignored.
        uint32_t identifier;

        uint32_t optcode;

        // the size of the data being sent
        uint32_t datalen;
    };


    void initialize_header(header_struct* headerdata){
      void* raw_data = headerdata;
      memcpy(raw_data, __headercode, __headercode_length);
    }

    uint8_t *object_as_rawdata(header_struct* headerdata){
      return reinterpret_cast<uint8_t*>(headerdata);
    }

    header_struct *rawdata_as_object(void* rawdata){
      return reinterpret_cast<header_struct*>(rawdata);
    }

    bool check_header(header_struct *headerdata){
      uint8_t *raw_data = object_as_rawdata(headerdata);

      for(int i = 0; i < __headercode_length; i++){
        if(raw_data[i] != __headercode[i])
          return false;
      }

      return true;
    }
  }
};


#endif