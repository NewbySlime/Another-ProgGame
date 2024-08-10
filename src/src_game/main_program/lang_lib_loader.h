#ifndef LANG_LIB_LOADER
#define LANG_LIB_LOADER


#include "godot_cpp/godot.hpp"
#include "godot_cpp/classes/node.hpp"
#include "godot_cpp/variant/string.hpp"

#include "logger.h"
#include "misc_helper.h"

#include "map"
#include "memory"

#include "../../communication_header.h"
#include "../../lib_param_structs.h"
#include "../../I_logger.h"
#include "../../IPC.h"

using namespace godot;
using namespace GameUtils::InterProcessCommunication;


namespace GameUtils{
  namespace LibCallbackInterface{
    typedef bool (__stdcall *run_code_callback)(process_info *p_info, run_info *r_info);
    typedef bool (__stdcall *compile_code_callback)(compile_info *c_info);

    typedef bool (__stdcall *suspend_process)(process_info *p_info);
    typedef bool (__stdcall *save_process)(process_info *p_info);
    typedef bool (__stdcall *stop_process)(process_info *p_info);

    typedef bool (__stdcall *check_language_readiness_callback)();

    typedef I_ipc* (__stdcall *get_io_callback)(process_info *p_info);

#ifdef _WIN64
    typedef const PROCESS_INFORMATION* (__stdcall *get_proc_info_callback)(process_info *p_info);
#endif

    typedef bool (__stdcall *pass_opt_code)(process_info *p_info, uint32_t code, void *data, uint32_t datalen);
    typedef bool (__stdcall *send_opt_code)(process_info *p_info, uint32_t code, void *data, uint32_t datalen);

    // only use this when after compiling, or want to create a child process
    typedef bool (__stdcall *init_object_process_info_callback)(process_info *p_info);

    // only use this when the process is done
    typedef bool (__stdcall *deinit_object_process_info_callback)(process_info *p_info);

    
    // below functions only be used by library loader
    typedef bool (__stdcall *init_lib_callback)(I_logger *log);
    typedef bool (__stdcall *deinit_lib_callback)();
  }

  typedef struct{
    const String* language_name;
    const String* include_file;

    LibCallbackInterface::run_code_callback run_code;
    LibCallbackInterface::compile_code_callback compile_code;

    LibCallbackInterface::check_language_readiness_callback check_language_readiness;

    LibCallbackInterface::get_io_callback get_io;
  } library_functions;
  

  class language_loader : public godot::Node {
  GDCLASS(language_loader, godot::Node)

    private:
      typedef struct{
        LibCallbackInterface::init_lib_callback init_lib;
        LibCallbackInterface::deinit_lib_callback deinit_lib;
      } _library_functions_private;

      typedef struct{
#ifdef _WIN64
        HMODULE _lib_handle;
#endif
        _library_functions_private _private_lib_functions;
      } _library_data;

      std::map<String, library_functions*> _dll_program_handle;
      Misc::Collection::extension_data<library_functions> _dll_program_handle_ext; 

      std::vector<_library_data*> _lib_data_container;
      std::vector<_library_data*> _deflib_data_container;

      bool _load_default_lib();
      void _unload_libraries();

      int _load_lib_folder(const String& folder_path, bool _isdef = false);


    protected:
      static void _bind_methods();

    public:
      virtual void _ready() override;

      /// @brief This function is to load libraries in a folder
      /// @param folder_path Folder that contains the libraries
      /// @return How many the libraries failed to load
      int load_libraries();

      const library_functions* get_lang_lib(const String& lang_name);
      const library_functions* get_lang_lib_ext(const String& extension);

      String get_lang_name(String extension);
  };
}

#endif