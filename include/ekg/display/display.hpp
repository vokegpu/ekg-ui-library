#ifndef EKG_DISPLAY_H
#define EKG_DISPLAY_H

#include "ekg/util/geometry.hpp"

namespace ekg::display {
  extern int32_t width;
  extern int32_t height;
  extern float dt;
  extern bool auto_scale;
  extern bool scale_base;
};

#endif