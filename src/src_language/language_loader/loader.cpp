#include "loader.h"
#include "lib_data.h"

#include "memory"
#include "map"
#include "string"
#include "sstream"




using namespace GameUtils;



I_logger *_static_logger;

I_logger *get_logger(){
  return _static_logger;
}


extern "C"{
  bool __declspec(dllexport) __stdcall init_lib(I_logger *log){
    INIT_ASSERT()

    _static_logger = log;
    
    LOG_ASSERT_LINE(log, I_logger::ST_LOG, "Initiating library...")
    LOG_ASSERT(log, I_logger::ST_ERROR, "This should be an error.")

    return true;
  }

  bool __declspec(dllexport) __stdcall deinit_lib(){
    INIT_ASSERT()
  }
  

  bool __declspec(dllexport) __stdcall init_object(process_info *p_info){
    if(!p_info->additional_data){
      p_info->additional_data = (char*)malloc(0);
      p_info->additional_data_length = 0;
    }

    if(!p_info->_lib_obj){
      p_info->_lib_obj = (program_data*)malloc(sizeof(program_data));
    }
  }

  bool __declspec(dllexport) __stdcall deinit_object(process_info *p_info){
    if(p_info->additional_data){
      free(p_info->additional_data); p_info->additional_data = NULL;
      p_info->additional_data_length = 0;
    }

    if(p_info->_lib_obj){
      free(p_info->_lib_obj); p_info->_lib_obj = NULL;
    }
  }
};