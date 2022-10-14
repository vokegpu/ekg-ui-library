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
    auto task = this->allocated_task_list.at(task_index);

    if (task != nullptr) {
        this->dispatch(task);
    }
}

void ekg::service::handler::on_update() {
    while (!this->event_queue.empty()) {
        ekg::cpu::event* &ekg_event {this->event_queue.front()};
        if (ekg_event == nullptr) continue;

        ekg_event->fun(ekg_event->callback);
        this->events_going_on[ekg_event->uuid] = false;

        if (!ekg::bitwise::contains(ekg_event->flags, ekg::event::alloc) && !ekg::bitwise::contains(ekg_event->flags, ekg::event::shared)) {
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
