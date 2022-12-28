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

namespace ekg::service {
    class handler {
    protected:
        std::vector<ekg::cpu::event*> allocated_task_list {};
        std::queue<ekg::cpu::event*> event_queue {};
        std::map<const char*, bool> events_going_on {};

        bool should_poll_queue {};
        uint8_t cool_down_ticks {};
    public:
        void dispatch(ekg::cpu::event* event);
        void task(std::size_t task_index);
        bool should_poll();
        void on_update();
    };
}

#endif