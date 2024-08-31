#ifndef EKG_LAYOUT_DOCKNIZE_H
#define EKG_LAYOUT_DOCKNIZE_H

#include "ekg/ui/abstract/ui_abstract_widget.hpp"
#include "ekg/util/geometry.hpp"

/**
 * Returns the dimensional extent based in count and the offset (space between rects). 
 * 
 * The pixel imperfect issue was solved here...
 * For a long time I did not know what was going on with the pixels,
 * some solutions I used did not work, then I discovered that all the time
 * was this dimension extent with float imprecision loss.
 * 
 * Each pixels represent 1.0f, if the GPU receives pixels with
 * (n + f) `n` a non-floating point number and `f` a floating point;
 * the rasterizer will jump between pixels, resulting in pixel-imperfect.
 * 
 * The following formula make you understand:
 * ( (g - d) - (c * o) ) / c
 * 
 * g = group rect
 * d = dimensional extent
 * c = amount of widgets with fill property flag until any flag next
 * o = UI offset setting
 * 
 * Float-only without the int32_t cast may results in pixel-imperfect
 * due the influence of dimensional size of parent rect, font height, font width etc.
 * 
 * There is no problem with float32 to int32 (I guess)
 * I do not believe in larger monitors size than 16k.
 * 
 * - Rina.
 **/
#define ekg_layout_get_dimensional_extent(dimension, extent, offset, count) \
( \
  (static_cast<int32_t>(dimension) - static_cast<int32_t>(extent)) \
    - \
  (static_cast<int32_t>(count * offset)) \
) / static_cast<int32_t>(count) \

namespace ekg::layout {
  /**
   * The between widgets offset: Auto-scaled & pixel-fixed.
   **/
  extern float offset;

  /**
   * Column and row store extent values, used to calculate
   * the remains size for `ekg::dock::fill`.
   **/
  struct extent_t {
  public:
    int64_t end_index {};
    int64_t begin_index {};
    int64_t count {};
    float extent {};
  };

  extern ekg::layout::extent_t h_extent;
  extern ekg::layout::extent_t v_extent;

  /**
   * The between rects from widgets can be docknized
   * and return a rect mask (the bouding limits of docknization action).
   * 
   * The use is simple:
   * set a preset,
   * insert dock rects (rect with dock),
   * docknize,
   * get the rect mask.
   **/
  struct mask {
  public:
    struct rect {
    public:
      ekg::rect *p_rect {};
      ekg::flags flags {};
    };
  protected:
    std::vector<ekg::layout::mask::rect> dock_rect_list {};
    float respective_all {};
    float respective_center {};
    ekg::axis axis {};
    ekg::vec3 offset {};
    ekg::rect mask {};
    ekg::layout::extent_t v_extent {};
    ekg::layout::extent_t h_extent {};
  protected:
    void extentnize(
      float &extent,
      ekg::flags flag_ok,
      ekg::flags flag_stop,
      int64_t &begin_and_count,
      ekg::axis axis
    );
  public:
    void preset(const ekg::vec3 &mask_offset, ekg::axis mask_axis, float initial_respective_size = 0.0f);
    void insert(const ekg::layout::mask::rect &dock_rect);
    void docknize();
    ekg::rect &get_rect();
  };

  /**
   * Obtain the remain extent size, from the latest widget index.
   **/
  void extentnize(
    float &extent,
    ekg::ui::abstract_widget *p_widget,
    ekg::flags flag_ok,
    ekg::flags flag_stop,
    int64_t &begin_and_count,
    ekg::axis axis
  );

  /**
   * A mid-functional feature to process dock position
   * from widgets.
   * Note: Recursive.
   **/
  void docknize(ekg::ui::abstract_widget *p_widget);
}

#endif