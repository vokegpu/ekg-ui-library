#ifndef EKG_UTIL_UI_H
#define EKG_UTIL_UI_H

#include "ekg/ui/abstract/ui_abstract_widget.hpp"

namespace ekg {
    void update(uint32_t id);
    void update(ekg::ui::abstract_widget* widget);
}

#endif