#include "ekg/ekg.hpp"

void ekg::process(const std::string &process_tag, const ekg::thread &thread_env) {
    switch (thread_env) {
        case ekg::thread::start: {
            ekg::core->get_cpu_thread_worker().start_process(process_tag);
            break;
        }

        case ekg::thread::stop: {
            ekg::core->get_cpu_thread_worker().get_process_by_tag(process_tag)->state = false;
            break;
        }
    }
}

void ekg::process(const ekg::env &process_env_id, const ekg::thread &thread_env) {
    switch (thread_env) {
        case ekg::thread::start: {
            ekg::core->get_cpu_thread_worker().start_process((int32_t) process_env_id);
            break;
        }

        case ekg::thread::stop: {
            ekg::core->get_cpu_thread_worker().get_process_by_id((int32_t) process_env_id)->state = false;
            break;
        }
    }
}
