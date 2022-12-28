/*
 * VOKEGPU EKG LICENSE
 *
 * Respect ekg license policy terms, please take a time and read it.
 * 1- Any "skidd" or "stole" is not allowed.
 * 2- Forks and pull requests should follow the license policy terms.
 * 3- For commercial use, do not sell without give credit to vokegpu ekg.
 * 4- For ekg users and users-programmer, we do not care, free to use in anything (utility, hacking, cheat, game, software).
 * 5- Malware, rat and others virus. We do not care.
 * 6- Do not modify this license under any instance.
 *
 * @VokeGpu 2023 all rights reserved.
 */

#ifndef EKG_CPU_EVENT_H
#define EKG_CPU_EVENT_H

#include <iostream>
#include <functional>

namespace ekg {
    enum event {
        unique    = 0,
        alloc     = 2,
        allocated = 4,
        shared    = 8
    };

    namespace cpu {
        struct event {
            const char *uuid {};
            void* callback {};
            std::function<void(void*)> fun {};
            uint16_t flags {};
        };
    }
}

#endif