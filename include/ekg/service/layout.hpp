#ifndef EKG_LAYOUT_H
#define EKG_LAYOUT_H

#include "ekg/ui/abstract/ui_abstract_widget.hpp"
#include <vector>

namespace ekg::service {
    class layout {
    protected:
        int32_t min_factor_height {};
        float min_offset {};
        float min_height {};
    public:
        void init();
        void quit();

        void process_scaled(ekg::ui::abstract_widget* widget);
        void process(ekg::ui::abstract_widget* widget);
    };
}

#endif