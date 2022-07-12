#include <ekg/api/ekg_core.hpp>
#include <ekg/ekg.hpp>
#include <list>

void ekg_core::process_event_section(SDL_Event &sdl_event) {
    // We do not need to track others events here.
    bool should_not_end_segment = (
            sdl_event.type == SDL_FINGERDOWN      || sdl_event.type == SDL_FINGERUP ||
            sdl_event.type == SDL_MOUSEBUTTONDOWN || sdl_event.type == SDL_MOUSEBUTTONUP ||
            sdl_event.type == SDL_MOUSEMOTION     || sdl_event.type == SDL_FINGERMOTION ||
            sdl_event.type == SDL_KEYDOWN         || sdl_event.type == SDL_KEYUP
    );

    if (!should_not_end_segment) {
        return;
    }

    this->focused_element_id = 0;

    for (ekg_element* &element : this->data) {
        if (element == nullptr || element->access_flag().dead) {
            continue;
        }

        // Verify point overlap.
        element->on_pre_event_update(sdl_event);

        if (element->get_visibility() == ekg::visibility::VISIBLE && element->access_flag().over) {
            this->focused_element_id = element->get_id();
        }

        element->on_post_event_update(sdl_event);
    }

    this->sizeof_render_buffer = 0;
    this->render_buffer.fill(nullptr);

    for (ekg_element* &element : this->data) {
        if (element == nullptr || element->access_flag().dead) {
            continue;
        }

        if (element->get_id() == this->focused_element_id) {
            element->on_pre_event_update(sdl_event);
        }

        element->on_event(sdl_event);

        if (element->get_visibility() == ekg::visibility::VISIBLE) {
            this->render_buffer[this->sizeof_render_buffer++] = element;
        }
    }

    if (ekgapi::any_input_down(sdl_event) && this->focused_element_id != 0 && this->last_focused_element_id != this->focused_element_id) {
        this->last_focused_element_id = this->focused_element_id;

        this->dispatch_todo_event(ekgutil::action::FIXSTACK);
        this->dispatch_todo_event(ekgutil::action::REFRESH);
    }
}

void ekg_core::process_update_section() {
    if (ekgutil::contains(this->todo_flags, ekgutil::SWAPBUFFERS)) {
        ekgutil::remove(this->todo_flags, ekgutil::SWAPBUFFERS);
        this->swap_buffers();
    }

    if (ekgutil::contains(this->todo_flags, ekgutil::action::FIXRECTS)) {
        ekgutil::remove(this->todo_flags, ekgutil::action::FIXRECTS);

        this->fix_rects();
    }

    if (ekgutil::contains(this->todo_flags, ekgutil::action::FIXSTACK)) {
        this->fix_stack();
    }
}

void ekg_core::process_render_section() {
    this->gpu_handler.calc_view_ortho_2d();
    this->gpu_handler.draw();

    if (ekgutil::contains(this->todo_flags, ekgutil::action::REFRESH)) {
        ekgutil::remove(this->todo_flags, ekgutil::action::REFRESH);
        ekggpu::invoke();

        for (uint32_t i = 0; i < this->sizeof_render_buffer; i++) {
            ekg_element *&element = this->render_buffer[i];
            element->on_draw_refresh();
        }

        ekggpu::revoke();
    }
}

void ekg_core::add_element(ekg_element* &element) {
    element->set_id(++this->last_id_used);
    element->set_visibility(ekg::visibility::VISIBLE);

    this->concurrent_buffer.push_back(element);
    this->dispatch_todo_event(ekgutil::action::SWAPBUFFERS);
    this->dispatch_todo_event(ekgutil::action::REFRESH);
}

/* Start of swap buffers. */
void ekg_core::swap_buffers() {
    // Clean the buffer render (not delete).
    this->sizeof_render_buffer = 0;
    this->render_buffer.fill(nullptr);

    this->data_invisible_to_memory = this->data;
    this->data.clear();

    for (ekg_element* &element : this->data_invisible_to_memory) {
        if (element == nullptr) {
            continue;
        }

        if (element->access_flag().dead) {
            delete element;
            continue;
        }

        this->data.push_back(element);

        if (element->get_visibility() == ekg::visibility::VISIBLE) {
            this->render_buffer[this->sizeof_render_buffer++] = element;
        }
    }

    for (ekg_element* &elements : this->concurrent_buffer) {
        if (elements == nullptr) {
            continue;
        }

        if (elements->access_flag().dead) {
            delete elements;
            continue;
        }

        this->data.push_back(elements);

        if (elements->get_visibility() == ekg::visibility::VISIBLE) {
            this->render_buffer[this->sizeof_render_buffer++] = elements;
        }
    }

    // Clean after push the buffers into main update buffer.
    this->concurrent_buffer.clear();
}
/* End of swap buffers. */

/* Start of fix stack. */
void ekg_core::fix_stack() {
    if (this->focused_element_id == 0 && this->forced_focused_element_id == 0) {
        return;
    }

    ekgutil::remove(this->todo_flags, ekgutil::action::FIXSTACK);

    if (this->focused_element_id == 0) {
        this->focused_element_id = this->forced_focused_element_id;
    }

    ekgutil::stack concurrent_all_data_stack;
    ekgutil::stack concurrent_focused_stack;
    ekgutil::stack concurrent_data_stack;

    for (ekg_element* &elements : this->data) {
        if (elements == nullptr) {
            continue;
        }

        if (concurrent_focused_stack.contains(elements->get_id())) {
            continue;
        }

        concurrent_data_stack.ids.clear();
        elements->collect_stack(concurrent_data_stack);

        if (concurrent_data_stack.contains(this->focused_element_id)) {
            concurrent_focused_stack = concurrent_data_stack;
        } else {
            concurrent_all_data_stack.add(elements->get_id());
        }
    }

    this->sizeof_render_buffer = 0;
    this->render_buffer.fill(nullptr);
    this->data_invisible_to_memory.clear();

    ekg_element* element;

    for (uint32_t ids : concurrent_all_data_stack.ids) {
        ekg_element* element;

        if (!this->find_element(element, ids)) {
            continue;
        }

        this->data_invisible_to_memory.push_back(element);

        if (element->get_visibility() == ekg::visibility::VISIBLE) {
            this->render_buffer[this->sizeof_render_buffer++] = element;
        }
    }

    for (uint32_t ids : concurrent_focused_stack.ids) {
        if (!this->find_element(element, ids)) {
            continue;
        }

        this->data_invisible_to_memory.push_back(element);

        if (element->get_visibility() == ekg::visibility::VISIBLE) {
            this->render_buffer[this->sizeof_render_buffer++] = element;
        }
    }

    this->data.clear();
    this->data = this->data_invisible_to_memory;
    this->data_invisible_to_memory.clear();

    this->forced_focused_element_id = 0;
}
/* End of fix stack. */

/* Start of fix rects. */
void ekg_core::fix_rects() {
    ekgutil::stack cached_stack;

    for (ekg_element* &element : this->data) {
        if (element == nullptr || element->access_flag().dead) {
            continue;
        }

        this->fix_rect(element, cached_stack);
    }
}

void ekg_core::fix_rect(ekg_element *&element, ekgutil::stack &cached_stack) {
    if (cached_stack.contains(element->get_id()) || !element->is_mother()) {
        return;
    }

    cached_stack.add(element->get_id());

    ekg_element* instance;

    for (uint32_t &ids : element->access_children_stack().ids) {
        if (cached_stack.contains(ids) || !this->find_element(instance, ids)) {
            continue;
        }

        instance->access_scaled_rect() = {element->get_x(), element->get_y(), element->get_width(), element->get_height()};
        instance->on_sync_position();

        cached_stack.add(ids);
        this->fix_rect(instance, cached_stack);
    }
}
/* End of fix rects. */

void ekg_core::set_instances(SDL_Window *&sdl_window) {
    this->sdl_window_instance = sdl_window;
}

void ekg_core::init() {
    this->gpu_handler.init();
    this->font_manager.init();
    this->theme_service.init();
}

ekg_gpu_data_handler &ekg_core::get_gpu_handler() {
    return this->gpu_handler;
}

ekg_font &ekg_core::get_font_manager() {
    return this->font_manager;
}

void ekg_core::quit() {
    this->gpu_handler.quit();
    this->font_manager.quit();
}

void ekg_core::dispatch_todo_event(uint8_t flag) {
    ekgutil::add(this->todo_flags, flag);
}

bool ekg_core::find_element(ekg_element *&element, uint32_t id) {
    element = nullptr;

    for (ekg_element* &elements : this->concurrent_buffer) {
        if (elements != nullptr && elements->get_id() == id) {
            element = elements;
            return true;
        }
    }

    for (ekg_element* &elements : this->data) {
        if (elements != nullptr && elements->get_id() == id) {
            element = elements;
            return true;
        }
    }

    return false;
}

void ekg_core::force_reorder_stack(uint32_t &id) {
    this->forced_focused_element_id = id;
    this->dispatch_todo_event(ekgutil::action::FIXSTACK);
}

ekg_theme_service &ekg_core::get_theme_service() {
    return this->theme_service;
}
