#ifndef LIB_FUNC_HEADER
#define LIB_FUNC_HEADER

#include "I_logger.h"
#include "IPC.h"
#include "lib_param_structs.h"

#include "memory"

using namespace  GameUtils;

extern "C"{
  bool __declspec(dllexport) __stdcall init_lib(I_logger *log);
  bool __declspec(dllexport) __stdcall deinit_lib(I_logger *log);

  bool __declspec(dllexport) __stdcall run_code(I_logger *log, process_info *p_info);
  bool __declspec(dllexport) __stdcall compile_code(I_logger *log, process_info *p_info, compile_info *result);

  bool __declspec(dllexport) __stdcall check_language_readiness(I_logger *log);

  std::shared_ptr<I_ipc> __declspec(dllexport) __stdcall get_io(I_logger *log, process_info *p_info);
};

#endif