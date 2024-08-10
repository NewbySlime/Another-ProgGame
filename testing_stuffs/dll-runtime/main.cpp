#include "windows.h"
#include "stdio.h"

#include "testclass.h"


typedef void (__stdcall *initfunc)(test_virtual*);
typedef void (__stdcall *deinitfunc)();

typedef const char* (__stdcall *testfunc)();


class test_class: public test_virtual{
  public:
    int test() override{
      printf("testing class\n");
      return 1024;
    }
};


int main(){
  printf("Start main function.\n");

  HMODULE _hmodule = LoadLibraryA("test.dll");
  if(_hmodule != NULL){
    initfunc _initfunc;
    deinitfunc _deinitfunc;
    test_class _tc;

    _initfunc = (initfunc)GetProcAddress(_hmodule, "init_func");
    if(_initfunc == NULL){
      printf("Cannot get function, err: 0x%X\n", GetLastError());
      goto cleanup_lib;
    }

    _deinitfunc = (deinitfunc)GetProcAddress(_hmodule, "deinit_func");
    if(_deinitfunc == NULL){
      printf("Cannot get function, err: 0x%X\n", GetLastError());
      goto cleanup_lib;
    }

    _initfunc(&_tc);
    _deinitfunc();


    cleanup_lib:
    FreeLibrary(_hmodule);
    printf("Freeing library.\n");
  }
  else{
    printf("Cannot open library\n");
  }
}