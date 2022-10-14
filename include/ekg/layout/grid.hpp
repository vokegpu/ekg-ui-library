#ifndef EKG_LAYOUT_GRID_H
#define EKG_LAYOUT_GRID_H

#include "ekg/util/geometry.hpp"

namespace ekg::layout {
    struct grid {
        ekg::rect top {},
                  top_left {},
                  top_center {},
                  top_right {};
        ekg::rect bottom {},
                  bottom_center {},
                  bottom_left {},
                  bottom_right {};
    };

    struct flag {
        bool top {},
             bottom {},
             left {},
             right {},
             center {},
             full {},
             free {},
             none {},
             next {};
    };
}

#endif