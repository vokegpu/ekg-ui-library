/*
* MIT License
* 
* Copyright (c) 2022-2023 Rina Wilk / vokegpu@gmail.com
* 
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
* 
* The above copyright notice and this permission notice shall be included in all
* copies or substantial portions of the Software.
* 
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
*/

#include "ekg/service/handler.hpp"
#include "ekg/util/io.hpp"
#include "ekg/os/stl_thread.hpp"

void ekg::multi_thread_task_thread_update(ekg::service::handler *p_service_handler) {
    std::mutex mutex {};
    while (p_service_handler->is_running_multi_thread_task()) {
        mutex.lock();
    }
}

void ekg::service::handler::set_running_multi_thread_task(bool state) {
    this->running_multi_thread_task = state;
}

bool ekg::service::handler::is_running_multi_thread_task() {
    return this->running_multi_thread_task;
}

ekg::cpu::event &ekg::service::handler::allocate() {
    return this->pre_allocated_task_list.emplace_back();
}

ekg::cpu::event &ekg::service::handler::generate() {
    return this->task_queue.emplace();
}

void ekg::service::handler::init_multi_thread_task_thread() {
}

void ekg::service::handler::dispatch_pre_allocated_task(uint64_t index) {
    ekg::cpu::event &task {this->pre_allocated_task_list.at(index)};
    bool &is_dispatched {this->pre_allocated_task_dispatched_map[task.p_tag]};

    if (!is_dispatched) {
        this->generate() = task;
        is_dispatched = true;
    }
}

void ekg::service::handler::on_update() {
    while (!this->task_queue.empty()) {
        ekg::cpu::event &ekg_event {this->task_queue.front()};
        ekg_event.function(ekg_event.p_callback);
        this->task_queue.pop();
    }

    this->pre_allocated_task_dispatched_map.clear();
}
