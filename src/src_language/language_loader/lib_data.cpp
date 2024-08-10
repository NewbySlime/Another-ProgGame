#include "lib_data.h"

extern "C"{
  bool __declspec(dllexport) __stdcall init_object_process_info(process_info* p_info){
    if(p_info->additional_data)
      return false;
    
    if(p_info->_lib_obj)
      return false;

    p_info->additional_data = (char*)malloc(0);
    p_info->additional_data_length = 0;

    p_info->_lib_obj = new program_data();

    return true;
  }

  bool __declspec(dllexport) __stdcall reallocate_process_info_additional_data(process_info* p_info, size_t mem_size){
    if(!p_info->additional_data)
      return false;

    p_info->additional_data = (char*)realloc(p_info->additional_data, mem_size);
    p_info->additional_data_length = mem_size;

    return true;
  }

  bool __declspec(dllexport) __stdcall deinit_object_process_info(process_info* p_info){
    if(!p_info->additional_data)
      return false;
    
    if(!p_info->_lib_obj)
      return false;

    free(p_info->additional_data);
    p_info->additional_data = NULL;
    p_info->additional_data_length = 0;

    program_data *p_pd = (program_data*)p_info->_lib_obj;
    delete p_pd;
    p_info->_lib_obj = NULL;

    return true;
  }
}