#pragma once
#ifndef EKG_CPU_ARCHITECTURE_H
#define EKG_CPU_ARCHITECTURE_H

namespace ekg {
    enum class platform {
        os_win, os_linux, os_android
    };

    extern ekg::platform os;
}

#endif