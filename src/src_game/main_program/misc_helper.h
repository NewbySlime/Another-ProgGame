#ifndef MISC_HELPER_HEADER
#define MISC_HELPER_HEADER

#include "godot_cpp/classes/ref.hpp"
#include "godot_cpp/variant/variant.hpp"
#include "godot_cpp/variant/dictionary.hpp"
#include "godot_cpp/variant/packed_string_array.hpp"


#include "vector"
#include "map"


using namespace godot;

namespace GameUtils{
  namespace Misc{
    namespace Folder{
      enum overwrite_type{
        // copies without modifying folder contents
        ot_nothing          = 0,
        
        // this should overwrite all files that has the same names
        ot_overwrite_file  = 0b001,

        // this should overwrite contents of the folder that has same names
        ot_overwrite_folder = 0b010,

        // ot_overwrite_file and ot_overwrite_folder combined
        ot_overwrite_folder_file = 0b011,

        // completely replacing the contents, any edited or added files in the folder
        // will be erased copying the same structure of the source folder
        ot_replace_folder   = 0b110
      };

      Error copy_folder_to(const String& source, const String& destination, overwrite_type overwrite = ot_overwrite_folder);

      Error verify_folder(const String& source, const String& destination, bool with_files = true);

      Error delete_path_recursive(const String& path);
    }

    
    namespace File{
      // excluding extension
      String get_file_name(const String& file_name);

      // returns the extension of the file (without separator '.')
      String get_file_extension(const String& file_name);
    }

    namespace Validator{
      class version_validator{
        public:
          enum version_validator_comp{
            vvc_equal                 = 0b001,
            vvc_greater               = 0b010,
            vvc_lesser                = 0b100,
            vvc_greater_than_or_equal = 0b011,
            vvc_lesser_than_or_equal  = 0b101,
            vvc_error                 = 0b111
          };


        private:
          // if the last int of the version part is -1, then it means "x"
          std::vector<int32_t> _vdata;
          int _compsymbol;

          bool _isvalid;

        public:
          version_validator(String &version);

          bool is_valid();
          bool change_validate(String &version);
          bool validate(String &version);
      };
    }

    namespace Collection{
      int check_dictionary_keys(godot::PackedStringArray& keys, godot::Dictionary& dict);

      const char* godot_str_to_cstr(const godot::String& str);

      template<typename T_data> class extension_data{
        private:
          std::map<String, T_data*> _extension_map;

        public:
          bool add_extension(const String& extension_name, T_data* data);
          T_data* get_extension_data(const String& path_file);
          T_data* get_extension_data_ext(const String& file_name);
      };
    }
  }
}

#endif