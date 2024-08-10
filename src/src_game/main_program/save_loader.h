#ifndef SAVE_LOADER_HEADER
#define SAVE_LOADER_HEADER

#include "godot_cpp/classes/node.hpp"
#include "godot_cpp/variant/string.hpp"
#include "godot_cpp/variant/packed_string_array.hpp"

#include "save_data.h"


#include "map"



namespace GameUtils{
  class save_loader: godot::Node{
  GDCLASS(save_loader, godot::Node)

    private:
      std::map<godot::String, save_data*> _save_lists;
      std::vector<save_data*> _unorganized_save_lists;

      save_data* _currentsave;
      
      void _load_saves();

    protected:
      static void _bind_methods();


    public:
      void _ready() override;

      void reload_save();

      save_data* get_save(godot::String ID);

      // some classes will use this to get the save data the game currently load
      save_data* get_current_save();

      // use this ONLY when the program is in main menu or choosing save data
      bool set_current_save(godot::String ID);

      const std::vector<save_data*>* get_save_list();

      bool create_new_save(godot::String ID);
  };
}

#endif