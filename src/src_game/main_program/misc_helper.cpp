#include "misc_helper.h"

#include "godot_cpp/classes/dir_access.hpp"
#include "godot_cpp/classes/file_access.hpp"

#include "godot_cpp/variant/utility_functions.hpp"

#include "set"

#include "logger.h"
#include "../defines.h"


using namespace godot;
using namespace GameUtils::Misc;


Error Folder::copy_folder_to(const String& source, const String& destination, overwrite_type overwrite_t){
  Error _currerr;

  // check first if files exists
  if(!DirAccess::dir_exists_absolute(source))
    return Error::ERR_FILE_NOT_FOUND;
  
  bool _createfolder = false;
  if(DirAccess::dir_exists_absolute(destination)){
    if(!(overwrite_t & ot_overwrite_folder))
      return Error::OK;
    else if((overwrite_t & ot_replace_folder) == ot_replace_folder){
      _currerr = delete_path_recursive(destination);
      if(_currerr != Error::OK)
        return _currerr;

      _createfolder = true;
    }
  }
  else
    _createfolder = true;

  if(_createfolder){
    _currerr = DirAccess::make_dir_absolute(destination);
    if(_currerr != Error::OK)
      return _currerr;
  }

  auto _pdir = DirAccess::open(source);
  // just in case if the dir cannot be accessed or another reason
  if(_pdir == NULL)
    return DirAccess::get_open_error();

  // copying file(s)
  {auto dirs_file = _pdir->get_files();
    for(int i = 0; i < dirs_file.size(); i++){
      auto _paramarr1 = Array();{
        _paramarr1.append(destination);
        _paramarr1.append(dirs_file[i]);
      }
      String _destinationfile_dir = __DIR_JOINER_FORMAT.format(_paramarr1);

      auto _paramarr2 = Array();{
        _paramarr2.append(source);
        _paramarr2.append(dirs_file[i]);
      }
      String _sourcefile_dir = __DIR_JOINER_FORMAT.format(_paramarr2);

      bool _docopy = false;
      if(FileAccess::file_exists(_destinationfile_dir)){
        if(overwrite_t & ot_overwrite_file)
          _docopy = true;
      }
      else
        _docopy = true;

      if(_docopy){
        _currerr = _pdir->copy(_sourcefile_dir, _destinationfile_dir);
        if(_currerr != Error::OK)
          return _currerr;
      }
    }
  }

  // copying folder(s)
  {auto dirs_folder = _pdir->get_directories();
    for(int i = 0; i < dirs_folder.size(); i++){
      auto _paramarr1 = Array();{
        _paramarr1.append(destination);
        _paramarr1.append(dirs_folder[i]);
      }
      String _destinationfolder_dir = __DIR_JOINER_FORMAT.format(_paramarr1);

      auto _paramarr2 = Array();{
        _paramarr2.append(source);
        _paramarr2.append(dirs_folder[i]);
      }
      String _sourcefolder_dir = __DIR_JOINER_FORMAT.format(_paramarr2);

      bool _docopy = false;
      if(DirAccess::dir_exists_absolute(_destinationfolder_dir)){
        if(overwrite_t & ot_overwrite_folder)
          _docopy = true;

        // ot_replace_folder not possible as it is deleted in the first recursion
      }
      else
        _docopy = true;

      if(_docopy){
        _currerr = copy_folder_to(_sourcefolder_dir, _destinationfolder_dir, overwrite_t);
        if(_currerr != Error::OK)
          return _currerr;
      }
    }
  }

  return Error::OK;
}


Error Folder::verify_folder(const String& source, const String& destination, bool with_files){
  Array _paramarr;
  
  if(DirAccess::dir_exists_absolute(destination)){
    auto _pdir_src = DirAccess::open(source);
    auto _pdir_dst = DirAccess::open(destination);

    {
      std::set<String> _dst_folders;
      auto _folders = _pdir_dst->get_directories();
      for(int i = 0; i < _folders.size(); i++)
        _dst_folders.insert(_folders[i]);
      
      _folders = _pdir_src->get_directories();
      for(int i = 0; i < _folders.size(); i++){
        _paramarr.clear();{
          _paramarr.append(source); 
          _paramarr.append(_folders[i]);
        }
        String _newdir_src = __DIR_JOINER_FORMAT.format(_paramarr);
        
        _paramarr.clear();{
          _paramarr.append(destination);
          _paramarr.append(_folders[i]);
        }
        String _newdir_dst = __DIR_JOINER_FORMAT.format(_paramarr);

        verify_folder(_newdir_src, _newdir_dst);
        _dst_folders.erase(_folders[i]);
      }

      for(auto _f_dst: _dst_folders){
        _paramarr.clear();{
          _paramarr.append(destination);
          _paramarr.append(_f_dst);
        }
        String _deldir_dst = __DIR_JOINER_FORMAT.format(_paramarr);

        delete_path_recursive(_deldir_dst);
      }
    }

    if(with_files){
      std::set<String> _dst_files;
      auto _files = _pdir_dst->get_files();
      for(int i = 0; i < _files.size(); i++)
        _dst_files.insert(_files[i]);

      _files = _pdir_src->get_files();
      for(int i = 0; i < _files.size(); i++){
        _paramarr.clear();{
          _paramarr.append(source);
          _paramarr.append(_files[i]);
        }
        String _newdir_src = __DIR_JOINER_FORMAT.format(_paramarr);

        _paramarr.clear();{
          _paramarr.append(destination);
          _paramarr.append(_files[i]);
        }
        String _newdir_dst = __DIR_JOINER_FORMAT.format(_paramarr);

        String _chk_src = FileAccess::get_sha256(_newdir_src);
        String _chk_dst = FileAccess::get_sha256(_newdir_dst);
        if(_chk_src != _chk_dst)
          DirAccess::copy_absolute(_newdir_src, _newdir_dst);

        _dst_files.erase(_files[i]);
      }

      for(auto _f_dst: _dst_files){
        _paramarr.clear();{
          _paramarr.append(destination);
          _paramarr.append(_f_dst);
        }
        String _deldir_dst = __DIR_JOINER_FORMAT.format(_paramarr);

        DirAccess::remove_absolute(_deldir_dst);
      }
    }
  }
  else
    copy_folder_to(source, destination, ot_nothing);

  return Error::OK;
}


Error Folder::delete_path_recursive(const String& path){
  if(DirAccess::dir_exists_absolute(path)){
    auto _pdir = DirAccess::open(path);
    if(_pdir != NULL){
      // deleting folder(s)
      auto _foldernames = _pdir->get_directories();
      for(int i = 0; i < _foldernames.size(); i++){
        auto _paramarr1 = Array();{
          _paramarr1.append(path);
          _paramarr1.append(_foldernames[i]);
        }
        String _folderpath = __DIR_JOINER_FORMAT.format(_paramarr1);
        
        delete_path_recursive(_folderpath);
      }

      // deleting file(s)
      auto _filenames = _pdir->get_files();
      for(int i = 0; i < _filenames.size(); i++)
        _pdir->remove(_filenames[i]);

      DirAccess::remove_absolute(path);
    }
    else
      return DirAccess::get_open_error();
  }
  else
    return Error::ERR_FILE_NOT_FOUND;

  return Error::OK;
}


int Collection::check_dictionary_keys(godot::PackedStringArray& keys, godot::Dictionary& dict){
  int _res = 0;
  for(int i = 0; i < keys.size(); i++){
    if(dict.has(keys[i])){
      keys.remove_at(i);
      i--;
    }
    else
      _res++;
  }

  return _res;
}

const char* Collection::godot_str_to_cstr(const godot::String& str){
  return (const char*)str.to_ascii_buffer().ptr();
}