#include "misc_helper.h"
#include "logger.h"

using namespace GameUtils::Misc::Collection;


template<typename T_data> bool extension_data<T_data>::add_extension(const String& extension_name, T_data* data){
  if(_extension_map.find(extension_name) != _extension_map.end())
    return false;

  _extension_map[extension_name] = data;
  return true;
}

template<typename T_data> T_data* extension_data<T_data>::get_extension_data(const String& path_file){
  String extension_name = path_file.right(-path_file.rfind("."));
  return get_extension_data_ext(extension_name);
}

template<typename T_data> T_data* extension_data<T_data>::get_extension_data_ext(const String& file_name){
  int _idx = file_name.rfind(".");
  if(_idx < 0)
    return NULL;

  String _extname = file_name.right(-(_idx+1));
  auto _iter = _extension_map.find(_extname);
  if(_iter == _extension_map.end())
    return NULL;

  return _iter->second;
}


// compile for certain template
#include "lang_lib_loader.h"
template extension_data<GameUtils::library_functions>;