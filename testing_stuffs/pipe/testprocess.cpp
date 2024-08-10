#ifdef _WIN64
#include "Windows.h"
#endif

#include "stdio.h"
#include "stdlib.h"


int main(){
  HANDLE 
    hInput = GetStdHandle(STD_INPUT_HANDLE),
    hOutput = GetStdHandle(STD_OUTPUT_HANDLE);

  if(hInput == INVALID_HANDLE_VALUE || hOutput == INVALID_HANDLE_VALUE)
    exit(-1);

  char buffer[512];
  char writebuffer[512];
  while(true){
    DWORD _bytesRead;
    ReadFile(
      hInput,
      buffer,
      512,
      &_bytesRead,
      NULL
    );
    
    sprintf(writebuffer, "data read: %d, data msg: '%s'", _bytesRead, buffer);
    WriteFile(
      hOutput,
      writebuffer,
      512,
      &_bytesRead,
      NULL
    );

    if(buffer[0] == '!')
      break;
  }
}