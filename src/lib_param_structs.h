#ifndef PROGRAM_HEADER
#define PORGRAM_HEADER

#ifdef _WIN64
#include "windows.h"
#endif

#include "stdint.h"

#include "string"
#include "vector"


namespace GameUtils{
  struct run_info{
    // If child program isn't ready (language_loader isn't blocking/waiting for readiness), game will wait for the child program by reading the query pipe
    bool is_ready = false;
  };

  struct process_info{
    std::string program_dir;
    std::string program_name;

#ifdef _WIN64
    HANDLE query_pipe_write;
#endif


    uint32_t program_id;


    // Data that will be saved as a part of player save data.
    // This should be allocated by library and freed by library.
    // The size of the memory when initiating is up to the library,
    //  in the end, main program will ask for resizing the data.
    // But the size of the heap memory will be changed every time the library needs to.
    char *additional_data = NULL;
    int additional_data_length = 0;

    // Library owned object, to hold any data associated to current child process.
    void *_lib_obj = NULL;
  };

  struct compile_info{
    std::string output_dir;
    std::string program_name;

    std::string folder_dir;
    std::vector<std::string> file_array;

#ifdef _WIN64
    HANDLE stdout_handle;
#endif

    // This used inbetween the loader message and error message outputted by compiler and such
    // Make sure don't use normal write, as it has null-terminating character even if not in the end of the seperator
    const char* separator_msg;
    uint32_t separator_msg_length;
  };
}

#endif