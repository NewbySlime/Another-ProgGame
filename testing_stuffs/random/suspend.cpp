#include "iostream"

using namespace std;

int main(){
  int test;
  int *test1 = new int();

  printf("0x%X 0x%X", (unsigned long long)&test, (unsigned long long)test1);
  delete test1;
}