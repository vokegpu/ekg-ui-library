#include "ekg/service/handler.hpp"

void ekg::service::handler::dispatch(ekg::cpu::event* event) {
    if (event->should_free_memory) {
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
    for (uint32_t poll_iterations = 0; poll_iterations < this->event_queue.size(); poll_iterations++) {
        ekg::cpu::event* &ekg_event = this->event_queue.front();
        this->event_queue.pop();

        if (!ekg_event->should_free_memory) {
            delete ekg_event;
            ekg_event = {};
        }
    }
}

bool ekg::service::handler::should_poll() {
    return this->should_poll_queue;
}
