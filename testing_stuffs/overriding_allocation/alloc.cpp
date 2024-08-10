#include "malloc.h"
#include "stdio.h"



// look at:
// https://en.cppreference.com/w/cpp/memory/new/operator_new
// https://en.cppreference.com/w/cpp/memory/new/operator_delete

void* custom_malloc(size_t mem_size){
  printf("size %d\n", mem_size);
  return malloc(mem_size);
}

void custom_free(void *mem_addr){
  printf("freeing 0x%X\n", mem_addr);
  free(mem_addr);
}

void* operator new(size_t mem_size){
  return custom_malloc(mem_size);
}

void* operator new[](size_t mem_size){
  return custom_malloc(mem_size);
}

void operator delete(void* ptr, size_t mem_size) throw(){
  custom_free(ptr);
}

void operator delete[](void* ptr) throw(){
  custom_free(ptr);
}


