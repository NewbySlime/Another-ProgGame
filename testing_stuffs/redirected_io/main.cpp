#ifdef _WIN64
#include "Windows.h"
#endif

#include "stdio.h"
#include "stdlib.h"
#include "conio.h"

#include "iostream"


int main(){
  FILE *_stdinfile = fopen("testin.txt", "w");
  fprintf(_stdinfile, "test");
  fflush(_stdinfile);

  freopen("testin.txt", "r", stdin);

  char buffer[512];
  fscanf(stdin, "%s", buffer);
  printf("data from stdin: %s\n", buffer);

  _getch(); 
  //freopen("testout.txt", "rw", stdout);
  //freopen("testerr.txt", "rw", stderr);

  fseek(_stdinfile, 0, SEEK_SET);
  fprintf(_stdinfile, "writingtostdin\n");
  fflush(_stdinfile);

  std::cout << "output from cout" << std::endl;

  fseek(stdin, 0, SEEK_SET);

  fscanf(stdin, "%s", buffer);
  printf("data from stdin: %s\n", buffer);
}