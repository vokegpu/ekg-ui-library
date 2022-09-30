#include "ekg/cpu/cpu_virtual_thread.hpp"
#include "ekg/util/util.hpp"

ekg::cpu::thread::thread(const std::string &thread_tag, void *thread_data, const std::function<void(void *)> &thread_callback, bool thread_initial_state) {
    this->tag = thread_tag;
    this->data = thread_data;
    this->callback = thread_callback;
    this->state = thread_initial_state;
}

ekg::cpu::thread::~thread() {

}

void ekg::cpu::thread_worker::alloc_thread(ekg::cpu::thread* thread) {
    this->allocated_thread[this->thread_ticked_iterations] = thread;
    this->thread_ticked_iterations++;

    if (thread->state) {
        this->should_thread_poll = true;
    }
}

void ekg::cpu::thread_worker::dispatch_thread(ekg::cpu::thread* thread) {
    this->loaded_thread_list.push_back(thread);
    this->should_thread_poll = true;
}

void ekg::cpu::thread_worker::process_threads() {
    for (uint8_t it = 0; it < this->thread_ticked_iterations; it++) {
        auto &thread = this->allocated_thread[it];

        if (thread->state) {
            thread->callback(thread->data);
            thread->state = false;
        }
    }

    for (thread* &threads : this->loaded_thread_list) {
        threads->callback(threads->data);
        delete threads;
    }

    this->loaded_thread_list.clear();
    this->should_thread_poll = false;
}

void ekg::cpu::thread_worker::start_process(const std::string &tag) {
    for (uint8_t it = 0; it < this->thread_ticked_iterations; it++) {
        auto &thread = this->allocated_thread[it];

        if (thread->tag == tag) {
            thread->state = true;
            this->should_thread_poll = true;
            break;
        }
    }
}

void ekg::cpu::thread_worker::start_process(uint32_t process_id) {
    // unsafe
    this->allocated_thread[process_id]->state = true;
}
