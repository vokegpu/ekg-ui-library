#pragma once
#ifndef EKG_CPU_INFO_H
#define EKG_CPU_INFO_H

namespace ekg {
    enum class platform {
        os_win, os_linux, os_android
    };

    extern ekg::platform os;
}

#endif