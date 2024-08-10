#ifndef UNSAFE_HEADER
#define UNSAFE_HEADER

#ifdef _WIN64
#include "windows.h"

#define START_UNSAFE_AREA \
  HANDLE ___unsafe_mutex = _unsafe_get_mutex(); \
  start_unsafe(___unsafe_mutex);

#define END_UNSAFE_AREA \
  end_unsafe(___unsafe_mutex);

HANDLE _unsafe_get_mutex();

void start_unsafe(HANDLE _mutex);
void end_unsafe(HANDLE _mutex);
#endif

#endif