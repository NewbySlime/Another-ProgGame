#include "lib_handling.h"
#include "thread_handling.h"
#include "unsafe.h"
#include "../communication.h"


#include "map"
#include "vector"
#include "string"

#include "exception"

#include "../user_header/robot.h"


using namespace GameUtils::InterProcessCommunication;



typedef bool (__stdcall *__library_function)(void *param, void *result);

std::vector<__library_function> _lib_funcs;
std::vector<std::string> _lib_function_name = {
  "initialize_communication",
  "communication_read",
  "communication_read_clear",
  "communication_write"
};


enum _program_context{
  __pc_initialization,
  __pc_user_code
};

_program_context _pc_context;


void initialize_library_functions();
void deinitialize_library_functions();

void _program_start(){
  try{
    start();
    
    while(true)
      loop();
  }
  catch(std::exception e){

  }
}

int main(){
  try{
    _pc_context = __pc_initialization;

    initialize_library_functions();

    // initializing communication library
    {initialize_communication_parameter _param; initialize_communication_result _result;
      bool _success = bypass_call_library_function(initialize_communication, &_param, &_result);
    }

    // tell game that the child is ready
    {communication_write_parameter _param; communication_write_result _result;
      _param.data = NULL;
      _param.data_length = 0;
      _param.optcode = opce_child_ready;

      bool _success = bypass_call_library_function(communication_write, &_param, &_result);
    }

    thread_meta_info m_info;
      m_info.stack_size = 512;
    thread_data t_data = create_thread(&m_info, _program_start);

    bool _keepload = true;
    while(_keepload){
      bool _success;
      
      communication_read_parameter _param1; communication_read_result _result1;
      _success = bypass_call_library_function(communication_read, &_param1, &_result1);
      switch(_result1.optcode){
        break; case gce_console_input:{
          
        }
      }

      
      communication_read_clear_parameter _param2; communication_read_clear_result _result2;
      memcpy(&_param2.result, &_result1, sizeof(communication_read_result));
      _success = bypass_call_library_function(communication_read_clear, &_param2, &_result2);
    }
  }
  catch(std::exception e){
    
  }
}



#ifdef _WIN64
bool _library_handle_module_loaded = false;
HMODULE _library_handle_module;


void initialize_library_functions(){
  if(!_library_handle_module_loaded){
    _library_handle_module_loaded = true;

    _lib_funcs.resize(__library_function_enum_length);
    if(_lib_function_name.size() < __library_function_enum_length)
      throw std::exception("Library function name not enough.");

    HMODULE _library_handle_module = LoadLibraryA("lib_gameutils.dll");
    for(int i = 0; i < __library_function_enum_length; i++){
      __library_function _lfunc = (__library_function)GetProcAddress(_library_handle_module, _lib_function_name[i].c_str());

      _lib_funcs[i] = _lfunc;
    }
  }
}

void deinitialize_library_functions(){
  if(_library_handle_module_loaded){
    _library_handle_module_loaded = false;

    FreeLibrary(_library_handle_module);
  }
}


bool bypass_call_library_function(library_function_enum func, void *parameter, void *result){
  if(func < __library_function_enum_length)
    return _lib_funcs[func](parameter, result);

  return false;
}

bool call_library_function(library_function_enum func, void *parameter, void *result){
  bool _result;
START_UNSAFE_AREA
  _result = bypass_call_library_function(func, parameter, result);
END_UNSAFE_AREA
  return _result;
}

#endif