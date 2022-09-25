#include "ekg/cpu/cpu_virtual_thread.hpp"

ekg::cpu::thread::thread(std::function<void(void)> _fun) {
    this->fun = _fun;
}

void ekg::cpu::worker_thread::pre_alloc_thread(ekg::cpu::thread &thread) {

}

void ekg::cpu::worker_thread::dynamic_alloc_thread(ekg::cpu::thread &thread) {

}

void ekg::cpu::worker_thread::process_threads() {
    switch (this->env_worker) {
        case ekg::cpu::env::dynamic: {
            break;
        }

        case ekg::cpu::env::allocated: {
            break;
        }

        case ekg::cpu::env::hybrid: {
            break;
        }
    }
}