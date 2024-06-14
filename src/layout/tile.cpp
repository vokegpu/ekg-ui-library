#include "ekg/layout/tile.hpp"
#include "ekg/ui/display.hpp"

ekg::rect &ekg::layout::tile_transform(ekg::rect &rect_to_transform) {
  ekg::layout::tile_t tile {
    .pos = {rect_to_transform.x, rect_to_transform.y},
    .dimension = {rect_to_transform.w, rect_to_transform.h}
  };

  tile.pos.x /= ekg::ui::dpi_tiling;
  tile.pos.y /= ekg::ui::dpi_tiling;

  tile.pos.x = static_cast<int32_t>(tile.pos.x) * ekg::ui::dpi_tiling;
  tile.pos.y = static_cast<int32_t>(tile.pos.y) * ekg::ui::dpi_tiling;

  tile.dimension.x /= ekg::ui::dpi_tiling;
  tile.dimension.y /= ekg::ui::dpi_tiling;

  tile.dimension.x = static_cast<int32_t>(tile.dimension.x) * ekg::ui::dpi_tiling;
  tile.dimension.y = static_cast<int32_t>(tile.dimension.y) * ekg::ui::dpi_tiling;

  rect_to_transform.x = tile.pos.x;
  rect_to_transform.y = tile.pos.y;
  rect_to_transform.w = tile.dimension.x;
  rect_to_transform.h = tile.dimension.y;

  return rect_to_transform;
}