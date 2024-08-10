#ifdef _WIN64
#include "Windows.h"
#endif

#include "WinBase.h"
#include "processthreadsapi.h"

#include "malloc.h"
#include "stdio.h"


int main(){
  HANDLE hInput_Read, hInput_Write, hOutput_Read, hOutput_Write;

  SECURITY_ATTRIBUTES hStdSecAttr; ZeroMemory(&hStdSecAttr, sizeof(SECURITY_ATTRIBUTES));
  hStdSecAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
  hStdSecAttr.bInheritHandle = true;
  hStdSecAttr.lpSecurityDescriptor = NULL;

  // creating STD IO Handles
  bool _res = CreatePipe(
    &hInput_Read,
    &hInput_Write,
    &hStdSecAttr,
    0
  );

  if(!_res){
    printf("Something went wrong when creating pipe.\n");
    exit(-1);
  }

  _res = SetHandleInformation(hInput_Write, HANDLE_FLAG_INHERIT, 0);
  
  if(!_res){
    printf("Something went wrong when creating pipe.\n");
    exit(-1);
  }


  _res = CreatePipe(
    &hOutput_Read,
    &hOutput_Write,
    &hStdSecAttr,
    0
  );

  if(!_res){
    printf("Something went wrong when creating pipe.\n");
    exit(-1);
  }
  
  _res = SetHandleInformation(hOutput_Read, HANDLE_FLAG_INHERIT, 0);
  
  if(!_res){
    printf("Something went wrong when creating pipe.\n");
    exit(-1);
  }


  // setting startup information
  STARTUPINFO _defaultStartInfo; ZeroMemory(&_defaultStartInfo, sizeof(STARTUPINFO));
  _defaultStartInfo.cb = sizeof(STARTUPINFO);
  _defaultStartInfo.hStdOutput = hOutput_Write;
  _defaultStartInfo.hStdError = hOutput_Write;
  _defaultStartInfo.hStdInput = hInput_Read;
  _defaultStartInfo.dwFlags |= STARTF_USESTDHANDLES;

  PROCESS_INFORMATION _procInfo; ZeroMemory(&_procInfo, sizeof(PROCESS_INFORMATION));
  bool success = CreateProcess(
    "testprocess.exe",
    NULL,
    NULL,
    NULL,
    true,
    0,
    NULL,
    NULL,
    &_defaultStartInfo,
    &_procInfo
  );

  if(!success){
    printf("Cannot create process.\n");
    exit(-1);
  }


  CloseHandle(_procInfo.hProcess);
  CloseHandle(_procInfo.hThread);

  CloseHandle(hOutput_Write);
  CloseHandle(hInput_Read);


  DWORD _bytesWrite;
  char buffer[512];
  for(int i = 0; i < 5; i++){
    sprintf(buffer, "test");

    printf("writing data\n");
    _res = WriteFile(
      hInput_Write,
      buffer,
      512,
      &_bytesWrite,
      NULL
    );

    if(!_res){
      printf("write fail.\n");
      break;
    }

    printf("data written: %d\n\n", _bytesWrite);

    printf("reading data\n");
    DWORD _bytesRead;
    _res = ReadFile(
      hOutput_Read,
      buffer,
      512,
      &_bytesRead,
      NULL
    );

    if(!_res){
      printf("read fail.\n");
      break;
    }

    printf("datalen from child: %d, msg: '%s'\n", _bytesRead, buffer);
    Sleep(1000);
  }

  WriteFile(
    hInput_Write,
    "!",
    1,
    &_bytesWrite,
    NULL
  );


  CloseHandle(hOutput_Read);
  CloseHandle(hOutput_Write);
  CloseHandle(hInput_Read);
  CloseHandle(hInput_Write);
}