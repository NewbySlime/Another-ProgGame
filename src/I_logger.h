#ifndef I_LOGGER_HEADER
#define I_LOGGER_HEADER


#ifndef __CUSTOM_ASSERT
#define __CUSTOM_ASSERT

#ifndef LANGUAGE_NAME
#error LANGUAGE_NAME is not defined, define it by passing the argument to the compiler. (Ex. -DLANGUAGE_NAME="cpp")
#endif

#include "string"


#define INIT_ASSERT() \
const std::string __func_name = "[lib-" LANGUAGE_NAME "] " + std::string(__FUNCTION__); \
int __src_line; 


#define LOG_ASSERT(i_logger, stdtype, msg) \
i_logger->print_func(stdtype, __func_name.c_str(), msg);

#define LOG_ASSERT_EVAL(i_logger, stdtype, msg, eval) \
if(eval) { \
  i_logger->print_func(stdtype, __func_name.c_str(), msg); \
}


#define LOG_ASSERT_LINE(i_logger, stdtype, msg) \
__src_line = __LINE__; \
i_logger->print_func_line(stdtype, __func_name.c_str(), __src_line, msg);

#define LOG_ASSERT_LINE_EVAL(i_logger, stdtype, msg, eval) \
__src_line = __LINE__; \
if(eval) { \
  i_logger->print_func_line(stdtype, __func_name.c_str(), __src_line, msg); \
}


#endif // __CUSTOM_ASSERT


namespace GameUtils{
  class I_logger{
    public:
      enum std_type{
        ST_LOG,
        ST_WARNING,
        ST_ERROR
      };

      virtual void print(std_type type, const char *msg){}
      virtual void print_func(std_type type, const char *func_name, const char *msg){}
      virtual void print_func_line(std_type type, const char *func_name, int line_code, const char *msg){}
  };
}

#endif