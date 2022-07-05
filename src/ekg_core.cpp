#include <ekg/ekg_core.hpp>
#include <list>

void ekg_core::process_event_section(SDL_Event &sdl_event) {
    this->focused_element_id = 0;

    for (uint32_t i = 0; i < this->sizeof_update_buffer; i++) {
        ekg_abstract_element* &element = this->update_buffer[i];

        if (element == nullptr || element->flag.flag_dead) {
            continue;
        }

        // Verify point overlap.
        element->on_pre_event_update(sdl_event);

        if (element->visibility == ekgutil::visibility::VISIBLE && element->flag.flag_over) {
            this->focused_element_id = element->id;
        }

        element->on_post_event_update(sdl_event);
    }

    this->sizeof_render_buffer = 0;
    this->render_buffer.fill(nullptr);

    for (uint32_t i = 0; i < this->sizeof_update_buffer; i++) {
        ekg_abstract_element* &element = this->update_buffer[i];

        if (element == nullptr || element->flag.flag_dead) {
            continue;
        }

        if (element->id == this->focused_element_id) {
            element->on_pre_event_update(sdl_event);
        }

        element->on_event(sdl_event);

        if (element->visibility == ekgutil::visibility::VISIBLE) {
            this->render_buffer[this->sizeof_render_buffer++] = element;
        }
    }

    if (ekgutil::contains(this->todo_flags, ekgutil::action::SWAPBUFFERS)) {
        this->swap_buffers();
        ekgutil::remove(this->todo_flags, ekgutil::action::SWAPBUFFERS);
    }
}

void ekg_core::process_update_section() {
    if (ekgutil::contains(this->todo_flags, ekgutil::SWAPBUFFERS)) {
        this->swap_buffers();
        ekgutil::remove(this->todo_flags, ekgutil::SWAPBUFFERS);
    }
}

void ekg_core::process_render_section() {
    ekggpu::vao();

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    this->gpu_handler.calc_view_ortho_2d();
    this->gpu_handler.draw();

    if (this->sizeof_render_buffer != 0) {
        ekggpu::invoke();

        for (uint32_t i = 0; i < this->sizeof_render_buffer; i++) {
            ekg_abstract_element *&element = this->render_buffer[i];
            element->on_draw_refresh();
        }

        ekggpu::revoke();

        this->sizeof_render_buffer = 0;
        this->render_buffer.fill(nullptr);
    }

    glBindVertexArray(0);
}

void ekg_core::add_element(ekg_abstract_element* &element) {
    element->id = this->last_id_used++;
    this->concurrent_buffer.push_back(element);
    element->on_draw_refresh();
    element->visibility = ekgutil::visibility::VISIBLE;

    ekgutil::add(this->todo_flags, ekgutil::action::SWAPBUFFERS);
}

/* Start of swap buffers. */
void ekg_core::swap_buffers() {
    // Clean the buffer render (not delete).
    this->sizeof_render_buffer = 0;
    this->render_buffer.fill(nullptr);

    for (uint32_t i = 0; i < this->sizeof_update_buffer; i++) {
        ekg_abstract_element* &element = this->update_buffer[i];

        if (element == nullptr) {
            continue;
        }

        if (element->flag.flag_dead) {
            delete element;
            this->update_buffer[i] = nullptr;
            this->sizeof_update_buffer--;

            continue;
        }

        if (element->visibility == ekgutil::visibility::VISIBLE) {
            this->render_buffer[this->sizeof_render_buffer++] = element;
        }
    }

    for (ekg_abstract_element* &elements : this->concurrent_buffer) {
        if (elements == nullptr) {
            continue;
        }

        this->update_buffer[this->sizeof_update_buffer++] = elements;

        if (elements->visibility == ekgutil::visibility::VISIBLE) {
            this->render_buffer[this->sizeof_render_buffer++] = elements;
        }
    }

    // Clean after push the buffers into main update buffer.
    this->concurrent_buffer.clear();
}
/* End of swap buffers. */

void ekg_core::set_instances(SDL_Window *&sdl_window) {
    this->sdl_window_instance = sdl_window;
}

void ekg_core::init() {
    this->gpu_handler.init();
}

ekg_gpu_data_handler &ekg_core::get_gpu_handler() {
    return this->gpu_handler;
}
