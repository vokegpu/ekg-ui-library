#include "ekg/util/env_gui.hpp"
#include "ekg/util/util_virtual_thread.hpp"

bool ekg::set(bool &var_mutable, bool predicate) {
    if (var_mutable != predicate) {
        ekg::process(ekg::env::redraw, ekg::thread::start);
    }

    return (var_mutable = predicate);
}

std::string &ekg::set(std::string &var_mutable, const std::string &predicate) {
    if (var_mutable != predicate) {
        ekg::process(ekg::env::redraw, ekg::thread::start);
    }

    return (var_mutable = predicate);
}
