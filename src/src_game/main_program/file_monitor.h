#ifndef FILE_MONITOR
#define FILE_MONITOR

#include "godot_cpp/classes/node.hpp"



namespace GameUtils{
  namespace Persistance{
    class file_monitor: godot::Node{
    GDCLASS(file_monitor, godot::Node)

      protected:
        void _bind_methods();

      public:
        void _ready() override;
    };
  }
}

#endif