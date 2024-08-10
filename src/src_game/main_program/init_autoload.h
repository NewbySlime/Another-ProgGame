#ifndef INIT_AUTOLOAD_HEADER
#define INIT_AUTOLOAD_HEADER

#include "godot_cpp/godot.hpp"
#include "godot_cpp/classes/node.hpp"

#include "godot_cpp/core/binder_common.hpp"


namespace Game{
  class init_autoload: public godot::Node {
  GDCLASS(init_autoload, godot::Node)

    protected:
      static void _bind_methods();

    public:
      void _ready() override;

      init_autoload();
      ~init_autoload();
  };
}


#endif