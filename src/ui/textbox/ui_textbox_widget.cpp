/*
 * VOKEGPU EKG LICENSE
 *
 * Respect ekg license policy terms, please take a time and read it.
 * 1- Any "skidd" or "stole" is not allowed.
 * 2- Forks and pull requests should follow the license policy terms.
 * 3- For commercial use, do not sell without give credit to vokegpu ekg.
 * 4- For ekg users and users-programmer, we do not care, free to use in anything (utility, hacking, cheat, game, software).
 * 5- Malware, rat and others virus. We do not care.
 * 6- Do not modify this license under any instance.
 *
 * @VokeGpu 2023 all rights reserved.
 */
#include "ekg/ui/textbox/ui_textbox_widget.hpp"
#include "ekg/ui/textbox/ui_textbox.hpp"
#include "ekg/ekg.hpp"
#include "ekg/draw/draw.hpp"

std::string &ekg::ui::textbox_widget::get_cursor_emplace_text() {
    if (this->cursor[4] >= this->text_chunk_list.size()) {
        return this->text_chunk_list.emplace_back();
    }

    return this->text_chunk_list.at(this->cursor[4]);
}

void ekg::ui::textbox_widget::check_cursor_text_bounding() {
    if (!this->flag.hovered) {
        return;
    }

    auto ui {(ekg::ui::popup*) this->data};
    auto &rect {this->get_abs_rect()};
    auto &f_renderer {ekg::f_renderer(ui->get_font_size())};

    float x {rect.x + this->scroll[0]};
    float y {rect.y + this->scroll[1]};
    float text_height {f_renderer.get_text_height()};
    text_height += text_height * 0.5f;

    ekg::rect char_rect {};
    ekg::char_data char_data {};

    uint64_t total_it {};
    char chars {};
    ekg::vec4 &interact {ekg::interact()};
    int64_t bounding_it {-1};
    int64_t chunk_it {};
    float text_width {};
    uint64_t it {};
    uint64_t text_it {};

    for (std::string &text : this->text_chunk_list) {
        x = rect.x + this->scroll[0];
        text_width = f_renderer.get_text_width(text);

        for (it = 0; it < text.size(); it++) {
            chars = text.at(it);
            if (f_renderer.ft_bool_kerning && f_renderer.ft_uint_previous) {
                FT_Get_Kerning(f_renderer.ft_face, f_renderer.ft_uint_previous, chars, 0, &f_renderer.ft_vector_previous_char);
                x += static_cast<float>(f_renderer.ft_vector_previous_char.x >> 6);
            }
            
            char_data = f_renderer.allocated_char_data[chars];
            char_rect.x = x;
            char_rect.y = y;
            char_rect.w = char_data.wsize / 2;
            char_rect.h = text_height;
            text_it = it;

            if (ekg::rect_collide_vec(char_rect, interact)) {
                bounding_it = total_it;
                break;
            }

            char_rect.w += char_rect.w;
            if (ekg::rect_collide_vec(char_rect, interact)) {
                bounding_it = ekg::max(total_it++, (int32_t) text.size());
                break;
            }

            total_it++;
            x += char_data.wsize;
        }

        char_rect.x = rect.x;
        char_rect.y = y;
        char_rect.w = rect.w;
        char_rect.h = text_height;

        if (ekg::rect_collide_vec(char_rect, interact) && bounding_it == -1) {
            char_rect.w /= 2;
            if (ekg::rect_collide_vec(char_rect, interact)) {
                bounding_it = total_it - text.size();
            } else {
                bounding_it = total_it;
            }

            break;
        }

        if (bounding_it != -1) break;
        y += text_height;
        chunk_it++;
    }

    if (bounding_it != -1) {
        this->cursor[0] = bounding_it;
        this->cursor[1] = bounding_it;
        this->cursor[4] = chunk_it;
        this->cursor[5] = text_it;
        ekg::dispatch(ekg::env::redraw);
    }
}

void ekg::ui::textbox_widget::on_destroy() {
    abstract_widget::on_destroy();
}

void ekg::ui::textbox_widget::on_reload() {
    abstract_widget::on_reload();

    auto ui {(ekg::ui::textbox*) this->data};
    auto &rect {this->get_abs_rect()};
    auto &f_renderer {ekg::f_renderer(ui->get_font_size())};
    auto scaled_height {ui->get_scaled_height()};

    float text_width {f_renderer.get_text_width(ui->get_text())};
    float text_height {f_renderer.get_text_height()};
    float dimension_offset {text_height / 2};
    this->text_offset = ekg::find_min_offset(text_width, dimension_offset);

    this->dimension.w = ekg::min(this->dimension.w, text_height);
    this->dimension.h = (text_height + dimension_offset) * static_cast<float>(scaled_height);

    this->min_size.x = ekg::min(this->min_size.x, text_height);
    this->min_size.y = ekg::min(this->min_size.y, this->dimension.h);

    if (this->widget_side_text != ui->get_text()) {
        this->widget_side_text = ui->get_text();
        this->text_chunk_list.emplace_back() = this->widget_side_text;
        this->visible_chunk[0] = 0;
        this->visible_chunk[1] = this->text_chunk_list.size();
    }
}

void ekg::ui::textbox_widget::on_pre_event(SDL_Event &sdl_event) {
    abstract_widget::on_pre_event(sdl_event);
}

void ekg::ui::textbox_widget::on_event(SDL_Event &sdl_event) {
    abstract_widget::on_event(sdl_event);

    bool pressed {ekg::input::pressed() && ekg::input::pressed("textbox-activy")};
    bool released {ekg::input::released()};
    bool motion {ekg::input::motion()};

    if (this->flag.hovered && pressed) {
        this->flag.focused = true;
        this->check_cursor_text_bounding();
        ekg::reset(ekg::core->get_ui_timing());

        if (!this->is_high_frequency) {
            this->flag.absolute = true;
            ekg::update_high_frequency(this);
        }
    }

    if (this->flag.focused && !this->flag.hovered && motion) {
        this->flag.absolute = false;
    } else if (this->flag.focused && (sdl_event.type == SDL_KEYDOWN || sdl_event.type == SDL_TEXTINPUT)) {
        this->flag.absolute = true;
    }

    if (!this->flag.hovered && (released || pressed)) {
        this->flag.focused = false;
        this->flag.absolute = false;
    }

    if (!this->flag.focused) {
        return;
    }

    switch (sdl_event.type) {
    case SDL_KEYDOWN:
        switch (sdl_event.key.keysym.sym) {
        case SDLK_ESCAPE:
            this->flag.focused = false;
            this->flag.absolute = false;
            break;
        default:
            break;
        }

        break;
    case SDL_TEXTINPUT:
        std::string &emplace_text {this->get_cursor_emplace_text()};

        if (this->cursor[0] == this->cursor[1]) {
            uint64_t it {this->cursor[5]};
            emplace_text = emplace_text.substr(0, it) + sdl_event.text.text + emplace_text.substr(it, emplace_text.size());
            ekg::dispatch(ekg::env::redraw);
            ekg::reset(ekg::core->get_ui_timing());
            this->cursor[0] = this->cursor[1]++;
        }

        break;
    }
}

void ekg::ui::textbox_widget::on_post_event(SDL_Event &sdl_event) {
    abstract_widget::on_post_event(sdl_event);
}

void ekg::ui::textbox_widget::on_update() {
    abstract_widget::on_update();

    if (ekg::reach(ekg::core->get_ui_timing(), 500) && !this->redraw_cursor) {
        this->redraw_cursor = true;
        ekg::dispatch(ekg::env::redraw);
    } else if (!ekg::reach(ekg::core->get_ui_timing(), 500) && this->redraw_cursor) {
        this->redraw_cursor = false;
        ekg::dispatch(ekg::env::redraw);
    }

    this->is_high_frequency = this->flag.focused;
}

void ekg::ui::textbox_widget::on_draw_refresh() {
    abstract_widget::on_draw_refresh();
    auto ui {(ekg::ui::textbox*) this->data};
    auto &rect {this->get_abs_rect()};
    auto &f_renderer {ekg::f_renderer(ui->get_font_size())};
    auto &theme {ekg::theme()};
    auto &allocator {ekg::core->get_gpu_allocator()};

    ekg::draw::bind_scissor(this->data->get_id());
    ekg::draw::sync_scissor_pos(rect.x, rect.y);

    ekg::draw::rect(rect, theme.textbox_background);
    ekg::draw::rect(rect, theme.textbox_outline, ekg::drawmode::outline);
    
    uint64_t chunk_size {this->text_chunk_list.size()};
    std::string text {};

    float x {rect.x + this->scroll[0]};
    float y {rect.y + this->scroll[1]};
    float text_height {f_renderer.get_text_height()};

    x = static_cast<float>(static_cast<int32_t>(x));
    y = static_cast<float>(static_cast<int32_t>(y - f_renderer.offset_text_height));

    ekg::gpu::data &data {allocator.bind_current_data()};
    ekg::vec4 color {theme.textbox_string};

    data.shape_rect[0] = x;
    data.shape_rect[1] = y;
    data.shape_rect[2] = static_cast<float>(ekg::concave);
    data.shape_rect[3] = static_cast<float>(ekg::concave);

    data.material_color[0] = static_cast<uint8_t>(color.x);
    data.material_color[1] = static_cast<uint8_t>(color.y);
    data.material_color[2] = static_cast<uint8_t>(color.z);
    data.material_color[3] = static_cast<uint8_t>(color.w);

    ekg::rect vertices {};
    ekg::rect coordinates {};
    ekg::char_data char_data {};
    char chars {};

    ekg::vec2 cursor_pos {};
    uint64_t total_it {};
    bool cursor_out_of_str {};
    uint64_t text_size {};

    x = this->text_offset + this->scroll[0];
    y = this->text_offset + this->scroll[1];

    for (uint64_t it_chunk {this->visible_chunk[0]}; it_chunk < this->visible_chunk[1]; it_chunk++) {
        if (it_chunk > chunk_size) {
            continue;
        }

        text = this->text_chunk_list.at(it_chunk);
        x = this->text_offset + this->scroll[0];
        data.factor = 1;
        text_size = text.size();

        for (uint64_t it {}; it < text_size; it++) {
            chars = text.at(it);
            if (f_renderer.ft_bool_kerning && f_renderer.ft_uint_previous) {
                FT_Get_Kerning(f_renderer.ft_face, f_renderer.ft_uint_previous, chars, 0, &f_renderer.ft_vector_previous_char);
                x += static_cast<float>(f_renderer.ft_vector_previous_char.x >> 6);
            }

            char_data = f_renderer.allocated_char_data[chars];
            cursor_out_of_str = total_it + 1 == text_size && (this->cursor[0] == total_it + 1 && this->cursor[0] == this->cursor[1]);
            if ((total_it == this->cursor[0] && total_it == this->cursor[1]) || cursor_out_of_str) {
                cursor_pos.x = x + (char_data.wsize * cursor_out_of_str);
                cursor_pos.y = y;
            }

            vertices.x = x + char_data.left;
            vertices.y = y + f_renderer.full_height - char_data.top;

            vertices.w = char_data.w;
            vertices.h = char_data.h;

            coordinates.x = char_data.x;
            coordinates.w = vertices.w / f_renderer.full_width;
            coordinates.h = vertices.h / f_renderer.full_height;

            allocator.vertex2f(vertices.x, vertices.y);
            allocator.vertex2f(vertices.x, vertices.y + vertices.h);
            allocator.vertex2f(vertices.x + vertices.w, vertices.y + vertices.h);
            allocator.vertex2f(vertices.x + vertices.w, vertices.y + vertices.h);
            allocator.vertex2f(vertices.x + vertices.w, vertices.y);
            allocator.vertex2f(vertices.x, vertices.y);

            allocator.coord2f(coordinates.x, coordinates.y);
            allocator.coord2f(coordinates.x, coordinates.y + coordinates.h);
            allocator.coord2f(coordinates.x + coordinates.w, coordinates.y + coordinates.h);
            allocator.coord2f(coordinates.x + coordinates.w, coordinates.y + coordinates.h);
            allocator.coord2f(coordinates.x + coordinates.w, coordinates.y);
            allocator.coord2f(coordinates.x, coordinates.y);

            x += char_data.wsize;
            f_renderer.ft_uint_previous = chars;
            data.factor += static_cast<int32_t>(x + chars);
            total_it++;
        }

        y += text_height;
    }

    allocator.bind_texture(f_renderer.texture);
    allocator.dispatch();

    if (this->flag.focused && !ekg::reach(ekg::core->get_ui_timing(), 500)) {
        ekg::draw::rect(rect.x + cursor_pos.x - 2, rect.y + cursor_pos.y, 2, text_height, theme.textbox_cursor);
    }

    ekg::draw::bind_off_scissor();
}