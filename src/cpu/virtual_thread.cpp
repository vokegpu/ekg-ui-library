#include "ekg/cpu/virtual_thread.hpp"
#include "ekg/util/geometry.hpp"

ekg::cpu::thread::thread(const std::string &thread_tag, ekg::feature* thread_data, const std::function<void(ekg::feature*)> &thread_callback, bool thread_initial_state) {
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

void ekg::cpu::thread_worker::process_threads() {
    for (uint8_t it = 0; it < this->thread_ticked_iterations; it++) {
        auto &thread = this->allocated_thread[it];

        if (thread->state) {
            thread->callback(thread->data);
            thread->state = false;
        }
    }

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
    this->allocated_thread[process_id]->state = true;
    this->should_thread_poll = true;
}

ekg::cpu::thread *ekg::cpu::thread_worker::get_process_by_tag(const std::string& process_tag) {
    for (uint8_t it = 0; it < this->thread_ticked_iterations; it++) {
        auto &thread = this->allocated_thread[it];

        if (thread->tag == process_tag) {
            return thread;
        }
    }

    return nullptr;
}

ekg::cpu::thread *ekg::cpu::thread_worker::get_process_by_id(uint32_t process_id) {
    return this->allocated_thread[process_id];
}
