#include "windows.h"

#include "loader.h"

#include "string"
#include "sstream"



extern "C"{
  bool __declspec(dllexport) __stdcall compile_code(compile_info *c_info){
    INIT_ASSERT()

#ifdef _WIN64
    std::stringstream _log_msg;
    std::stringstream _concat_str;

    bool _res = true;
    bool _winapi_resb;
    char* _cmd_param = (char*)malloc(0);
    
    STARTUPINFOA _startinfo; ZeroMemory(&_startinfo, sizeof(STARTUPINFOA));
    PROCESS_INFORMATION _pinfo; ZeroMemory(&_pinfo, sizeof(PROCESS_INFORMATION));

    _startinfo.cb = sizeof(STARTUPINFOA);
    _startinfo.dwFlags = STARTF_FORCEOFFFEEDBACK | STARTF_USESTDHANDLES;
    _startinfo.hStdOutput = c_info->stdout_handle;
    _startinfo.hStdError = c_info->stdout_handle;

    _pinfo.hProcess = INVALID_HANDLE_VALUE;

    // compiling to .o file(s)
    _concat_str.str(""); _concat_str <<
      COMPILER_PROGRAM_NAME " " COMPILER_COMPILE_OPTION;

    for(auto _file: c_info->file_array)
      _concat_str << " " << _file; 
    
    _cmd_param = (char*)realloc(_cmd_param, _concat_str.tellg());
    memcpy(_cmd_param, _concat_str.str().c_str(), _concat_str.tellg());
    _winapi_resb = CreateProcessA(
      NULL,
      _cmd_param,
      NULL,
      NULL,
      true,
      CREATE_NO_WINDOW,
      NULL,
      c_info->folder_dir.c_str(),
      &_startinfo,
      &_pinfo
    );

    if(_winapi_resb){
      WaitForSingleObject(_pinfo.hProcess, INFINITE);

      DWORD _exitcode = 0;
      GetExitCodeProcess(&_pinfo.hProcess, &_exitcode);

      if(_exitcode != 0){
        _log_msg.str(""); _log_msg <<
          "Error when compiling program.";

        goto log_compiler_error;
      }
    }
    else{
      _log_msg.str(""); _log_msg <<
        "Cannot create process for compiler. Error code: " <<
        GetLastError();

      goto log_internal_error;
    }


    // compiling to .exe file
    _concat_str.str(""); _concat_str <<
      COMPILER_PROGRAM_NAME " " COMPILER_OUTPUT_OPTION " " <<
      c_info->output_dir << "/" << c_info->program_name;
      
#ifdef COMPILER_USE_WILDCARD
    // if compiler can use wildcard
    _concat_str <<
      " " WILDCARD_LINKING_OBJECT;
#else
    // if not using wildcard, then iterate all files with .o
    for(auto _file: c_info->file_array){
      size_t _dotidx = _file.rfind(".");
      if(_dotidx != std::string::npos)
        _concat_str << " " << _file.erase(0, _dotidx+1) << COMPILER_COMPILE_RESULT_EXT;
    }
#endif // COMPILER_USE_WILDCARD

    _cmd_param = (char*)realloc(_cmd_param, _concat_str.tellg());
    memcpy(_cmd_param, _concat_str.str().c_str(), _concat_str.tellg());
    _winapi_resb = CreateProcessA(
      NULL,
      _cmd_param,
      NULL,
      NULL,
      true,
      CREATE_NO_WINDOW,
      NULL,
      c_info->folder_dir.c_str(),
      &_startinfo,
      &_pinfo
    );

    if(_winapi_resb){
      WaitForSingleObject(_pinfo.hProcess, INFINITE);

      DWORD _errorcode;
      GetExitCodeProcess(_pinfo.hProcess, &_errorcode);
      if(_errorcode != 0){
        _log_msg.str(""); _log_msg <<
          "Error when linking compiled codes.";
        
        goto log_compiler_error;
      }
    }
    else{
      _log_msg.str(""); _log_msg <<
        "Cannot create process for compiler. Error code: " <<
        GetLastError();

      goto log_internal_error;
    }
    
    // no error, skip to safe_return
    goto safe_return;



    log_internal_error:{
      LOG_ASSERT_LINE(get_logger(), I_logger::ST_ERROR, _log_msg.str().c_str());
      // continuing to log_compiler_error
    }

    log_compiler_error:{
      DWORD _writelen = 0;

      // writing seperator
      WriteFile(
        c_info->stdout_handle,
        c_info->separator_msg,
        c_info->separator_msg_length,
        &_writelen,
        NULL
      );

      // writing message to game
      WriteFile(
        c_info->stdout_handle,
        _log_msg.str().c_str(),
        _log_msg.tellg(),
        &_writelen,
        NULL
      );

      _res = false;
    }

    safe_return:{
      if(_pinfo.hProcess != INVALID_HANDLE_VALUE){
        if(TerminateProcess(_pinfo.hProcess, 0))
          WaitForSingleObject(_pinfo.hProcess, INFINITE);
      }

      free(_cmd_param);
    }

    return _res;
#endif // _WIN64
  }


  bool __declspec(dllexport) __stdcall check_language_readiness(){
    INIT_ASSERT()
  
#ifdef _WIN64
    const int _bufferlen = 512;

    bool _res = true;
    // + 1 is for null-terminator
    char _buffer[_bufferlen+1];

    bool _winapi_resb;
    std::stringstream _log_msg;

    STARTUPINFOA _startinfo; ZeroMemory(&_startinfo, sizeof(STARTUPINFOA));
    SECURITY_ATTRIBUTES _p_sec_attr; ZeroMemory(&_p_sec_attr, sizeof(SECURITY_ATTRIBUTES));
    PROCESS_INFORMATION _pinfo; ZeroMemory(&_pinfo, sizeof(PROCESS_INFORMATION));
    _pinfo.hProcess = INVALID_HANDLE_VALUE;

    HANDLE
      _pchildout_write = INVALID_HANDLE_VALUE,
      _pchildout_read = INVALID_HANDLE_VALUE;

    HANDLE
      _pchildin_write = INVALID_HANDLE_VALUE,
      _pchildin_read = INVALID_HANDLE_VALUE;

    std::string _output_str = "";

    _p_sec_attr.nLength = sizeof(SECURITY_ATTRIBUTES);
    _p_sec_attr.bInheritHandle = true;
    _winapi_resb = CreatePipe(&_pchildout_read, &_pchildout_write, &_p_sec_attr, _bufferlen);
    if(!_winapi_resb){
      _log_msg.str("");
        _log_msg << "Cannot create input pipe. Error code: ";
        _log_msg << GetLastError();
      LOG_ASSERT_LINE(get_logger(), I_logger::ST_ERROR, _log_msg.str().c_str())
      goto on_error;
    }

    _winapi_resb = CreatePipe(&_pchildin_read, &_pchildin_write, &_p_sec_attr, _bufferlen);
    if(!_winapi_resb){
      _log_msg.str("");
        _log_msg << "Cannot create input pipe. Error code: ";
        _log_msg << GetLastError();
      LOG_ASSERT_LINE(get_logger(), I_logger::ST_ERROR, _log_msg.str().c_str())
      goto on_error;
    }

    _startinfo.cb = sizeof(STARTUPINFOA);
    _startinfo.hStdInput = _pchildin_read;
    _startinfo.hStdOutput = _pchildout_write;
    _startinfo.hStdError = _pchildout_write;
    _startinfo.dwFlags =
      STARTF_FORCEOFFFEEDBACK |
      STARTF_USESTDHANDLES;

    _winapi_resb = CreateProcessA(
      NULL,
      COMPILER_PROGRAM_NAME " " COMPILER_VERSION_OPTION,
      NULL,
      NULL,
      true,
      CREATE_NO_WINDOW,
      NULL,
      NULL,
      &_startinfo,
      &_pinfo
    );
    if(!_winapi_resb){
      _log_msg.str("");
        _log_msg << "Cannot create process for compiler. Error code: ";
        _log_msg << GetLastError();
      LOG_ASSERT_LINE(get_logger(), I_logger::ST_ERROR, _log_msg.str().c_str())
      goto on_error;
    }

    CloseHandle(_pchildin_read);
    _pchildin_read = INVALID_HANDLE_VALUE;

    CloseHandle(_pchildout_write);
    _pchildout_write = INVALID_HANDLE_VALUE;

    while(true){
      DWORD _recvlen = 0;
      bool _success = ReadFile(
        _pchildout_read,
        _buffer,
        _bufferlen,
        &_recvlen,
        NULL
      );

      if(_recvlen > 0){
        _buffer[_recvlen] = '\0';
        _output_str += _buffer;
        
        LOG_ASSERT_LINE(get_logger(), I_logger::ST_LOG, _buffer)
      }
      else
        break;
    }

    { // then check the output
      LOG_ASSERT(get_logger(), I_logger::ST_LOG, "Compiler version output:");
      LOG_ASSERT(get_logger(), I_logger::ST_LOG, _output_str.c_str());
    goto safe_return;}

    on_error:{
      _res = false;
    }
    
    safe_return:{
      if(_pinfo.hProcess != INVALID_HANDLE_VALUE){
        if(TerminateProcess(_pinfo.hProcess, 0))
          WaitForSingleObject(_pinfo.hProcess, INFINITE);
      }

      if(_pchildout_read != INVALID_HANDLE_VALUE)
        CloseHandle(_pchildout_read);
      
      if(_pchildout_write != INVALID_HANDLE_VALUE)
        CloseHandle(_pchildout_write);

      if(_pchildin_read != INVALID_HANDLE_VALUE)
        CloseHandle(_pchildin_read);

      if(_pchildin_write != INVALID_HANDLE_VALUE)
        CloseHandle(_pchildin_write);
    }

    return _res;
#endif
  }
}