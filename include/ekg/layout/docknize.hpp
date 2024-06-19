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
   * Obtain the remain extent size, from the latest widget index.
   **/
  void extentnize(
    float &extent,
    ekg::ui::abstract_widget *p_widget,
    uint16_t flag_ok,
    uint16_t flag_stop,
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