#include "ekg/core/runtime.hpp"
#include "ekg/cpu/cpu_input.hpp"
#include "ekg/draw/draw_font.hpp"
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

    if (FT_Init_FreeType(&ekg::draw::font_renderer::ft_library)) {
        ekg::log("Could not init FreeType");
    }

    this->f_renderer_small.font_size = 16;
    this->f_renderer_small.reload();
    this->f_renderer_small.bind_allocator(&this->allocator);

    this->f_renderer_normal.font_size = 18;
    this->f_renderer_normal.reload();
    this->f_renderer_normal.bind_allocator(&this->allocator);

    this->f_renderer_big.font_size = 24;
    this->f_renderer_big.reload();
    this->f_renderer_big.bind_allocator(&this->allocator);
}

void ekg::runtime::quit() {
    this->allocator.quit();
}

ekg::draw::font_renderer &ekg::runtime::get_f_renderer_small() {
    return this->f_renderer_small;
}

ekg::draw::font_renderer &ekg::runtime::get_f_renderer_normal() {
    return this->f_renderer_normal;
}

ekg::draw::font_renderer &ekg::runtime::get_f_renderer_big() {
    return this->f_renderer_big;
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

        ekg::draw::rect(10, 20, 200, 200, {255, 255, 255, 255});
        this->f_renderer_big.blit("Hi this text was render using font and allocator features.", 11, 80, {0, 0, 0, 255});

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
