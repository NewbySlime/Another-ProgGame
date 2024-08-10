#include <gdextension_interface.h>
#include <godot_cpp/core/defs.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/godot.hpp>

// set all the includes here
#include "init_autoload.h"
#include "lang_lib_loader.h"
#include "logger.h"

#include "save_data.h"
#include "save_loader.h"

using namespace godot;


void initialize_gdextension_module(ModuleInitializationLevel p_level) {
  if (p_level != MODULE_INITIALIZATION_LEVEL_SCENE) {
    return;
  }

  // set all the class here
  ClassDB::register_class<Game::init_autoload>();
  ClassDB::register_class<GameUtils::language_loader>();
  ClassDB::register_class<GameUtils::logger>();

  ClassDB::register_class<GameUtils::save_data>();
  ClassDB::register_class<GameUtils::save_loader>();
}

void uninitialize_gdextension_module(ModuleInitializationLevel p_level) {
  if (p_level != MODULE_INITIALIZATION_LEVEL_SCENE) {
    return;
  }
}

extern "C" {
  // Initialization.
  GDExtensionBool GDE_EXPORT gdextension_library_init(GDExtensionInterfaceGetProcAddress p_get_proc_address, const GDExtensionClassLibraryPtr p_library, GDExtensionInitialization *r_initialization) {
    godot::GDExtensionBinding::InitObject init_obj(p_get_proc_address, p_library, r_initialization);

    init_obj.register_initializer(initialize_gdextension_module);
    init_obj.register_terminator(uninitialize_gdextension_module);
    init_obj.set_minimum_library_initialization_level(MODULE_INITIALIZATION_LEVEL_SCENE);

    return init_obj.init();
  }
}