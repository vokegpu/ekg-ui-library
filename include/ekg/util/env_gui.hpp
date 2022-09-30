#pragma once
#ifndef EKG_UTIL_ENV_GUI_H
#define EKG_UTIL_ENV_GUI_H

#include <iostream>xz

namespace ekg {
    bool set(bool &var_mutable, bool predicate);
    bool set(std::string &var_mutable, const std::string &predicate);
};

#endif