#ifndef TEST_HEADER
#define TEST_HEADER

#ifdef DLL_EXPORTING
  #define DLL_DECLSPEC __declspec(dllexport)
#else
  #define DLL_DECLSPEC __declspec(dllimport)
#endif

DLL_DECLSPEC void testfunc();

#endif