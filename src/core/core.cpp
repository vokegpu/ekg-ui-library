#include "ekg/core/runtime.hpp"
#include "ekg/cpu/cpu_input.hpp"
#include "ekg/draw/draw.hpp"

void ekg::runtime::set_root(SDL_Window *sdl_win_root) {
    this->root = sdl_win_root;
}

SDL_Window* ekg::runtime::get_root() {
    return this->root;
}

void ekg::runtime::init() {
    this->allocator.init();
    this->should_redraw = true;
}

void ekg::runtime::quit() {
    this->allocator.quit();
}

void ekg::runtime::process_event(SDL_Event &sdl_event) {
}

void ekg::runtime::process_update() {
    this->thread_worker.process_threads();
}

void ekg::runtime::process_render() {
    if (this->should_redraw) {
        this->should_redraw = false;
        this->allocator.invoke();
        
        for (ekg::ui::abstract_widget* &widgets : this->loaded_abstract_widget_list) {
            if (widgets->data->is_alive() && widgets->data->get_state() == ekg::state::visible) {
                widgets->on_draw_refresh();
            }
        }

        this->allocator.revoke();
    }

    this->allocator.draw();
}

ekg::gpu::allocator &ekg::runtime::get_gpu_allocator() {
    return this->allocator;
}

ekg::cpu::thread_worker &ekg::runtime::get_cpu_thread_worker() {
    return this->thread_worker;
}
