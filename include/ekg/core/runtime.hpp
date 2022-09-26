#pragma once
#ifndef EKG_CORE_H
#define EKG_CORE_H

#include <SDL2/SDL.h>
#include "ekg/gpu/gpu_allocator.hpp"
#include "ekg/ui/abstract/ui_abstract_widget.hpp"
#include <map>
#include "ekg/cpu/cpu_virtual_thread.hpp"

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

        bool should_redraw {};
    public:
        void set_root(SDL_Window* sdl_win_root);
        SDL_Window* get_root();

        ekg::gpu::allocator &get_gpu_allocator();
        ekg::cpu::thread_worker &get_cpu_thread_worker();

        void init();
        void quit();

        void process_event(SDL_Event &sdl_event);
        void process_update();
        void process_render();
    };
};

#endif