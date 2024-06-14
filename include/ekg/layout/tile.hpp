#ifndef EKG_LAYOUT_TILE_H
#define EKG_LAYOUT_TILE_H

#include "ekg/util/geometry.hpp"

namespace ekg::layout {
  struct tile_t {
  public:
    ekg::vec2 pos {};
    ekg::vec2 dimension {};
  };

  ekg::rect &tile_transform(ekg::rect &rect_to_transform);
}

#endif