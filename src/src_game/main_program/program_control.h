#ifndef PROGRAM_CONTROL_HEADER
#define PROGRAM_CONTROL_HEADER

#include "lang_lib_loader.h"
#include "I_child_respondent.h"

#include "map"

#include "godot_cpp/variant/string.hpp"
#include "godot_cpp/classes/node.hpp"

using namespace GameUtils;

namespace GameUtils{
  class program_control: public godot::Node{
  GDCLASS(program_control, godot::Node)

    private:
#ifdef _WIN64
      HANDLE _query_thread;
      HANDLE _querypipe_mutex;
      HANDLE 
        _querypipe_read,
        _querypipe_write;

      static DWORD WINAPI _query_thread_init_func(void* parameter);
      DWORD _query_thread_actual_func();
#endif

      bool _query_thread_keep_listening = false;

      std::map<uint32_t, I_child_respondent*> _respondent_map;


      void _clear_class();

      // this is used when thread needs to stop/exit safely
      void _query_thread_send_dump_header();


    protected:
      static void _bind_methods();

    public:
      program_control();
      ~program_control();

      void _ready() override;

      bool compile_project_folder(String project_folder);
      uint32_t create_program_project(String project_folder);

      bool suspend_program(uint32_t program_ID);
      bool resume_program(uint32_t program_ID);

      bool register_respondent(uint32_t program_ID, I_child_respondent* respondent);

      bool stop_listening_query();
  };
}

#endif