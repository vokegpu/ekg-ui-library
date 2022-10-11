#ifndef EKG_SERVICE_HANDLER_H
#define EKG_SERVICE_HANDLER_H

#include "ekg/cpu/event.hpp"
#include <iostream>
#include <vector>
#include <queue>

namespace ekg::service {
    class handler {
    protected:
        std::vector<ekg::cpu::event*> allocated_task_list {};
        std::queue<ekg::cpu::event*> event_queue {};

        bool should_poll_queue {};
    public:
        void dispatch(ekg::cpu::event* event);
        void task(std::size_t task_index);
        bool should_poll();
        void on_update();
    };
}

#endif