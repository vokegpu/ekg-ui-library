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

#ifndef EKG_UI_ABSTRACT_WIDGET_H
#define EKG_UI_ABSTRACT_WIDGET_H

#include "ekg/ui/abstract/ui_abstract.hpp"
#include "ekg/os/ekg_sdl.hpp"
#include "ekg/util/io.hpp"

namespace ekg::ui {
    class abstract_widget {
    public:
        ekg::ui::abstract *data {};
        ekg::flag flag {};
        ekg::rect dimension {};
        ekg::vec2 min_size {};

        ekg::rect *parent {};
        ekg::vec4 *scroll {};
        
        ekg::rect empty_parent {};
        ekg::vec4 empty_scroll {};

        bool is_scissor_refresh {};
        bool is_high_frequency {};
        bool is_targeting_absolute_parent {};
    public:
        ekg::rect get_static_rect();
        [[nodiscard]] ekg::rect &get_abs_rect();
    public:
        explicit abstract_widget();
        ~abstract_widget();

        virtual void on_destroy();
        virtual void on_reload();
        virtual void on_pre_event(SDL_Event &sdl_event);
        virtual void on_event(SDL_Event &sdl_event);
        virtual void on_post_event(SDL_Event &sdl_event);
        virtual void on_update();
        virtual void on_draw_refresh();
    };
}

#endif