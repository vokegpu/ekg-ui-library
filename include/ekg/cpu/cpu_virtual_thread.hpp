#pragma once
#ifndef EKG_CPU_VIRTUAL_THREAD_H
#define EKG_CPU_VIRTUAL_THREAD_H

#include <iostream>
#include <vector>
#include <array>
#include <functional>

namespace ekg::cpu {
    struct thread {
        std::function<void(void*)> callback {};
        std::string tag {};

        void* data {nullptr};
        bool state {};

        thread(const std::string &thread_tag, void* thread_data, const std::function<void(void*)> &thread_callback, bool thread_initial_state = false);
        ~thread();
    };

    class thread_worker {
    protected:
        std::array<ekg::cpu::thread*, 32> allocated_thread;
        std::vector<ekg::cpu::thread*> loaded_thread_list{};

        uint8_t thread_ticked_iterations {};
    public:
        bool should_thread_poll {};

        void alloc_thread(ekg::cpu::thread* thread);
        void dispatch_thread(ekg::cpu::hread* thread);
        void start_process(const std::string &tag);
        void start_process(uint32_t process_id);
        void process_threads();
    };
}

#endif