#include "input_output.h"
#include "stdio.h"


bool _io_init = false;




bool init_io(init_io_param param){
  if(!_io_init){
    

    return _io_init;
  }
  else{
    return false;
  }
}





int print(const char *str){
  DWORD _writelen;
  WriteFile(_file_stdout, str, strlen(str), &_writelen, NULL);
}