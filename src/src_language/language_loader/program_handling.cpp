#include "windows.h"

#include "loader.h"
#include "lib_data.h"

#include "sstream"


extern "C"{
  bool __declspec(dllexport) __stdcall run_code(process_info *p_info){
    INIT_ASSERT()

    program_data *_prog_data = (program_data*)p_info->_lib_obj;

    _prog_data->IPC_conn = pipe_communication();
    pipe_communication::pipe_data _p_data;

    std::stringstream _log_msg;
  
#ifdef _WIN64
    bool _res = true;
    bool _winapi_resb;

    STARTUPINFOA _startinfo; ZeroMemory(&_startinfo, sizeof(STARTUPINFOA));
    ZeroMemory(&_prog_data->child_proc_info, sizeof(PROCESS_INFORMATION));

    _winapi_resb = _prog_data->IPC_conn.create_pipe();
    if(!_winapi_resb){
      _log_msg.str(""); _log_msg <<
        "Cannot create pipe for IPC. Error code: " <<
        _prog_data->IPC_conn.get_last_winapi_error();

      goto on_error;
    }

    _prog_data->IPC_conn.get_pipe_data(&_p_data);

    _startinfo.cb = sizeof(STARTUPINFOA);
    _startinfo.dwFlags = 
      STARTF_FORCEOFFFEEDBACK |
      STARTF_USESTDHANDLES;
    _startinfo.hStdInput = _p_data.pipe_read;
    _startinfo.hStdOutput = _p_data.pipe_write;
    _startinfo.hStdError = p_info->query_pipe_write;

    _winapi_resb = CreateProcessA(
      p_info->program_name.c_str(),
      NULL,
      NULL,
      NULL,
      true,
      CREATE_NO_WINDOW,
      NULL,
      p_info->program_dir.c_str(),
      &_startinfo,
      &_prog_data->child_proc_info
    );
    if(!_winapi_resb){
      _log_msg.str(""); _log_msg <<
        "Cannot create child process. Error code: " <<
        GetLastError();
      
      goto on_error;
    }

    
    // then try to communicate with child process

    

    goto safe_return;

    on_error:{
      LOG_ASSERT_LINE(get_logger(), I_logger::ST_ERROR, _log_msg.str().c_str());
      _prog_data->IPC_conn.close_all_pipes();
      
      _res = false;
    }

    safe_return:{
      _prog_data->IPC_conn.close_unnesessary_pipe();
    }

    return _res;
#endif
  }


  bool 


  I_ipc* __declspec(dllexport) __stdcall get_io(process_info *p_info){
    return &((program_data*)p_info->_lib_obj)->IPC_conn;
  }
}