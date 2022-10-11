#include "ekg/service/handler.hpp"
#include "ekg/util/env.hpp"

void ekg::service::handler::dispatch(ekg::cpu::event* event) {
    if (event->should_free_memory && event->first_call) {
        this->allocated_task_list.push_back(event);
        event->first_call = false;
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

        if (!ekg_event->should_free_memory) {
            delete ekg_event;
            ekg_event = {};
        }

        this->event_queue.pop();
    }
}

bool ekg::service::handler::should_poll() {
    return this->should_poll_queue;
}
