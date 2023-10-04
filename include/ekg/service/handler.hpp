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

#include "ekg/core/task.hpp"
#include <iostream>
#include <vector>
#include <queue>
#include <map>
#include <unordered_map>

namespace ekg::service {
    class handler {
    protected:
        std::queue<task> task_queue {};
        std::unordered_map<const char*, bool> pre_allocated_task_dispatched_map {};
        std::vector<task> pre_allocated_task_list {};
    protected:
        std::queue<task> multi_thread_task_queue {};
        std::unordered_map<const char*, bool> multi_thread_task_dispatched_map {};
        bool running_multi_thread_task {};
    public:
        void set_running_multi_thread_task(bool state);
        bool is_running_multi_thread_task();

        task &allocate();
        task &generate();
        task &generate_multi_thread();

        void init_multi_thread_task_thread();
        void dispatch_pre_allocated_task(uint64_t index);
        void on_update();
        void on_update_multi_thread();
    };
}

namespace ekg {
    void multi_thread_task_thread_update(ekg::service::handler *p_service_handler);
}

#endif