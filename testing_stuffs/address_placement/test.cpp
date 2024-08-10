#include "stdio.h"

int _test;

void test(){

}

int main(){
  printf("0x%X\n", &_test);
  printf("0x%X\n", main);
}