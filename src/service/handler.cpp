#include "ekg/service/handler.hpp"
#include "ekg/util/env.hpp"

void ekg::service::handler::dispatch(ekg::cpu::event* event) {
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
        ekg::cpu::event* &ekg_event = this->event_queue.front();
        ekg_event->fun(ekg_event->callback);

        if (!ekg::bitwise::contains(ekg_event->flags, ekg::event::alloc) && !ekg::bitwise::contains(ekg_event->flags, ekg::event::shared)) {
            delete ekg_event;
            ekg_event = {};
        }

        this->event_queue.pop();
    }
}

bool ekg::service::handler::should_poll() {
    return this->should_poll_queue;
}
