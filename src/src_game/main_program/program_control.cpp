#include "Windows.h"
#include "program_control.h"

#include "../defines.h"
#include "../../communication_header.h"
#include "logger.h"

#include "godot_cpp/classes/engine.hpp"

#include "sstream"
#include "memory"


#define __RESERVED_LOW_ID     0x10
#define __RESERVED_DUMP_ID    0x0


using namespace godot;
using namespace GameUtils;
using namespace GameUtils::InterProcessCommunication;


void program_control::_clear_class(){
  if(!_query_thread){
    // stop the thread and wait for it to join/stop
    _query_thread_keep_listening = false;
    _query_thread_send_dump_header();

    WaitForSingleObject(_query_thread, INFINITE);
    CloseHandle(_query_thread);
  }

  if(!_querypipe_mutex)
    CloseHandle(_querypipe_mutex);

  if(_querypipe_read != INVALID_HANDLE_VALUE)
    CloseHandle(_querypipe_read);

  if(_querypipe_write != INVALID_HANDLE_VALUE)
    CloseHandle(_querypipe_write);
}

void program_control::_query_thread_send_dump_header(){
  header_struct _dump_header; ZeroMemory(&_dump_header, sizeof(header_struct));
  initialize_header(&_dump_header);

  _dump_header.identifier = __RESERVED_DUMP_ID;

  DWORD _writelen;
  WriteFile(
    _querypipe_write,
    &_dump_header,
    sizeof(header_struct),
    &_writelen,
    NULL
  );
}


DWORD WINAPI program_control::_query_thread_init_func(void* parameter){
  program_control* _pc = (program_control*)parameter;
  return _pc->_query_thread_actual_func();
}

DWORD program_control::_query_thread_actual_func(){
  INIT_ASSERT()
  __func_name = "query_reader";

  std::stringstream _log_str;

  _query_thread_keep_listening = true;
  while(_query_thread_keep_listening){
    header_struct _header_comm;

    DWORD _read_len;
    bool _success;
    _success = ReadFile(
      _querypipe_read,
      &_header_comm,
      sizeof(header_struct),
      &_read_len,
      NULL
    );

    if(!_success){
      _log_str.str("");
      _log_str << "Cannot read query pipes. WINAPI error: " << GetLastError();
      
      goto on_error;
    }
    else if(_read_len != sizeof(header_struct) || !check_header(&_header_comm))
      continue;

    if(_header_comm.datalen > __CHILD_MAXIMUM_SEND_LENGTH){
      // if the child program sent a data that is more than the program da

    }

    auto _iter = _respondent_map.find(_header_comm.identifier);
    if(_iter == _respondent_map.end()){
      // no respondent registered
    }

    void* _buffer = std::malloc(_header_comm.datalen);
    _success = ReadFile(
      _querypipe_read,
      _buffer,
      _header_comm.datalen,
      &_read_len,
      NULL
    );

    if(_read_len != _header_comm.datalen){
      // not enough data while reading
    }
    else
      _iter->second->on_header_received(&_header_comm, _buffer);


    continue;
    on_error:{
      LOG_ASSERT(String(_log_str.str().c_str()))
    }
  }

  return 0;
}


void program_control::_bind_methods(){

}


program_control::program_control(){
  _query_thread = NULL;

  _querypipe_mutex = INVALID_HANDLE_VALUE;
  _querypipe_read = INVALID_HANDLE_VALUE;
  _querypipe_write = NULL;


  INIT_ASSERT()
  std::stringstream _logstr;

  bool _success;
  Engine* _engine = Engine::get_singleton();
  if(!_engine->is_editor_hint()){
    SECURITY_ATTRIBUTES _sec_attr; ZeroMemory(&_sec_attr, sizeof(SECURITY_ATTRIBUTES));
    _sec_attr.nLength = sizeof(SECURITY_ATTRIBUTES);
    _sec_attr.bInheritHandle = true;
    _sec_attr.lpSecurityDescriptor = NULL;

    _success = CreatePipe(
      &_querypipe_read,
      &_querypipe_write,
      &_sec_attr,
      __QUERY_PIPE_BUFFER_LENGTH
    );

    if(!_success){
      _logstr.str("");
      _logstr << "Cannot create pipe. WINAPI Error: " << GetLastError();

      goto on_error;
    }

    HANDLE _qpipe_mutex = CreateMutexA(
      &_sec_attr,
      false,
      NULL
    );

    if(!_qpipe_mutex){
      _logstr.str("");
      _logstr << "Cannot create mutex. WINAPI Error: " << GetLastError();

      goto on_error;
    }
    else
      _querypipe_mutex = _qpipe_mutex;
  }


  on_error:{
    _clear_class();

    LOG_ASSERT(String(_logstr.str().c_str()))
  }

  skip_on_error:{}
}

program_control::~program_control(){
  _clear_class();
}


void program_control::_ready(){
  // creating thread for processing query
  _query_thread = CreateThread(
    NULL,
    0,
    _query_thread_init_func,
    this,
    0,
    NULL
  );

  if(!_query_thread){

  }
}


bool program_control::compile_project_folder(String project_folder){

}

uint32_t program_control::create_program_project(String project_folder){

}


bool program_control::suspend_program(uint32_t program_ID){
  
}

bool program_control::resume_program(uint32_t program_ID){

}


bool program_control::register_respondent(uint32_t program_ID, I_child_respondent* respondent){
  auto _iter = _respondent_map.find(program_ID);
  if(_iter == _respondent_map.end()){
    _respondent_map[program_ID] = respondent;
    return true;
  }

  return false;
}