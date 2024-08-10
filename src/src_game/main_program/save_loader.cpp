#include "save_loader.h"
#include "logger.h"

#include "godot_cpp/classes/dir_access.hpp"
#include "godot_cpp/classes/engine.hpp"

#include "../defines.h"


using namespace godot;
using namespace GameUtils;


void save_loader::_load_saves(){
  INIT_ASSERT()
  Array _paramarr;

  _save_lists.clear();
  _unorganized_save_lists.clear();

  PackedStringArray _save_folders = DirAccess::get_directories_at(__SAVE_FOLDERS_DIRECTORY);
  for(int i = 0; i < _save_folders.size(); i++){
    _paramarr.clear();{
      _paramarr.append(__SAVE_FOLDERS_DIRECTORY);
      _paramarr.append(_save_folders[i]);
    }

    String _folder_dir = __DIR_JOINER_FORMAT.format(_paramarr);
    save_data *_sdata = new save_data();
    _sdata->set_folder_context(_folder_dir);{
      bool _success = _sdata->load();
      if(!_success){
        _paramarr.clear();{
          _paramarr.append(_save_folders[i]);
        }

        LOG_ASSERT(String("Cannot load save file in folder: {0}").format(_paramarr));
        goto on_failing;
      }

      _save_lists[_sdata->get_ID()] = _sdata;
      _unorganized_save_lists.insert(_unorganized_save_lists.end(), _sdata);
    // skipping failing label
    continue;}

    on_failing:{
      delete _sdata;
    }
  }

  if(_save_lists.size() > 0){
    _paramarr.clear();{
      _paramarr.append(_save_lists.size());
    }

    LOG_ASSERT(String("Load {0} save files.").format(_paramarr));
  }
  else{
    LOG_ASSERT(String("Cannot find any save files."));
  }
}


void save_loader::_bind_methods(){
  // create signal "on_bind_save_data"
  // create signal "on_unbind_save_data"
}


void save_loader::_ready(){
  INIT_ASSERT()
  Array _paramarr;

  reload_save();
}


void save_loader::reload_save(){
  _load_saves();
}

save_data* save_loader::get_save(String ID){
  auto iter = _save_lists.find(ID);
  if(iter != _save_lists.end())
    return iter->second;
  else
    return NULL;
}

save_data* save_loader::get_current_save(){
  return _currentsave;
}

bool save_loader::set_current_save(String ID){
  save_data *_data = get_save(ID);
  if(_data){
    _currentsave = _data;
    return true;
  }
  else
    return false;
}

const std::vector<save_data*>* save_loader::get_save_list(){
  return &_unorganized_save_lists;
}

bool save_loader::create_new_save(String ID){
  Array _paramarr;

  _paramarr.clear();{
    _paramarr.append(__SAVE_FOLDERS_DIRECTORY);
    _paramarr.append(ID);
  }

  String _folder_dir = __DIR_JOINER_FORMAT.format(_paramarr);
  save_data *_data = new save_data();
  _data->set_folder_context(_folder_dir);

  _data->save();
}