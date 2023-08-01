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

#include "ekg/service/handler.hpp"
#include "ekg/util/env.hpp"

ekg::cpu::event &ekg::service::allocate() {
    return this->pre_allocated_task_list.emplace_back();
}

ekg::cpu::event &ekg::service::generate() {
    return this->task_queue.emplace_back();
}

void ekg::service::handler::dispatch_pre_allocated_task(uint64_t index) {
    ekg::cpu::event &task {this->allocated_task_list.at(index)};
    bool &is_dispatched {this->pre_allocated_task_dispatched_map[task.uuid]};

    if (!is_dispatched) {
        this->generate() = task;
        is_dispatched = true;
    }
}

void ekg::service::handler::on_update() {
    while (!this->event_queue.empty()) {
        ekg::cpu::event &ekg_event {this->event_queue.front()};
        ekg_event.fun(ekg_event.callback);
        this->event_queue.pop();
    }

    this->events_going_on.clear();
}
