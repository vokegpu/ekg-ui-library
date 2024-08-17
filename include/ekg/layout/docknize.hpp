#ifndef EKG_LAYOUT_DOCKNIZE_H
#define EKG_LAYOUT_DOCKNIZE_H

#include "ekg/ui/abstract/ui_abstract_widget.hpp"
#include "ekg/util/geometry.hpp"

namespace ekg::layout {
  /**
   * The between widgets offset: Auto-scaled & pixel-fixed.
   **/
  extern float offset;

  /**
   * Column and row store extent values, used to calculate
   * the remains size for `ekg::dock::fill`.
   **/
  struct extent {
  public:
    static ekg::rect h;
    static ekg::rect v;
  };

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
    float respective_all {};
    float respective_center {};
    ekg::axis axis {};
    ekg::vec3 offset {};
    std::vector<ekg::layout::mask::rect> dock_rect_list {};
    ekg::rect mask {};
  public:
    void preset(const ekg::vec3 &mask_offset, ekg::axis mask_axis, float initial_respective_size = 0.0f);
    void insert(const ekg::layout::mask::rect &dock_rect);
    void docknize();

    float get_respective_size();
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