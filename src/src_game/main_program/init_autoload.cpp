#include "godot_cpp/classes/engine.hpp"

#include "misc_helper.h"
#include "init_autoload.h"
#include "../defines.h"


using namespace Game;
using namespace GameUtils::Misc::Folder;


void init_autoload::_bind_methods(){

}

void init_autoload::_ready(){
  godot::Engine* _engine = godot::Engine::get_singleton();
  if(!_engine->is_editor_hint()){
    verify_folder(__TEMPLATE_DEFAULT_DATA_DIR, __DEFAULT_DIR);
    copy_folder_to(__TEMPLATE_USER_DATA_DIR, __USER_DIR, ot_overwrite_folder);
  }
}

init_autoload::init_autoload(){
  
}

init_autoload::~init_autoload(){

}