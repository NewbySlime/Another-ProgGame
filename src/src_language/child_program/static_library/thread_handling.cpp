#include "thread_handling.h"
#include "unsafe.h"

#include "map"

#ifdef _WIN64

struct _thread_data{
  public:
    HANDLE _thread_handle;
    HANDLE _thread_mutex;
};

std::map<DWORD, _thread_data> thread_data_list;

volatile bool _continue_mutex = true;


DWORD WINAPI _thread_start(LPVOID param){
  ((thread_function)param)();
}


bool create_thread(const thread_meta_info* meta_info, thread_function start_func, thread_data *data){
  if(data)
    ZeroMemory(data, sizeof(thread_data));

  _thread_data _t_data; ZeroMemory(&_t_data, sizeof(_thread_data));
  SECURITY_ATTRIBUTES thread_sec_attr; ZeroMemory(&thread_sec_attr, sizeof(SECURITY_ATTRIBUTES));
  SECURITY_ATTRIBUTES mutex_sec_attr; ZeroMemory(&mutex_sec_attr, sizeof(SECURITY_ATTRIBUTES));

  thread_sec_attr.nLength = sizeof(SECURITY_ATTRIBUTES);
  thread_sec_attr.bInheritHandle = true;

  mutex_sec_attr.nLength = sizeof(SECURITY_ATTRIBUTES);
  mutex_sec_attr.bInheritHandle = true;

  _t_data._thread_handle = CreateThread(
    &thread_sec_attr,
    meta_info->stack_size,
    _thread_start,
    start_func,
    CREATE_SUSPENDED,
    NULL
  );

  if(!_t_data._thread_handle){
    data->_error_code = GetLastError();

    goto on_winapi_error;
  }

  _t_data._thread_mutex = CreateMutex(
    &mutex_sec_attr,
    false,
    NULL
  );

  if(!_t_data._thread_mutex){
    data->_error_code = GetLastError();

    goto on_winapi_error;
  }

  thread_data_list[GetThreadId(_t_data._thread_handle)] = _t_data;
  ResumeThread(_t_data._thread_handle);
  
  goto skip_error;
  on_winapi_error:{
    if(_t_data._thread_handle){
      TerminateThread(_t_data._thread_handle, 0);
      CloseHandle(_t_data._thread_handle);
    }

    if(_t_data._thread_mutex)
      CloseHandle(_t_data._thread_mutex);

    return false;
  }

  skip_error:{
    if(data){
      data->thread_handle = _t_data._thread_handle;
    }

    return true;
  }
}

void suspend_all_thread(){
  if(!_continue_mutex)
    return;

  _continue_mutex = false;
  Sleep(1000);

  for(auto _td_iter = thread_data_list.begin(); _td_iter != thread_data_list.end(); _td_iter++){
    WaitForSingleObject(_td_iter->second._thread_mutex, INFINITE);
    SuspendThread(_td_iter->second._thread_handle);
  }
}

void resume_all_thread(){

}


HANDLE _unsafe_get_mutex(){
  DWORD _tid = GetCurrentThreadId();
  _thread_data &_td = thread_data_list[_tid];
  return _td._thread_mutex;
}


void start_unsafe(HANDLE _mutex){
  if(!_continue_mutex)
    Sleep(INFINITE);
  
  WaitForSingleObject(_mutex, INFINITE);
}

void end_unsafe(HANDLE _mutex){
  ReleaseMutex(_mutex);
}

#endif