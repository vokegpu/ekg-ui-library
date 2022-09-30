#pragma once
#ifndef EKG_CORE_H
#define EKG_CORE_H

#include <SDL2/SDL.h>
#include "ekg/gpu/gpu_allocator.hpp"
#include "ekg/ui/abstract/ui_abstract_widget.hpp"
#include <map>
#include "ekg/cpu/cpu_virtual_thread.hpp"
#include "ekg/draw/draw_font.hpp"

namespace ekg {
    struct runtime {
    protected:
        SDL_Window* root {};
        ekg::gpu::allocator allocator {};
        ekg::cpu::thread_worker thread_worker {};

        std::vector<ekg::ui::abstract_widget*> loaded_abstract_widget_list {};
        std::vector<ekg::ui::abstract*> loaded_abstract_interface_list {};

        std::map<int32_t, ekg::ui::abstract_widget*> map_abstract_widget {};
        std::map<int32_t, ekg::ui::abstract> map_abstract {};

        ekg::draw::font_renderer f_renderer_small {};
        ekg::draw::font_renderer f_renderer_normal {};
        ekg::draw::font_renderer f_renderer_big {};

        bool should_redraw {};
        void prepare_virtual_threads();
    public:
        void set_root(SDL_Window* sdl_win_root);
        SDL_Window* get_root();

        ekg::gpu::allocator &get_gpu_allocator();
        ekg::cpu::thread_worker &get_cpu_thread_worker();

        ekg::draw::font_renderer &get_f_renderer_small();
        ekg::draw::font_renderer &get_f_renderer_normal();
        ekg::draw::font_renderer &get_f_renderer_big();

        void init();
        void quit();

        void process_event(SDL_Event &sdl_event);
        void process_update();
        void process_render();
    };
};

#endif