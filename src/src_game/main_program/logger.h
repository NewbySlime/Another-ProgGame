#ifndef LOGGER_HEADER
#define LOGGER_HEADER


#include "godot_cpp/godot.hpp"

#include "godot_cpp/classes/node.hpp"
#include "godot_cpp/classes/file_access.hpp"

#include "godot_cpp/variant/string.hpp"
#include "godot_cpp/variant/variant.hpp"

#include "godot_cpp/variant/array.hpp"

// to cancel out the assert defines in I_logger.h
#define __CUSTOM_ASSERT
#include "../../I_logger.h"

#include "map"

#ifdef _WIN64
#include "windows.h"
#endif


#define __ASSERT_PART(msg) \
__paramarr.clear();{ \
  __paramarr.append(__func_name); \
  __paramarr.append(__src_line); \
  __paramarr.append(msg); \
} __assert_msg = String("{0}:{1} {2}").format(__paramarr);


#define INIT_ASSERT() \
String __func_name = __FUNCTION__; \
Array __paramarr; \
int __src_line; \
String __assert_msg;


#define LOG_ASSERT(msg) \
__src_line = __LINE__; \
__ASSERT_PART(msg) \
logger::print_log_static(__assert_msg);


#define LOG_ASSERT_EVAL(msg, eval) \
__src_line = __LINE__; \
if(eval) { \
  __ASSERT_PART(msg); \
  logger::print_log_static(__assert_msg); \
}


#define WARN_ASSERT(msg) \
__src_line = __LINE__; \
__ASSERT_PART(msg) \
logger::print_warn_static(__assert_msg);


#define WARN_ASSERT_EVAL(msg, eval) \
__src_line = __LINE__; \
if(eval){ \
  __ASSERT_PART(msg) \
  logger::print_warn_static(__assert_msg); \
}


#define ERR_ASSERT(msg) \
__src_line = __LINE__; \
__ASSERT_PART(msg); \
logger::print_err_static(__assert_msg); 


#define ERR_ASSERT_EVAL(msg, eval) \
__src_line = __LINE__; \
if(eval){ \
  __ASSERT_PART(msg); \
  logger::print_err_static(__assert_msg); \
}



using namespace godot;


namespace GameUtils{
  class logger : public godot::Node, public I_logger {
  GDCLASS(logger, godot::Node)

    private:
#ifdef _WIN64
      HANDLE _log_mutex;
#endif

    private:
      static String _get_current_time();

    protected:
      static void _bind_methods();

    public:
      virtual void _ready() override;

      static logger *get_static_logger();

      static void print_log_static(const String &log);
      static void print_warn_static(const String &warning);
      static void print_err_static(const String &err);

      void print_log(const String &log);
      void print_warn(const String &warning);
      void print_err(const String &err);

      void print(std_type type, const String &msg);

      void print(std_type type, const char *msg) override;
      void print_func(std_type type, const char *func_name, const char *msg) override;
      void print_func_line(std_type type, const char *func_name, int line_code, const char *msg) override;
  };
}

#endif