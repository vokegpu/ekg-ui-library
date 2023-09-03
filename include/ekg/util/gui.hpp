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

#ifndef EKG_UTIL_EVENT_H
#define EKG_UTIL_EVENT_H

#include "ekg/ui/abstract/ui_abstract_widget.hpp"
#include "ekg/cpu/event.hpp"
#include <map>

namespace ekg {
    enum class env {
        refresh, swap, reload, synclayout, gc, redraw
    };

    void dispatch(ekg::cpu::event *p_ekg_event);
    void dispatch(ekg::env env);
    bool listen(ekg::cpu::uievent &ui_event, SDL_Event &sdl_event);

    extern ekg::rect empty;

    struct hovered {
        static int32_t id;
        static ekg::type type;
        static int32_t up;
        static int32_t down;
        static ekg::type downtype;
        static ekg::type uptype;
    };

    struct stack {
        std::map<int32_t, bool> registry {};
        std::vector<ekg::ui::abstract_widget*> ordered_list {};
        void clear();
    };

    void reload(int32_t id);
    void reload(ekg::ui::abstract_widget *p_widget);

    void synclayout(int32_t id);
    void synclayout(ekg::ui::abstract_widget *p_widgets);

    void refresh(int32_t id);
    void refresh(ekg::ui::abstract_widget *p_widget);

    void update_high_frequency(int32_t id);
    void update_high_frequency(ekg::ui::abstract_widget *p_widget);

    void push_back_stack(ekg::ui::abstract_widget *p_widget, ekg::stack &stack);
    ekg::ui::abstract_widget *find_absolute_parent_master(ekg::ui::abstract_widget *p_widget);

    int32_t &set(int32_t &value, int32_t result);
    bool &set(bool &value, bool result);
    std::string &set(std::string &value, std::string_view result);

    /*
     * Instead of dispatching lot of useless events like layout processing,
     * the assert is able to check and prevent that.
     */
    extern bool assert_task_flag;
    float assert_task(float, float);
}

#endif