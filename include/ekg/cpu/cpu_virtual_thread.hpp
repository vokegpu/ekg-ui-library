#pragma once
#ifndef EKG_CPU_VIRTUAL_THREAD_H
#define EKG_CPU_VIRTUAL_THREAD_H

#include <iostream>
#include <vector>
#include <array>
#include <functional>

namespace ekg {
    namespace cpu {
        enum class env {
            hybrid, dynamic, allocated
        };

        struct thread {
            std::function<void(void)> fun{};
            uint8_t id{};

            thread(std::function<void(void)> _fun);
        };

        class worker_thread {
        protected:
            std::array<ekg::cpu::thread, 32> allocated_thread;
            std::vector<ekg::cpu::thread> loaded_thread_list{};

            ekg::cpu::env env_worker {};
        public:
            void pre_alloc_thread(ekg::cpu::thread &thread);
            void dynamic_alloc_thread(ekg::cpu::thread &thread);
            void process_threads();
        };
    }
}

#endif