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

void ekg::service::handler::dispatch(ekg::cpu::event* event) {
    if (this->events_going_on[event->uuid]) {
        return;
    }

    this->events_going_on[event->uuid] = true;
    if (ekg::bitwise::contains(event->flags, ekg::event::alloc) && !ekg::bitwise::contains(event->flags, ekg::event::allocated) && ekg::bitwise::add(event->flags, ekg::event::allocated)) {
        this->allocated_task_list.push_back(event);
    }

    this->event_queue.push(event);
    this->should_poll_queue = true;
}

void ekg::service::handler::task(std::size_t task_index) {
    auto *task = this->allocated_task_list.at(task_index);

    if (task != nullptr) {
        this->dispatch(task);
    }
}

void ekg::service::handler::on_update() {
    while (!this->event_queue.empty()) {
        ekg::cpu::event* &ekg_event {this->event_queue.front()};
        if (ekg_event != nullptr) {
            ekg_event->fun(ekg_event->callback);
            this->events_going_on[ekg_event->uuid] = false;
        }

        if (ekg_event != nullptr && !ekg::bitwise::contains(ekg_event->flags, ekg::event::alloc) && !ekg::bitwise::contains(ekg_event->flags, ekg::event::shared)) {
            delete ekg_event;
            ekg_event = {};
        }

        this->event_queue.pop();
    }

    this->cool_down_ticks++;

    if (this->cool_down_ticks > 255) {
        this->events_going_on.clear();
        this->cool_down_ticks = false;
    }
}

bool ekg::service::handler::should_poll() {
    return this->should_poll_queue;
}
