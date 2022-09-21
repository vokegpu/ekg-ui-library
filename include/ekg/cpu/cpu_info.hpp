#pragma once
#ifndef EKG_CPU_ARCHITECTURE_H
#define EKG_CPU_ARCHITECTURE_H

namespace ekg {
    // enum class architecture {
    //     x86x64, x86, x64, arm, arm64
    // };

    enum class platform {
        os_win, os_linux, os_android
    };
}

#if defined(_WIN)
static ekg::platform EKG_CPU_PLATFORM {ekg::platform::os_win};
#elif defined(__linux__)
static ekg::platform EKG_CPU_PLATFORM {ekg::platform::os_linux};
#elif defined(__ANDROID__)
static ekg::platform EKG_CPU_PLATFORM {ekg::platform::os_android};
#endif

#endif