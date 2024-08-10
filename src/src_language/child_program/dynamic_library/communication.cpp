#ifdef _WIN64
#include "windows.h"
#endif

#include "../../../opt_code_data.h"
#include "../communication.h"



bool _initialized = false;
uint32_t _current_programID;


#ifdef _WIN64

HANDLE
  h_stdout,
  h_stdin,
  h_query;

HANDLE
  comm_mutex_rd,
  comm_mutex_wr;

// acquire mutex from game, not create it by itself
// as the query pipe will be used by all program
HANDLE comm_mutex_query;


bool __stdcall initialize_communication(void *param, void *result){
  if(_initialized)
    return false;

  _initialized = false;
  _LIB_GET_FUNCTION_PARAMETER(initialize_communication);

  h_stdout = GetStdHandle(STD_OUTPUT_HANDLE);
  h_stdin = GetStdHandle(STD_INPUT_HANDLE);
  h_query = GetStdHandle(STD_ERROR_HANDLE);
  communication_read_parameter _cread_param; communication_read_result _cread_result;

  SECURITY_ATTRIBUTES _mutex_sec_attr; ZeroMemory(&_mutex_sec_attr, sizeof(SECURITY_ATTRIBUTES));
  _mutex_sec_attr.nLength = sizeof(SECURITY_ATTRIBUTES);
  _mutex_sec_attr.bInheritHandle = false;
  _mutex_sec_attr.lpSecurityDescriptor = NULL;

  // ignoring all data until 
  bool _keeplooping = true;
  communication_read(&_cread_param, &_cread_result);

  if(_cread_result.optcode == opce_child_init_data){
    if(_cread_result.buffer_length == sizeof(opcd_child_init_data)){
      opcd_child_init_data* _data = (opcd_child_init_data*)_cread_result.buffer;
      _current_programID = _data->program_ID;
      comm_mutex_query = _data->query_mutex;
    }
    else{
      // TODO error checking
      goto on_error;
    }
  }
  else{
    // TODO error checking
    goto on_error;
  }


  comm_mutex_rd = CreateMutex(&_mutex_sec_attr, false, NULL);
  comm_mutex_wr = CreateMutex(&_mutex_sec_attr, false, NULL);

  _initialized = true;
  goto clear;

  on_error:{

  }

  clear:{
    communication_read_clear_parameter _c_param; communication_read_clear_result _c_result;
    _c_param.result = &_cread_result;

    communication_read_clear(&_c_param, &_c_result);
  }

  return _initialized;
}


bool __stdcall communication_read(void *param, void *result){
  if(!_initialized)
    return false;

  // initializing function variables
  _LIB_GET_FUNCTION_PARAMETER(communication_read)

  _result->buffer = (char*)malloc(0);
  _result->buffer_length = 0;

  header_struct _header;

  DWORD _recvlen;
  bool _success = false;


  // do reading
  WaitForSingleObject(comm_mutex_rd, INFINITE);
  while(true){
    _success = ReadFile(
      h_stdin,
      &_header,
      sizeof(_header),
      &_recvlen,
      NULL
    );

    if(!_success || _recvlen != sizeof(_header)){
      // TODO error checking
      goto on_error;
    }

    _success = check_header(&_header);
    if(!_success){
      // TODO error checking

      // reading until header is correct
      continue;
    }

    _result->optcode = _header.optcode;

    _result->buffer = (char*)realloc(_result->buffer, _header.datalen);
    _result->buffer_length = _header.datalen;
    _success = ReadFile(
      h_stdin,
      _result->buffer,
      _result->buffer_length,
      &_recvlen,
      NULL
    );

    if(!_success || _recvlen != _result->buffer_length){
      // TODO error checking
      goto on_error;
    }


    _success = true;
    goto skip_error;
  }

  on_error:{
    _success = false;
  }

  skip_error:{
    ReleaseMutex(comm_mutex_rd);
  }

  return _success;
}

bool __stdcall communication_read_clear(void *param, void *result){
  if(!_initialized)
    return false;

  _LIB_GET_FUNCTION_PARAMETER(communication_read_clear);
  free(_param->result->buffer);

  return true;
}


bool __stdcall communication_write(void *param, void *result){
  if(!_initialized)
    return false;

  // initialize function variables
  _LIB_GET_FUNCTION_PARAMETER(communication_write);
  _result->write_length = 0;

  header_struct _header; initialize_header(&_header);
  _header.identifier = _current_programID;
  _header.optcode = _param->optcode;
  _header.datalen = _param->data_length;

  bool _success = false;
  DWORD _writelen;


  // do writing
  WaitForSingleObject(comm_mutex_wr, INFINITE);

  WaitForSingleObject(comm_mutex_query, INFINITE);
  _success = WriteFile(
    h_query,
    &_header,
    sizeof(_header),
    &_writelen,
    NULL
  );
  ReleaseMutex(comm_mutex_query);

  if(!_success || _writelen != sizeof(_header)){
    // TODO error checking
    goto on_error;
  }

  // makes sure that buffer is not null and buffer length is greater than zero
  _writelen = 0;
  if(_param->data && _param->data_length){
    _success = WriteFile(
      h_stdout,
      _param->data,
      _param->data_length,
      &_writelen,
      NULL
    );

    if(!_success || _writelen != _param->data_length){
      // TODO error checking
      goto on_error;
    }
  }

  _result->write_length = _writelen;

  _success = true;
  goto skip_error;

  on_error:{
    _success = false;
  }

  skip_error:{
    ReleaseMutex(comm_mutex_wr);
  }

  return _success;
}

#endif