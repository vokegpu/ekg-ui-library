#pragma once
#ifndef EKG_CORE_H
#define EKG_CORE_H

#include <SDL2/SDL.h>
#include "ekg/gpu/gpu_allocator.hpp"
#include "ekg/ui/abstract/ui_abstract_widget.hpp"
#include <map>

namespace ekg {
    struct runtime {
    protected:
        SDL_Window* root {};
        gpu::allocator allocator {};

        std::vector<ekg::ui::abstract_widget*> loaded_abstract_widget_list {};
        std::vector<ekg::ui::abstract*> loaded_abstract_interface_list {};

        std::map<int32_t, ekg::ui::abstract_widget*> map_abstract_widget {};
        std::map<int32_t, ekg::ui::abstract> map_abstract {};
    public:
        void set_root(SDL_Window* sdl_win_root);
        SDL_Window* get_root();

        void init();
        void quit();

        void process_event(SDL_Event &sdl_event);
        void process_update();
        void process_render();
    };
};

#endif