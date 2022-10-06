#include "ekg/core/runtime.hpp"
#include "ekg/ekg.hpp"

void ekg::runtime::set_root(SDL_Window *sdl_win_root) {
    this->root = sdl_win_root;
}

SDL_Window* ekg::runtime::get_root() {
    return this->root;
}

void ekg::runtime::init() {
    this->allocator.init();
    this->theme_manager.init();

    if (FT_Init_FreeType(&ekg::draw::font_renderer::ft_library)) {
        ekg::log("Could not init FreeType");
    }

    this->prepare_virtual_threads();
    this->prepare_ui_env();
}

void ekg::runtime::quit() {
    this->allocator.quit();
    this->theme_manager.quit();
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
    this->input_manager.on_event(sdl_event);
}

void ekg::runtime::process_update() {
    this->input_manager.on_update();

    if (this->thread_worker.should_thread_poll) {
        this->thread_worker.process_threads();
    }
}

void ekg::runtime::process_render() {
    this->allocator.draw();
}

ekg::gpu::allocator &ekg::runtime::get_gpu_allocator() {
    return this->allocator;
}

ekg::cpu::thread_worker &ekg::runtime::get_cpu_thread_worker() {
    return this->thread_worker;
}

void ekg::runtime::prepare_virtual_threads() {
    ekg::log("creating events allocating virtual threads");

    this->thread_worker.alloc_thread(new ekg::cpu::thread("swap", nullptr, [](void* data) {
    }));

    this->thread_worker.alloc_thread(new ekg::cpu::thread("refresh", nullptr, [](void* data) {
    }));

    this->thread_worker.alloc_thread(new ekg::cpu::thread("reset", nullptr, [](void* data) {
    }));

    this->thread_worker.alloc_thread(new ekg::cpu::thread("update", &this->list_update_widget, [](void* data) {
        auto list = static_cast<std::vector<ekg::ui::abstract_widget*>*>(data);

        for (ekg::ui::abstract_widget* &widgets : *list) {
            widgets->on_update();
        }

        list->clear();
    }));

    this->thread_worker.alloc_thread(new ekg::cpu::thread("redraw", &this->list_widget, [](void* data) {
        auto list = *static_cast<std::vector<ekg::ui::abstract_widget*>*>(data);
        ekg::core->allocator.invoke();

        for (ekg::ui::abstract_widget* &widgets : list) {
            if (widgets->data->is_alive() && widgets->data->get_state() == ekg::state::visible) {
                widgets->on_draw_refresh();
            }
        }

        ekg::core->allocator.revoke();
    }));
}

ekg::ui::abstract_widget *ekg::runtime::get_fast_widget_by_id(uint32_t id) {
    return this->map_widget[id];
}

void ekg::runtime::update_widget(ekg::ui::abstract_widget* widget) {
    if (widget != nullptr) {
        this->list_update_widget.push_back(widget);
        ekg::process(ekg::env::update, ekg::thread::start);
    }
}

ekg::service::input &ekg::runtime::get_service_input() {
    return this->input_manager;
}

ekg::service::theme &ekg::runtime::get_service_theme() {
    return this->theme_manager;
}

void ekg::runtime::prepare_ui_env() {
    ekg::log("creating ui fonts");

    this->f_renderer_small.font_size = 16;
    this->f_renderer_small.reload();
    this->f_renderer_small.bind_allocator(&this->allocator);

    this->f_renderer_normal.font_size = 18;
    this->f_renderer_normal.reload();
    this->f_renderer_normal.bind_allocator(&this->allocator);

    this->f_renderer_big.font_size = 24;
    this->f_renderer_big.reload();
    this->f_renderer_big.bind_allocator(&this->allocator);

    ekg::log("creating ui binds");

    this->input_manager.bind("frame-drag-activy", "mouse-left");
    this->input_manager.bind("frame-drag-activy", "finger-click");
    this->input_manager.bind("frame-resize-activy", "mouse-left");
    this->input_manager.bind("frame-resize-activy", "finger-click");
    this->input_manager.bind("button-activy", "lctrl+lshift+a");
    this->input_manager.bind("button-activy", "finger-click");
    this->input_manager.bind("popup-activy", "mouse-right");
    this->input_manager.bind("popup-activy", "finger-hold");
    this->input_manager.bind("popup-component-activy", "mouse-left");
    this->input_manager.bind("popup-component-activy", "finger-click");
    this->input_manager.bind("textbox-action-delete-left", "backspace");
    this->input_manager.bind("textbox-action-select-all", "lctrl+a");
    this->input_manager.bind("textbox-action-select-all", "finger-hold");
    this->input_manager.bind("textbox-action-select-all", "mouse-double-");
    this->input_manager.bind("textbox-action-delete-right", "delete");
    this->input_manager.bind("textbox-action-break-line", "return");
    this->input_manager.bind("textbox-action-up", "up");
    this->input_manager.bind("textbox-action-down", "down");
    this->input_manager.bind("textbox-action-down", "right");
    this->input_manager.bind("textbox-action-down", "left");
}