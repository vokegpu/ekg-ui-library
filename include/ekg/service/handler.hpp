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

#ifndef EKG_SERVICE_HANDLER_H
#define EKG_SERVICE_HANDLER_H

#include <iostream>
#include <vector>
#include <queue>
#include <map>
#include <unordered_map>

#include "ekg/core/task.hpp"

namespace ekg::service {
  class handler {
  protected:
    std::queue<ekg::task> task_queue {};
    std::unordered_map<const char *, bool> pre_allocated_task_dispatched_map {};
    std::vector<ekg::task> pre_allocated_task_list {};
  public:
    ekg::task &allocate();

    ekg::task &generate();

    void dispatch_pre_allocated_task(uint64_t index);

    void on_update();
  };
}

#endif