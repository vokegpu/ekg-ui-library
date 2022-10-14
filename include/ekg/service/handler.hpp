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