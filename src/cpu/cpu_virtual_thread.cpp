#include "ekg/cpu/cpu_virtual_thread.hpp"

ekg::cpu::thread::thread(const std::function<void(void*)> &_fun) {
    this->fun = _fun;
}

void ekg::cpu::worker_thread::pre_alloc_thread(ekg::cpu::thread* thread) {
    this->allocated_thread[this->thread_ticked_iterations] = thread;
    this->thread_ticked_iterations++;
    this->token++;
}

void ekg::cpu::worker_thread::dynamic_alloc_thread(ekg::cpu::thread* thread) {
    this->loaded_thread_list.push_back(thread);
}

void ekg::cpu::worker_thread::process_threads() {
    switch (this->enable_thread_poll) {
        case true: {
            for (uint8_t it = 0; it < this->thread_ticked_iterations; it++) {
                auto &thread = this->allocated_thread[it];

                if (thread->state) {
                    thread->fun(thread->data);
                    thread->state = false;
                }
            }

            for (thread* &threads : this->loaded_thread_list) {
                threads->fun(threads->data);
                delete threads;
            }

            this->loaded_thread_list.clear();
            break;
        }
    }
}