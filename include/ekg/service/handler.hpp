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

#ifndef EKG_SERVICE_HANDLER_H
#define EKG_SERVICE_HANDLER_H

#include "ekg/cpu/event.hpp"
#include <iostream>
#include <vector>
#include <queue>
#include <map>
#include <unordered_map>

namespace ekg::service {
    class handler {
    protected:
        std::queue<ekg::cpu::event> task_queue {};
        std::unordered_map<const char*, bool> pre_allocated_task_dispatched_map {};
        std::vector<ekg::cpu::event> pre_allocated_task_list {};
    public:
        ekg::cpu::event &allocate();
        ekg::cpu::event &generate();

        void dispatch_pre_allocated_task(uint64_t index);
        void on_update();
    };
}

#endif