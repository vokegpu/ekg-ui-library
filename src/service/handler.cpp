/**
 * MIT License
 * 
 * Copyright (c) 2022-2024 Rina Wilk / vokegpu@gmail.com
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

ekg::task *&ekg::service::handler::allocate() {
  return this->pre_allocated_task_list.emplace_back();
}

void ekg::service::handler::dispatch(ekg::task *p_task) {
  this->task_queue.push(p_task);
}

void ekg::service::handler::dispatch_pre_allocated_task(uint64_t index) {
  ekg::task *&p_task {
    this->pre_allocated_task_list.at(index)
  };

  if (!p_task->is_dispatched) {
    this->task_queue.push(p_task);
    p_task->is_dispatched = true;
  }
}

void ekg::service::handler::on_update() {
  while (!this->task_queue.empty()) {
    ekg::task *p_ekg_event {this->task_queue.front()};
    p_ekg_event->function(p_ekg_event->info);
    p_ekg_event->is_dispatched = false;
    this->task_queue.pop();
  }
}