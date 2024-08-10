#ifdef _WIN64
#include "Windows.h"
#endif

#include "stdio.h"



DWORD WINAPI _threadfunc(void *param){
  for(int i = 0; i < 3; i++){
    printf("second = %d\n", i+1);
    Sleep(1000);
  }

  printf("thread exited.\n");
}

int main(){
  printf("waiting for the thread in 5 sec...\n");
  
  HANDLE _thread = CreateThread(
    NULL,
    0,
    _threadfunc,
    NULL,
    0,
    NULL
  );

  Sleep(5000);
  DWORD errcode = WaitForSingleObject(_thread, INFINITE);

  printf("errcode: %d\nprogram exited.\n", errcode);
}