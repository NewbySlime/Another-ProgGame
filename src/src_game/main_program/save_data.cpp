#include "save_data.h"

#include "godot_cpp/classes/dir_access.hpp"
#include "godot_cpp/classes/file_access.hpp"
#include "godot_cpp/classes/json.hpp"

#include "../defines.h"
#include "misc_helper.h"

#include "memory"



#define __META_DATA_KEY_NAME String("save_name")

#define __META_DATA_KEY_SAVED_DATE String("date")

#define __META_DICTIONARY_CHECK(dict, missing) { \
  PackedStringArray _keycheck;{ \
    _keycheck.append(__META_DATA_KEY_NAME); \
  } \
  missing = Misc::Collection::check_dictionary_keys(_keycheck, dict); \
}


using namespace godot;
using namespace GameUtils;



void save_data::_bind_methods(){
  ADD_SIGNAL(MethodInfo("on_clearing_memory"));
  ADD_SIGNAL(MethodInfo("on_saving_data"));
  ADD_SIGNAL(MethodInfo("on_data_loaded"));
}


save_data::save_data(){
  _loaded = false;
  _folder_context = "";
}

save_data::~save_data(){
  clear_data();
}

String save_data::get_ID(){
  if(!_loaded || _folder_context == ""){
    save();
    clear_data();
  }
}

save_data::dict_data* save_data::get_dict_map(String name){
  auto _iter = _dict_map.find(name);
  if(_iter != _dict_map.end())
    return _iter->second;
  else
    return NULL;
}

bool save_data::create_dict_map(String name){
  auto _iter = _dict_map.find(name);
  if(_iter == _dict_map.end()){
    dict_data* _dict = new dict_data();

#ifdef _WIN64
    _dict->data_mutex = CreateMutex(NULL, false, NULL);
#endif

    _dict_map[name] = _dict;

    return true;
  }
  else
    return false;
}

bool save_data::dict_map_exist(godot::String name){
  auto _iter = _dict_map.find(name);
  return _iter != _dict_map.end();
}

save_data::bin_data* save_data::get_bin_map(String name){
  auto _iter = _bin_map.find(name);
  if(_iter != _bin_map.end())
    return _iter->second;
  else
    return NULL;
}

bool save_data::create_bin_map(String name){
  auto _iter = _bin_map.find(name);
  if(_iter == _bin_map.end()){
    bin_data* _bin = new bin_data();
    _bin->data = std::malloc(0);
    _bin->data_length = 0;

#ifdef _WIN64
    _bin->data_mutex = CreateMutex(NULL, false, NULL);
#endif

    _bin_map[name] = _bin;

    return true;
  }
  else
    return false;
}

bool save_data::bin_map_exist(String name){
  auto _iter = _bin_map.find(name);
  return _iter != _bin_map.end();
}


bool save_data::resize_bin_data(godot::String name, uint32_t new_size){
  bin_data* _bin = get_bin_map(name);
  if(!_bin)
    return false;

  _bin->data = std::realloc(_bin->data, new_size);
  _bin->data_length = new_size;
  return true;
}


void save_data::set_folder_context(String folder_dir){
  _folder_context = folder_dir;
}


bool save_data::load(){
  Array _paramarr;

  // loading dictionary data
  std::map<String, Error (*)(save_data*, Ref<FileAccess>, const String&)> _file_parser {
    {String(__EXTENSION_JSON), [](save_data *_this, Ref<FileAccess> _file, const String& _file_name) {
        JSON _parser;
        Error _err = _parser.parse(_file->get_as_text());

        if(_err != Error::OK)
          return _err;

        _this->create_dict_map(_file_name);
        dict_data* _dict = _this->get_dict_map(_file_name);

        _dict->dict = _parser.get_data();
        return Error::OK;
      }
    },

    {String(__EXTENSION_BINARY), [](save_data* _this, Ref<FileAccess> _file, const String& _file_name){
        _this->create_bin_map(_file_name);
        bin_data* _bin = _this->get_bin_map(_file_name);

        _bin->data_length = _file->get_length();
        _this->resize_bin_data(_file_name, _bin->data_length);
        
        _file->get_buffer((uint8_t*)_bin->data, _bin->data_length);
        return Error::OK;
      }
    }
  };
  
  PackedStringArray _file_arr = DirAccess::get_files_at(_folder_context);
  for(int i = 0; i < _file_arr.size(); i++){
    String _file_name = Misc::File::get_file_name(_file_arr[i]);
    String _file_ext = Misc::File::get_file_extension(_file_arr[i]);

    _paramarr.clear();{
      _paramarr.append(_folder_context);
      _paramarr.append(_file_arr[i]);
    }

    auto _iter = _file_parser.find(_file_ext);
    if(_iter != _file_parser.end()){
      Error _err = _iter->second(
        this,
        FileAccess::open(__DIR_JOINER_FORMAT.format(_paramarr), FileAccess::ModeFlags::READ),
        _file_name
      );

      if(_err != Error::OK){
        // cannot load data
      }
    }
    else{
      // cannot find a parser
    }
  }

  _loaded = true;
  emit_signal("on_data_loaded");
  return true;
}

bool save_data::load_meta(){
  Array _paramarr;

  {// loading meta data json
    _paramarr.clear();{
      _paramarr.append(_folder_context);
      _paramarr.append(__SAVE_META_FILE);
    }

    Ref<FileAccess> _meta_file = FileAccess::open(__DIR_JOINER_FORMAT.format(_paramarr), FileAccess::ModeFlags::READ);

    JSON _parser;
    Error _err = _parser.parse(_meta_file->get_as_text());
    if(_err != Error::OK)
      return false;

    _meta_dict = _parser.get_data();
    int _missing; __META_DICTIONARY_CHECK(_meta_dict, _missing)

    if(_missing != 0)
      return false;
  }

  _loaded = true;
  return true;
}


bool save_data::save(){
  if(_loaded){
    emit_signal("on_saving_data");

    Array _paramarr;

    // saving dictionary data
    // meta data json
    {String _meta_data = JSON::stringify(_meta_dict);

      _paramarr.clear();{
        _paramarr.append(_folder_context);
        _paramarr.append(__SAVE_META_FILE);
      }
      Ref<FileAccess> _meta_file = FileAccess::open(__DIR_JOINER_FORMAT.format(_paramarr),FileAccess::ModeFlags::WRITE);
      _meta_file->store_string(_meta_data);
    }

    // all other json file
    for(auto _dict: _dict_map){
  #ifdef _WIN64
      WaitForSingleObject(_dict.second->data_mutex, INFINITE);
  #endif
      
      String _dict_data = JSON::stringify(_dict.second);

      _paramarr.clear();{
        _paramarr.append(_dict.first);
        _paramarr.append(__EXTENSION_JSON);
      }

      String _file_name = __FILE_EXT_JOINER_FORMAT.format(_paramarr);

      _paramarr.clear();{
        _paramarr.append(_folder_context);
        _paramarr.append(_file_name);
      }

      Ref<FileAccess> _dict_file = FileAccess::open(__DIR_JOINER_FORMAT.format(_paramarr), FileAccess::ModeFlags::WRITE);
      _dict_file->store_string(_dict_data);
    
  #ifdef _WIN64
      ReleaseMutex(_dict.second->data_mutex);
  #endif
    }


    // saving binary data
    for(auto _bin: _bin_map){
  #ifdef _WIN64
      WaitForSingleObject(_bin.second->data_mutex, INFINITE);
  #endif

      _paramarr.clear();{
        _paramarr.append(_bin.first);
        _paramarr.append(__EXTENSION_BINARY);
      }

      String _file_name = __FILE_EXT_JOINER_FORMAT.format(_paramarr);

      _paramarr.clear();{
        _paramarr.append(_folder_context);
        _paramarr.append(_file_name);
      }

      Ref<FileAccess> _bin_file = FileAccess::open(__DIR_JOINER_FORMAT.format(_paramarr), FileAccess::ModeFlags::WRITE);
      _bin_file->store_buffer((uint8_t*)_bin.second->data, _bin.second->data_length);

  #ifdef _WIN64
      ReleaseMutex(_bin.second->data_mutex);
  #endif
    }

    return true;
  }

  return false;
}

bool save_data::init_save_file(save_meta_data* meta){
  if(!_loaded){
    _meta_dict[__META_DATA_KEY_NAME] = meta->save_name;

    _loaded = true;
    save();

    return true;
  }

  return false;
}


bool save_data::clear_data(){
  if(_loaded){
    emit_signal("on_clearing_memory");
    _loaded = false;

    for(auto _dict: _dict_map){
#ifdef _WIN64
      CloseHandle(_dict.second->data_mutex);
#endif

      delete _dict.second;
    }

    for(auto _bin: _bin_map){
      std::free(_bin.second->data);

#ifdef _WIN64
      CloseHandle(_bin.second->data_mutex);
#endif

      delete _bin.second;
    }

    return true;
  }

  return false;
}