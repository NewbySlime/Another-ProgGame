#include "communication.h"


pipe_communication::pipe_communication(){
  _pout_data.pipe_read = INVALID_HANDLE_VALUE;
  _pout_data.pipe_write = INVALID_HANDLE_VALUE;
  
  _pin_data.pipe_read = INVALID_HANDLE_VALUE;
  _pin_data.pipe_write = INVALID_HANDLE_VALUE;
}

pipe_communication::~pipe_communication(){
  close_all_pipes();
}


uint32_t pipe_communication::read(char* buffer, uint32_t readlen){
  uint32_t _readlen = 0;

#ifdef _WIN64
  if(_pipe_ready){
    bool _success = ReadFile(
      _pout_data.pipe_read,
      buffer,
      readlen,
      (LPDWORD)&_readlen,
      NULL
    );

    if(!_success)
      _last_error = GetLastError();
  }
#endif

  return _readlen;
}

uint32_t pipe_communication::write(const char* buffer, uint32_t writelen){
  uint32_t _writelen = 0;

#ifdef _WIN64
  if(_pipe_ready){
    bool _success = WriteFile(
      _pin_data.pipe_write,
      buffer,
      writelen,
      (LPDWORD)&_writelen,
      NULL
    );

    if(!_success)
      _last_error = GetLastError();
  }
#endif
  
  return _writelen;
}


bool pipe_communication::get_pipe_data(pipe_data* p_pipe_data){
  if(_pipe_ready){
    p_pipe_data->pipe_read = _pin_data.pipe_read;
    p_pipe_data->pipe_write = _pout_data.pipe_write;

    return true;
  }
  else
    return false;
}

bool pipe_communication::create_pipe(){
  if(!_pipe_ready){
#ifdef _WIN64
    bool _res = true;
    SECURITY_ATTRIBUTES _sec_attr; ZeroMemory(&_sec_attr, sizeof(SECURITY_ATTRIBUTES));
    _sec_attr.nLength = sizeof(SECURITY_ATTRIBUTES);
    _sec_attr.bInheritHandle = true;

    _pout_data.pipe_read = INVALID_HANDLE_VALUE;
    _pout_data.pipe_write = INVALID_HANDLE_VALUE;
    _pin_data.pipe_read = INVALID_HANDLE_VALUE;
    _pout_data.pipe_write = INVALID_HANDLE_VALUE;

    if(!CreatePipe(&_pout_data.pipe_read, &_pout_data.pipe_write, &_sec_attr, PIPE_BUFFER_LENGTH)){
      goto on_error;
    }

    if(!CreatePipe(&_pin_data.pipe_read, &_pin_data.pipe_write, &_sec_attr, PIPE_BUFFER_LENGTH)){
      goto on_error;
    }

    _pipe_ready = true;
    goto skip_error;

    on_error:{
      close_all_pipes();

      _last_error = GetLastError();
      _res = false;
    }

    skip_error:{}
    return _res;
#endif
  }
  else
    return false;
}

void pipe_communication::close_unnesessary_pipe(){
  if(_pout_data.pipe_write != INVALID_HANDLE_VALUE){
    CloseHandle(_pout_data.pipe_write);
    _pout_data.pipe_write = INVALID_HANDLE_VALUE;
  }

  if(_pin_data.pipe_read != INVALID_HANDLE_VALUE){
    CloseHandle(_pin_data.pipe_read);
    _pin_data.pipe_read = INVALID_HANDLE_VALUE;
  }
}

void pipe_communication::close_all_pipes(){
  if(_pout_data.pipe_read != INVALID_HANDLE_VALUE){
    CloseHandle(_pout_data.pipe_read);
    _pout_data.pipe_read = INVALID_HANDLE_VALUE;
  }
  
  if(_pout_data.pipe_write != INVALID_HANDLE_VALUE){
    CloseHandle(_pout_data.pipe_write);
    _pout_data.pipe_write = INVALID_HANDLE_VALUE;
  }

  if(_pin_data.pipe_read != INVALID_HANDLE_VALUE){
    CloseHandle(_pin_data.pipe_read);
    _pin_data.pipe_read = INVALID_HANDLE_VALUE;
  }
  
  if(_pin_data.pipe_write != INVALID_HANDLE_VALUE){
    CloseHandle(_pin_data.pipe_write);
    _pin_data.pipe_write = INVALID_HANDLE_VALUE;
  }

  _pipe_ready = false;
}


#ifdef _WIN64
DWORD pipe_communication::get_last_winapi_error(){
  return _last_error;
}
#endif