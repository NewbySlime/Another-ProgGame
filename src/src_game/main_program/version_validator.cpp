#include "misc_helper.h"

#include "godot_cpp/variant/utility_functions.hpp"


using namespace GameUtils::Misc::Validator;


bool _version_splitter(String &version, std::vector<int32_t> *vdata, bool allow_wildcard = true){
  vdata->clear();

  auto _splits = version.split(".");
  if(_splits.size() == 0)
    return false;

  for(int i = 0; i < _splits.size(); i++){
    String &_currstr = _splits[i];
    if(allow_wildcard && _currstr.length() == 1 && _currstr.match("[xX]")){
      vdata->insert(vdata->end(), -1);
      break;
    }

    int32_t num = 0;
    auto _conv = UtilityFunctions::str_to_var(_splits[i]);
    if(_conv.get_type() == _conv.INT)
      num = _conv;

    vdata->insert(vdata->end(), num);
  }

  return true;
}


version_validator::version_validator(String &version){
  change_validate(version);
}


bool version_validator::is_valid(){
  return _isvalid;
}

bool version_validator::change_validate(String &version){
  _compsymbol = 0;
  int _currentstep = 0;

  auto _splits = version.split(" ", false);
  for(int i = 0; i < _splits.size(); i++){
    switch(_currentstep){
      break; case 0:{
        if(_splits[i].match("[=<>]")){
          int _cmp = 0;
          String &_str = _splits[i];
          for(int i = 0; i < _str.length(); i++){
            switch(_str[i]){
              break; case '=':
                _cmp |= vvc_equal;

              break; case '<':
                _cmp |= vvc_lesser;
              
              break; case '>':
                _cmp |= vvc_greater;
            }
          }

          _compsymbol = _cmp;
          _currentstep = 1;
        }
        else{
          _compsymbol = vvc_equal;
          goto explicit_jump1;
        }
      }
      
      break; case 1:{
        explicit_jump1:

        _isvalid = _version_splitter(_splits[i], &_vdata);
        return _isvalid;
      }
    }
  }

  return false;
}

bool version_validator::validate(String &version){
  if(_compsymbol == vvc_error)
    return false;

  std::vector<int32_t> _vsplit;
  if(_version_splitter(version, &_vsplit, false)){
    for(int i = 0; i < _vdata.size(); i++){
      int _checkn = _vdata[i];
      int _currn = _vsplit[i];
      if(_checkn < 0)  // if wildcard
        continue;

      switch(_compsymbol){
        break; case vvc_equal:
          if(_currn == _checkn)
            continue;
          else
            return false;
        
        break; case vvc_greater:
          if(_currn > _checkn)
            return true;
          else
            return false;

        break; case vvc_lesser:
          if(_currn < _checkn)
            return true;
          else
            return false;

        break; case vvc_greater_than_or_equal:
          if(_currn == _checkn)
            continue;
          else if(_currn > _checkn)
            return true;
          else
            return false;

        break; case vvc_lesser_than_or_equal:
          if(_currn == _checkn)
            continue;
          else if(_currn < _checkn)
            return true;
          else
            return false;
      }      
    }

    return true;
  }

  return false;
}