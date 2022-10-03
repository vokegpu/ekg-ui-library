#pragma once
#ifndef EKG_CORE_H
#define EKG_CORE_H

#include <SDL2/SDL.h>
#include "ekg/gpu/allocator.hpp"
#include "ekg/ui/abstract/ui_abstract_widget.hpp"
#include <map>
#include "ekg/cpu/virtual_thread.hpp"
#include "ekg/draw/font.hpp"

namespace ekg {
    struct runtime {
    protected:
        /* Widget env lists and maps for tasks. */

        std::vector<ekg::ui::abstract_widget*> list_widget {};
        std::vector<ekg::ui::abstract*> list_interface {};

        std::vector<ekg::ui::abstract_widget*> list_update_widget {};

        std::map<int32_t, ekg::ui::abstract_widget*> map_widget {};
        std::map<int32_t, ekg::ui::abstract> map_interface {};

        /* Core services and instances. */

        SDL_Window* root {};
        ekg::gpu::allocator allocator {};
        ekg::cpu::thread_worker thread_worker {};

        ekg::draw::font_renderer f_renderer_small {};
        ekg::draw::font_renderer f_renderer_normal {};
        ekg::draw::font_renderer f_renderer_big {};

        void prepare_virtual_threads();
    public:
        void set_root(SDL_Window* sdl_win_root);
        SDL_Window* get_root();

        ekg::gpu::allocator &get_gpu_allocator();
        ekg::cpu::thread_worker &get_cpu_thread_worker();

        ekg::draw::font_renderer &get_f_renderer_small();
        ekg::draw::font_renderer &get_f_renderer_normal();
        ekg::draw::font_renderer &get_f_renderer_big();

        void update_widget(ekg::ui::abstract_widget* widget);

        void init();
        void quit();

        void process_event(SDL_Event &sdl_event);
        void process_update();
        void process_render();
    };
};

#endif