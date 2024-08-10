#include "logger.h"
#include "../defines.h"

#include "chrono"

#include "godot_cpp/classes/engine.hpp"
#include "godot_cpp/classes/time.hpp"

#include "godot_cpp/variant/utility_functions.hpp"


// 0: Function name
// 1: Code line
#define FUNC_CONTEXT_FORMAT_FUNCNAME_LINE String("{0}:{1}")

// 0: Time string (HH-MM-SS)
// 1: msec string
#define TIME_STRING_FORMAT String("{0}.{1}")

// 0: Current time
// 1: Log message
#define LOGGING_FORMAT String("[INF][{0}] {1}")

// 0: Current time
// 1: Log message
#define WARNING_FORMAT String("[WRN][{0}] {1}")

// 0: Current time
// 1: Log message
#define ERROR_FORMAT String("[ERR][{0}] {1}")



using namespace GameUtils;


logger *_static_logger_obj = NULL;

String logger::_get_current_time(){
  using namespace std::chrono;

  Time *_time_obj = Time::get_singleton();
  auto _currtime = system_clock::now().time_since_epoch();

  auto _time_str = _time_obj->get_time_string_from_system();
  auto _time_milli = duration_cast<milliseconds>(_currtime).count() % 1000;

  Array _paramarr;{
    _paramarr.append(_time_str);
    _paramarr.append(_time_milli);
  }
  return TIME_STRING_FORMAT.format(_paramarr);
}

void logger::_bind_methods(){

}

void logger::_ready(){
  Engine *_engine = Engine::get_singleton();
  if(!_engine->is_editor_hint()){
    
  }

#ifdef _WIN64
  _log_mutex = CreateMutex(NULL, false, NULL);
#endif

  _static_logger_obj = this;
}

logger *logger::get_static_logger(){
  return _static_logger_obj;
}


void logger::print_log_static(const String &log){
  logger *_inst = get_static_logger();

  if(_inst)
    _inst->print_log(log);
}

void logger::print_warn_static(const String &warning){
  logger *_inst = get_static_logger();

  if(_inst)
    _inst->print_warn(warning);
}

void logger::print_err_static(const String &err){
  logger *_inst = get_static_logger();

  if(_inst)
    _inst->print_err(err);
}

void logger::print_log(const String &log){
  __LOCK_MUTEX(_log_mutex);
  Array _paramarr;{
    _paramarr.append(_get_current_time());
    _paramarr.append(log);
  }

  String _formattedlog = LOGGING_FORMAT.format(_paramarr);
  UtilityFunctions::print(_formattedlog);

  __RELEASE_MUTEX(_log_mutex);
}

void logger::print_warn(const String &warning){
  __LOCK_MUTEX(_log_mutex);
  Array _paramarr;{
    _paramarr.append(_get_current_time());
    _paramarr.append(warning);
  }

  String _formattedwarn = WARNING_FORMAT.format(_paramarr);
  UtilityFunctions::print(_formattedwarn);

  __RELEASE_MUTEX(_log_mutex); 
}

void logger::print_err(const String &err){
  __LOCK_MUTEX(_log_mutex);
  Array _paramarr;{
    _paramarr.append(_get_current_time());
    _paramarr.append(err);
  }

  String _formattederr = ERROR_FORMAT.format(_paramarr);
  UtilityFunctions::printerr(_formattederr);

  __RELEASE_MUTEX(_log_mutex);
}


void logger::print(std_type type, const String &msg){
  switch(type){
    break; case std_type::ST_LOG:
      print_log(msg);

    break; case std_type::ST_WARNING:
      print_warn(msg);

    break; case std_type::ST_ERROR:
      print_err(msg);
  }
}


void logger::print(std_type type, const char *msg){
  String _msgstr = String(msg);
  print(type, _msgstr);
}

void logger::print_func(std_type type, const char *func_name, const char *msg){
  Array _paramarr;{
    _paramarr.append(String(func_name));
    _paramarr.append(String(msg));
  }
  String _msgstr = String("{0} {1}").format(_paramarr);
  print(type, _msgstr);
}

void logger::print_func_line(std_type type, const char *func_name, int line_code, const char *msg){
  Array _paramarr;{
    _paramarr.append(String(func_name));
    _paramarr.append(line_code);
    _paramarr.append(String(msg));
  }
  String _msgstr = String("{0}:{1} {2}").format(_paramarr);
  print(type, _msgstr);
}