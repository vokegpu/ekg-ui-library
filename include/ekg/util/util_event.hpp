#ifndef EKG_UTIL_EVENT_H
#define EKG_UTIL_EVENT_H

#include "ekg/util/env.hpp"
#include "ekg/cpu/event.hpp"
#include <iostream>

namespace ekg {
    void dispatch(ekg::cpu::event* event);
    void dispatch(const ekg::env &env);
}

#endif