#include "vector"

int main(){
  int* test = new int();
  delete test;

  int* test1 = new int[2];
  delete[] test1;

  std::vector<int> intdata;
  intdata.insert(intdata.begin(), 1);
  intdata.insert(intdata.end(), 10);
}