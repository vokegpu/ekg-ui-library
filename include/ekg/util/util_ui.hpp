#ifndef EKG_UTIL_UI_H
#define EKG_UTIL_UI_H

#include "ekg/ui/abstract/ui_abstract_widget.hpp"
#include <map>

namespace ekg {
    void reload(uint32_t id);
    void reload(ekg::ui::abstract_widget* widget);

    void reset(uint32_t id);
    void reset(ekg::ui::abstract_widget* widget);

    void sync_layout(uint32_t id);
    void sync_layout(ekg::ui::abstract_widget* widget);

    void stack(ekg::ui::abstract_widget* widget, std::map<uint32_t, ekg::ui::abstract_widget*> &map);
}

#endif