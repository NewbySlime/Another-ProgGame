#ifndef MISC_HEADER
#define MISC_HEADER

#include "stdint.h"


namespace GameUtils{
  namespace Misc{
    template<typename t_object> uint8_t *object_as_rawdata(t_object* obj){
      return reinterpret_cast<uint8_t*>(obj);
    }

    template<typename t_object> t_object *rawdata_as_object(void* rawdata){
      return reinterpret_cast<t_object*>(rawdata);
    }
  }
}

#endif