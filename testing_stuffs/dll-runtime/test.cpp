#include "stdio.h"
#include "testclass.h"


extern "C"{
  void __declspec(dllexport) __stdcall init_func(test_virtual *_test){
    printf("initiating dll...\n");
    printf("base member: %d\n", test_virtual().test());
    printf("virtual number: %d\n", _test->test());
  }

  const char* __declspec(dllexport) test_func(){
    return "string from dll";
  }

  void __declspec(dllexport) __stdcall deinit_func(){
    printf("deinitiating dll...\n");
  }
};