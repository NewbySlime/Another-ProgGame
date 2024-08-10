#ifndef LOADER_HEADER
#define LOADER_HEADER

#define LANGUAGE_NAME "cpp"
#include "../../I_logger.h"
#include "../../lib_param_structs.h"

#ifndef COMPILER_PROGRAM_NAME
#define COMPILER_PROGRAM_NAME "g++.exe"
#endif

#ifndef COMPILER_VERSION_OPTION
#define COMPILER_VERSION_OPTION "-v"
#endif

#ifndef COMPILER_OUTPUT_OPTION
#define COMPILER_OUTPUT_OPTION "-o"
#endif

#ifndef COMPILER_COMPILE_OPTION
#define COMPILER_COMPILE_OPTION "-c"
#endif

#ifndef COMPILER_COMPILE_RESULT_EXT
#define COMPILER_COMPILE_RESULT_EXT ".o"
#endif

// Use this if compiler support Unix-style path
//#define COMPILER_USE_WILDCARD
#ifdef COMPILER_USE_WILDCARD
#define WILDCARD_LINKING_OBJECT "*" COMPILER_COMPILE_RESULT_EXT
#endif

#define INTERNAL_ERROR_MESSAGE "Internal error. Error message: "


using namespace GameUtils;


I_logger *get_logger();

#endif