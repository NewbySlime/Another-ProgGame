#ifndef SAVE_DATA_HEADER
#define SAVE_DATA_HEADER

#ifdef _WIN64
#include "Windows.h"
#endif

#include "godot_cpp/classes/node.hpp"

#include "godot_cpp/variant/string.hpp"
#include "godot_cpp/variant/dictionary.hpp"

#include "map"


namespace GameUtils{
  class save_data: godot::Node{
  GDCLASS(save_data, godot::Node)

    public:
      typedef struct{
        String save_name;
      } save_meta_data;

      typedef struct{
        void *data;
        uint64_t data_length;

#ifdef _WIN64
        HANDLE data_mutex;
#endif
      } bin_data;

      typedef struct{
        godot::Dictionary dict;

#ifdef _WIN64
        HANDLE data_mutex;
#endif
      } dict_data;

    private:
      bool _loaded = false;
      godot::String _folder_context = "";

      Dictionary _meta_dict;

      std::map<godot::String, dict_data*> _dict_map;
      std::map<godot::String, bin_data*> _bin_map;

    protected:
      static void _bind_methods();

    public:
      save_data();
      ~save_data();

      godot::String get_ID();
      godot::String get_name();

      dict_data* get_dict_map(godot::String name);
      bool create_dict_map(godot::String name);
      bool dict_map_exist(godot::String name);

      bin_data* get_bin_map(godot::String name);
      bool create_bin_map(godot::String name);
      bool bin_map_exist(godot::String name);

      bool resize_bin_data(godot::String name, uint32_t new_size);

      void set_folder_context(godot::String folder_dir);

      bool load();
      bool load_meta();

      // DO NOT save after creating this class as all of save data is not loaded
      bool save();
      bool init_save_file(save_meta_data* meta);

      // only use this to clear up memory associated with it, when changing save files
      bool clear_data();
  };
}

#endif