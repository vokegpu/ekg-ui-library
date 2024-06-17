#ifndef EKG_LAYOUT_DOCKNIZE_H
#define EKG_LAYOUT_DOCKNIZE_H

#include "ekg/ui/abstract/ui_abstract_widget.hpp"
#include "ekg/util/geometry.hpp"

namespace ekg::layout {
  struct extent {
  protected:
    static ekg::rect h;
    static ekg::rect v;
  };

  void docknize(ekg::ui::abstract_widget *p_widget);
}

#endif