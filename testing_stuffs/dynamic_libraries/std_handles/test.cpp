#ifdef _WIN64
#include "Windows.h"
#endif

#include "stdio.h"

#include "test.h"


int _testint = 0;

void testfunc(){
  HANDLE stdout_hand = GetStdHandle(STD_OUTPUT_HANDLE);
  const char teststr[] = "test from dll.\n";

  DWORD writelen;
  WriteFile(
    stdout_hand,
    teststr,
    sizeof(teststr),
    &writelen,
    NULL
  );

  while(true){
    _testint++;

    printf("data: %d\n", _testint);
    Sleep(1000);
  }
}