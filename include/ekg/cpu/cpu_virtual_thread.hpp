#pragma once
#ifndef EKG_CPU_VIRTUAL_THREAD_H
#define EKG_CPU_VIRTUAL_THREAD_H

#include <iostream>
#include <vector>
#include <array>
#include <functional>

namespace ekg {
    namespace cpu {
        struct thread {
            std::function<void(void*)> fun{};
            void* data {nullptr};

            std::string tag;
            bool state {};

            thread(const std::function<void(void*)> &_fun);
        };

        class worker_thread {
        protected:
            std::array<ekg::cpu::thread*, 32> allocated_thread;
            std::vector<ekg::cpu::thread*> loaded_thread_list{};

            uint8_t thread_ticked_iterations {};
            uint8_t token {};

            bool enable_thread_poll {};
        public:
            void pre_alloc_thread(ekg::cpu::thread* thread);
            void dynamic_alloc_thread(ekg::cpu::thread* thread);
            void process_threads();
        };
    }
}

#endif