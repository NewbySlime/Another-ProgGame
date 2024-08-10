#include "godot_cpp/variant/variant.hpp"
#include "godot_cpp/classes/json.hpp"
#include "godot_cpp/classes/dir_access.hpp"
#include "godot_cpp/classes/file_access.hpp"
#include "godot_cpp/classes/os.hpp"
#include "godot_cpp/classes/project_settings.hpp"

#include "godot_cpp/variant/utility_functions.hpp"
#include "godot_cpp/classes/engine.hpp"

#include "../defines.h"
#include "misc_helper.h"
#include "lang_lib_loader.h"

#include "logger.h"

#include "iostream"


using namespace GameUtils;
using namespace godot;

const std::vector<const char*> _functionlist = {
  "init_lib",
  "deinit_lib",
  "run_code",
  "compile_code",
  "check_language_readiness",
  "get_io"
};


namespace GameUtils{
  bool language_loader::_load_default_lib(){
    INIT_ASSERT()

    int _err_load = _load_lib_folder(__PROGRAMMER_LIBRARY_DEFAULT_DIR, true);
    if(_err_load > 0){
      Array _paramarr;{
        _paramarr.append(_err_load);
      }
    
      ERR_ASSERT(String("{0} libraries cannot be loaded.").format(_paramarr))
      return false;
    }

    return true;
  }
  
  int language_loader::_load_lib_folder(const String& folder_path, bool _isdef){
    INIT_ASSERT()
    
    int _err_load = 0;
    auto pdir = DirAccess::open(folder_path);
    if(pdir != NULL){
      auto dirs = pdir->get_directories();
      int _err_load = 0;
      for(int i = 0; i < dirs.size(); i++){
        Array _paramarr;{
          _paramarr.append(folder_path);
          _paramarr.append(dirs[i]);
        }

        String _folderdir = __DIR_JOINER_FORMAT.format(_paramarr);
        _paramarr.clear();{
          _paramarr.append(_folderdir);
          _paramarr.append(__PROGRAMMER_LIBRARY_JSON);
        }

        String _jsondir = __DIR_JOINER_FORMAT.format(_paramarr);
        if(FileAccess::file_exists(_jsondir)){
          auto _pfilejson = FileAccess::open(_jsondir, FileAccess::ModeFlags::READ);

          auto jsonParse = JSON();
          auto _err = jsonParse.parse(_pfilejson->get_as_text());
          if(_err == Error::OK){
            Dictionary _jsonData = jsonParse.get_data();
            PackedStringArray _keycheck;{
              _keycheck.append("name");
              _keycheck.append("handler_library");
            }

            int _missing = Misc::Collection::check_dictionary_keys(_keycheck, _jsonData);
            if(_missing == 0){
              // getting the json data
              String _langname = _jsonData["name"];
              String _libfilename = _jsonData["handler_library"];

              // setting the structure
              _library_data *_lib_data = new _library_data();
              library_functions *_lib_funcs = new library_functions();
              _lib_funcs->language_name = new String(_langname);
              
              if(_jsonData.has("include_file"))
                _lib_funcs->include_file = new String(_jsonData["include_file"]);
              else
                _lib_funcs->include_file = new String("");
      
#ifdef _WIN64
              _lib_data->_lib_handle = NULL;
#endif

              // combine paths
              _paramarr.clear(); _paramarr.append(_langname);
              LOG_ASSERT(String("Language name: {0}").format(_paramarr));

              _paramarr.clear();{
                _paramarr.append(_folderdir);
                _paramarr.append(_libfilename);
              }
              String _libdir = ProjectSettings::get_singleton()->globalize_path(__DIR_JOINER_FORMAT.format(_paramarr));
              _libdir = _libdir.replace("/", "\\");

              _paramarr.clear(); _paramarr.append(_libfilename);
              LOG_ASSERT(String("Loading library '{0}'...").format(_paramarr))

#ifdef _WIN64
              bool _lib_load_failing = false;
              _lib_data->_lib_handle = LoadLibraryA((const char*)_libdir.to_ascii_buffer().ptr());
              if(_lib_data->_lib_handle != NULL){
                std::map<std::string, FARPROC> _f_map;
                for(int i = 0; i < _functionlist.size(); i++){
                  FARPROC _faddr = GetProcAddress(_lib_data->_lib_handle, _functionlist[i]);
                  if(_faddr == NULL){
                    _paramarr.clear();{
                      _paramarr.append(_functionlist[i]);
                      _paramarr.append((long)GetLastError());
                    }
                    ERR_ASSERT(String("Cannot load '{0}', WINAPI Error code: {1}.").format(_paramarr));

                    _lib_load_failing = true;
                  }

                  _f_map[_functionlist[i]] = _faddr;
                }

                if(!_lib_load_failing){
                  _lib_data->_private_lib_functions.init_lib
                    = (LibCallbackInterface::init_lib_callback)_f_map["init_lib"];
                  _lib_data->_private_lib_functions.deinit_lib 
                    = (LibCallbackInterface::deinit_lib_callback)_f_map["deinit_lib"];

                  _lib_funcs->run_code
                    = (LibCallbackInterface::run_code_callback)_f_map["run_code"];
                  _lib_funcs->compile_code
                    = (LibCallbackInterface::compile_code_callback)_f_map["compile_code"];
                  _lib_funcs->get_io
                    = (LibCallbackInterface::get_io_callback)_f_map["get_io"];
                  _lib_funcs->check_language_readiness
                    = (LibCallbackInterface::check_language_readiness_callback)_f_map["check_language_readiness"];
                }
                else{
                  _paramarr.clear();{
                    _paramarr.append(_langname);
                  }
                  ERR_ASSERT(String("Cannot load language '{0}'.").format(_paramarr));

                  goto skip_to_error;
                }
              }
              else{
                _paramarr.clear();{
                  _paramarr.append(_libdir);
                  _paramarr.append((long)GetLastError());
                }
                ERR_ASSERT(String("Cannot load library '{0}', WINAPI Error code: {1}.").format(_paramarr));
                
                goto skip_to_error;
              }
#endif
              // check if failing load
              if(_lib_data->_private_lib_functions.init_lib(logger::get_static_logger())){
                LOG_ASSERT("Loading success!");
                _dll_program_handle[_langname] = _lib_funcs;

                // getting extension types
                if(_jsonData.has("extension")){
                  Array _extarr = _jsonData["extension"];
                  for(int i = 0; i < _extarr.size(); i++)
                    _dll_program_handle_ext.add_extension(_extarr[i], _lib_funcs);
                }

                if(_isdef)
                  _deflib_data_container.insert(_deflib_data_container.end(), _lib_data);
                else
                  _lib_data_container.insert(_lib_data_container.end(), _lib_data);
                
                goto skip_no_error;
              }
              else{
                ERR_ASSERT("Loading unsuccessful");
              }

              skip_to_error:{
                if(_lib_data->_lib_handle != NULL)
                  FreeLibrary(_lib_data->_lib_handle);
                delete _lib_data;
                delete _lib_funcs->language_name;
                delete _lib_funcs;
                
                _err_load++;
              }

              skip_no_error:{}
            }
            else{
              _paramarr.clear();{
                _paramarr.append(_missing);
                _paramarr.append(_jsondir);
              }
              ERR_ASSERT(String("{0} meta info in '{1}' are missing, the keys missing are:").format(_paramarr));

              for(int i = 0; i < _keycheck.size(); i++){
                _paramarr.clear();{
                  _paramarr.append(_keycheck[i]);
                }
                ERR_ASSERT(String("\t'{0}'").format(_paramarr))
              }

              _err_load++;
            }
          }
          else{
            _paramarr.clear();{
              _paramarr.append((int)_err);
            }
            ERR_ASSERT(String("Error when parsing JSON file. (Error {0}).").format(_paramarr))

            _err_load++;
          }
        }
        else{
          _paramarr.clear();{
            _paramarr.append(__PROGRAMMER_LIBRARY_JSON);
            _paramarr.append(dirs[i]);
          }
          ERR_ASSERT(String("File \"{0}\" doens't exists in folder \"{1}\".").format(_paramarr))

          _err_load++;
        }
      }
    }

    return _err_load;
  }


  void language_loader::_bind_methods(){
    ADD_SIGNAL(MethodInfo("on_library_refreshed"));
  }

  void language_loader::_ready(){
    INIT_ASSERT()

    Engine *_engine = Engine::get_singleton();
    if(_engine->is_editor_hint()){
      UtilityFunctions::print("test editor");
    }
    else{
      logger *_log = logger::get_static_logger();
      _log->print_func_line(logger::ST_LOG, "testfunction", 12, "hello_world");
      _load_default_lib();
      load_libraries();
      logger::print_log_static("hello world");
      logger::print_warn_static("there's a bug?");
      logger::print_err_static("an error occurred");

      LOG_ASSERT("test")
      LOG_ASSERT_EVAL("this is true", true)
      LOG_ASSERT_EVAL("this is false", false)

      WARN_ASSERT("this is a warn")
      ERR_ASSERT("there's something wrong")

      String _langtest = "C++";
      const library_functions* _libfuncs = get_lang_lib(_langtest);
      if(_libfuncs){
        Array _paramarr;{
          _paramarr.append(*_libfuncs->language_name);
        }
        LOG_ASSERT(String("Loader found. Name of the language: '{0}'.").format(_paramarr))
        LOG_ASSERT("Checking compiler version...")

        if(_libfuncs->check_language_readiness()){
          LOG_ASSERT("Compiler is ready!")
        }
        else{
          ERR_ASSERT("Compiler is not ready")
        }
      }
      else{
        ERR_ASSERT("Cannot find a language.")
      }

      String _langfile = "hello_world.cpp";
      _libfuncs = get_lang_lib_ext(_langfile);
      if(_libfuncs){
        Array _paramarr;{
          _paramarr.append(_langfile);
          _paramarr.append(*_libfuncs->language_name);
        }
        LOG_ASSERT(String("Language for '{0}', is: '{1}'").format(_paramarr))
      }
      else{
        Array _paramarr;{
          _paramarr.append(_langfile);
        }
        ERR_ASSERT(String("Cannot find loader for '{0}'").format(_paramarr))
      }
    }
  }
  
  const library_functions* language_loader::get_lang_lib(const String& _langname){
    auto _iter = _dll_program_handle.find(_langname);
    if(_iter == _dll_program_handle.end())
      return NULL;

    return _iter->second;
  }

  const library_functions* language_loader::get_lang_lib_ext(const String& extension){
    return _dll_program_handle_ext.get_extension_data_ext(extension);
  }

  int language_loader::load_libraries(){
    return _load_lib_folder(__PROGRAMMER_LIBRARY_USER_DIR);
  }
}