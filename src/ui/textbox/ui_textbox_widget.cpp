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

/*
 * Cursor data indexes explain:
 * 0 - The A index.
 * 1 - The B index. (A == B: cursor position) else (A != B: Select bar)
 * 2 - The current cursor text chunk index.
 * 3 - The current cursor text index.
 * 4 - The last cursor text index.
 */
void ekg::ui::textbox_widget::move_cursor(int64_t x, int64_t y, bool magic) {
    if (magic) {
        // avadakedabra: im a sexy girl om pom pom
    } else {
        int64_t base_it {ekg::min(this->cursor[0] - this->cursor[3], (int64_t) 0)};
        std::string &emplace_text {this->get_cursor_emplace_text()};
        int64_t emplace_text_size {(int64_t) ekg::utf8length(emplace_text)};
        bool text_chunk_it_bounding_size {this->cursor[2] + 1 == this->text_chunk_list.size()};

        if (x < 0) {
            x = abs(x);
            this->cursor[0] -= x;
            this->cursor[3] -= x;
        } else if (x > 0) {
            x = abs(x);
            this->cursor[0] += x;
            this->cursor[3] += x;
        }

        if (y < 0) {
            if (this->cursor[2] == 0) {
                this->cursor[0] = 0;
                this->cursor[3] = 0;
            } else {
                this->cursor[2]--;
                emplace_text_size = (int64_t) ekg::utf8length(this->get_cursor_emplace_text());

                this->cursor[3] = this->cursor[4];
                this->cursor[3] = ekg::max(this->cursor[3], (int64_t) emplace_text_size);
                this->cursor[0] = base_it - (emplace_text_size - this->cursor[3]); 
            }
        } else if (y > 0) {
            if (text_chunk_it_bounding_size) {
                this->cursor[0] = base_it + emplace_text_size;
                this->cursor[3] = emplace_text_size;
            } else {
                this->cursor[0] = (base_it + emplace_text_size);
                this->cursor[2]++;

                emplace_text_size = (int64_t) ekg::utf8length(this->get_cursor_emplace_text());
                this->cursor[3] = this->cursor[4];
                this->cursor[3] = ekg::max(this->cursor[3], (int64_t) emplace_text_size);
                this->cursor[0] += this->cursor[3];        
            }
        }

        if (this->cursor[0] < base_it && this->cursor[2] > 0 && x != 0) {
            this->cursor[2]--;
            this->cursor[0] = base_it;
            this->cursor[3] = ekg::utf8length(this->get_cursor_emplace_text());
        }

        int64_t base_it_plus_emplace_text_size {base_it + emplace_text_size};
        if (this->cursor[0] > base_it_plus_emplace_text_size && text_chunk_it_bounding_size && x != 0) {
            this->cursor[0] = base_it_plus_emplace_text_size;
            this->cursor[3] = emplace_text_size;
        } else if (this->cursor[0] > base_it_plus_emplace_text_size && x != 0) {
            this->cursor[0] = base_it_plus_emplace_text_size;
            this->cursor[2]++;
            this->cursor[3] = 0;
        }

        this->cursor[0] = ekg::min(this->cursor[0], (int64_t) 0);
        this->cursor[1] = this->cursor[0];
        this->cursor[3] = ekg::min(this->cursor[3], (int64_t) 0);
        this->cursor[2] = ekg::max(this->cursor[2], (int64_t) this->text_chunk_list.size());

        if (x != 0) {
            this->cursor[4] = this->cursor[3];
        }

        ekg::reset(ekg::core->get_ui_timing());
        ekg::dispatch(ekg::env::redraw);
    }
}

void ekg::ui::textbox_widget::process_text(std::string_view text, ekg::ui::textbox_widget::action action, int64_t direction) {
    switch (action) {
    case ekg::ui::textbox_widget::action::addtext: {
        std::string &emplace_text {this->get_cursor_emplace_text()};
        if (this->cursor[0] == this->cursor[1] && !text.empty()) {
            int64_t it {this->cursor[3]};
            emplace_text = ekg::utf8substr(emplace_text, 0, it) + text.data() + ekg::utf8substr(emplace_text, it, ekg::utf8length(emplace_text));
            this->move_cursor(1, 0);
        }

        break;
    }

    case ekg::ui::textbox_widget::action::erasetext: {
        if (this->cursor[0] == this->cursor[1] && direction < 0 && this->cursor[0] > 0) {
            if (this->cursor[3] - 1 < 0 && this->cursor[2] > 0) {
                std::string stored_text {this->get_cursor_emplace_text()};
                this->move_cursor(-1, 0);
                this->text_chunk_list.erase(this->text_chunk_list.begin() + this->cursor[2] + 1);
                std::string &upper_line_text {this->get_cursor_emplace_text()};
                upper_line_text += stored_text;
            } else {
                std::string &emplace_text {this->get_cursor_emplace_text()};
                int64_t it {ekg::min(this->cursor[3] - 1, (int64_t) 0)};
                emplace_text = ekg::utf8substr(emplace_text, 0, it) + ekg::utf8substr(emplace_text, it + 1, ekg::utf8length(emplace_text));
                this->move_cursor(-1, 0);
            }
        } else if (this->cursor[0] == this->cursor[1] && direction > 0) {
            std::string &emplace_text {this->get_cursor_emplace_text()};
            int64_t emplace_text_size {(int64_t) ekg::utf8length(emplace_text)};
            bool text_chunk_it_bounding_size {this->cursor[2] + 1 == this->text_chunk_list.size()};

            if (this->cursor[3] >= emplace_text_size && !text_chunk_it_bounding_size) {
                emplace_text += this->text_chunk_list.at(this->cursor[2] + 1);
                this->text_chunk_list.erase(this->text_chunk_list.begin() + this->cursor[2] + 1);
            } else if (this->cursor[3] < emplace_text_size) {
                int64_t it {this->cursor[3]};
                emplace_text = ekg::utf8substr(emplace_text, 0, it) + ekg::utf8substr(emplace_text, it + 1, ekg::utf8length(emplace_text));
            }

            ekg::reset(ekg::core->get_ui_timing());
            ekg::dispatch(ekg::env::redraw);
        }

        break;
    }

    /*
     * @TODO Linked text chunk list:
     *
     * The complexity of this function is horrible,
     * for the moment it is okay, but soon should be rewrite,
     * this is not secure for low-performance hardwares. :cat2:
     */
    case ekg::ui::textbox_widget::action::breakline: {
        int64_t it {this->cursor[3]};

        std::string &emplace_text {this->get_cursor_emplace_text()};
        std::string previous {};
        std::string next {ekg::utf8substr(emplace_text, it, ekg::utf8length(emplace_text))};

        emplace_text = ekg::utf8substr(emplace_text, 0, it);
        this->text_chunk_list.emplace_back().clear();

        for (int64_t it {this->cursor[2] + 1}; it < this->text_chunk_list.size(); it++) {
            previous = this->text_chunk_list.at(it);
            this->text_chunk_list[it] = next;
            next = previous;
        }

        this->move_cursor(1, 0);
        break;
    }
    }
}

std::string &ekg::ui::textbox_widget::get_cursor_emplace_text() {
    if (this->cursor[2] > this->text_chunk_list.size() || this->text_chunk_list.empty()) {
        return this->text_chunk_list.emplace_back();
    }

    return this->text_chunk_list.at(this->cursor[2]);
}

void ekg::ui::textbox_widget::check_cursor_text_bounding() {
    if (!this->flag.hovered) {
        return;
    }

    auto ui {(ekg::ui::textbox*) this->data};
    auto &rect {this->get_abs_rect()};
    auto &f_renderer {ekg::f_renderer(ui->get_font_size())};

    float x {};
    float y {rect.y + this->embedded_scroll.scroll.y};

    ekg::rect char_rect {};
    ekg::char_data char_data {};

    ekg::vec4 &interact {ekg::interact()};
    uint64_t total_it {};
    char chars {};

    int64_t bounding_it {-1};
    int64_t chunk_it {};
    uint64_t text_it {};
    uint64_t it {};
    uint64_t text_size {};
    float text_width {};

    char32_t ui32char {};
    uint8_t ui8char {};
    std::string utf8string {};
    size_t stringsize {};
    size_t indexjump {};
    float cursor_line_thickness {2.0f};

    for (std::string &text : this->text_chunk_list) {
        x = rect.x + cursor_line_thickness + this->embedded_scroll.scroll.x;
        stringsize = 0;
        f_renderer.ft_uint_previous = 0;
        text_width = cursor_line_thickness;

        for (size_t it {}; it < text.size(); it++) {
            ui8char = static_cast<uint8_t>(text.at(it));
            ui32char = 0;
            indexjump = 0;

            if (ui8char <= 0x7F) {
                ui32char = static_cast<char32_t>(ui8char);
                utf8string = ui8char;
            } else if ((ui8char & 0xE0) == 0xC0) {
                indexjump = 1;
                utf8string = text.substr(it, 2);
                ui32char = ekg::char32str(utf8string);
            } else if ((ui8char & 0xF0) == 0xE0) {
                indexjump = 2;
                utf8string = text.substr(it, 3);
                ui32char = ekg::char32str(utf8string);
            } else if ((ui8char & 0xF8) == 0xF0) {
                indexjump = 3;
                utf8string = text.substr(it, 4);
                ui32char = ekg::char32str(utf8string);
            }

            if (f_renderer.ft_bool_kerning && f_renderer.ft_uint_previous) {
                FT_Get_Kerning(f_renderer.ft_face, f_renderer.ft_uint_previous, ui32char, 0, &f_renderer.ft_vector_previous_char);
                x += static_cast<float>(f_renderer.ft_vector_previous_char.x >> 6);
            }

            /*
             * Check if interact position collide with char.
             */
            
            char_data = f_renderer.allocated_char_data[ui32char];
            char_rect.x = x;
            char_rect.y = y;
            char_rect.w = char_data.wsize / 2;
            char_rect.h = this->text_height;

            if (ekg::rect_collide_vec(char_rect, interact)) {
                bounding_it = total_it;
                text_it = stringsize;
                break;
            }

            char_rect.w = char_data.wsize;
            if (ekg::rect_collide_vec(char_rect, interact)) {
                bounding_it = total_it + 1;
                text_it = stringsize + 1;
                break;
            }

            total_it++;
            x += char_data.wsize;
            it += indexjump;
            text_width += char_data.wsize;
            stringsize++;
            f_renderer.ft_uint_previous = ui32char;
        }

        /*
         * If interact position is not colliding with any char in this line, then set index to the end or begin of line.
         */

        char_rect.x = rect.x;
        char_rect.y = y;
        char_rect.w = rect.w;
        char_rect.h = this->text_height;

        if (ekg::rect_collide_vec(char_rect, interact) && bounding_it == -1) {
            char_rect.w = this->text_offset;
            if (ekg::rect_collide_vec(char_rect, interact)) {
                bounding_it = total_it - stringsize;
            } else {
                bounding_it = total_it;
                text_it = stringsize;
            }

            break;
        }

        if (bounding_it != -1) break;
        y += this->text_height;
        chunk_it++;
    }

    if (bounding_it != -1) {
        this->cursor[0] = bounding_it;
        this->cursor[1] = bounding_it;
        this->cursor[2] = chunk_it;
        this->cursor[3] = text_it;
        this->cursor[4] = this->cursor[3];
    } else {
        this->cursor[0] = total_it;
        this->cursor[1] = total_it;
        this->cursor[2] = chunk_it - (!this->text_chunk_list.empty());
        this->cursor[3] = stringsize;
        this->cursor[4] = this->cursor[3];
    }

    ekg::dispatch(ekg::env::redraw);
}

void ekg::ui::textbox_widget::unset_focus() {
    if (this->flag.focused) {
        ekg::set(this->flag.focused, false);
        auto *ui {(ekg::ui::textbox*) this->data};

        std::string compatibility_text {};
        if (!this->text_chunk_list.empty()) {
            for (std::string &text : this->text_chunk_list) {
                compatibility_text += text;
                compatibility_text += '\n';
            }

            compatibility_text = compatibility_text.substr(0, compatibility_text.size() - 1);
        }

        ui->set_text(compatibility_text);
        this->widget_side_text = compatibility_text;
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

    float text_width {f_renderer.get_text_width(ui->get_tag())};
    float dimension_offset {this->text_height / 2};

    this->text_height = f_renderer.get_text_height();
    this->text_offset = ekg::find_min_offset(text_width, dimension_offset);

    this->dimension.w = ekg::min(this->dimension.w, this->text_height);
    this->dimension.h = (this->text_height + dimension_offset) * static_cast<float>(scaled_height);

    this->min_size.x = ekg::min(this->min_size.x, this->text_height);
    this->min_size.y = ekg::min(this->min_size.y, this->dimension.h);

    if (this->widget_side_text != ui->get_text()) {
        this->widget_side_text = ui->get_text();
        ekg::utf8read(this->widget_side_text, this->text_chunk_list);
    }

    this->embedded_scroll.rect_mother = &rect;
    this->embedded_scroll.acceleration.y = this->text_offset + this->text_height;
    this->embedded_scroll.on_reload();
}

void ekg::ui::textbox_widget::on_pre_event(SDL_Event &sdl_event) {
    abstract_widget::on_pre_event(sdl_event);
}

void ekg::ui::textbox_widget::on_event(SDL_Event &sdl_event) {
    abstract_widget::on_event(sdl_event);

    bool pressed {ekg::input::pressed() && ekg::input::pressed("textbox-activy")};
    bool released {ekg::input::released()};
    bool motion {ekg::input::motion()};
    auto &rect {this->get_abs_rect()};

    this->embedded_scroll.flag.hovered = this->flag.hovered;
    this->embedded_scroll.on_event(sdl_event);
    this->flag.absolute = this->embedded_scroll.is_dragging_bar();

    if (this->flag.hovered && pressed && !this->flag.absolute) {
        ekg::set(this->flag.focused, this->flag.hovered);

        this->check_cursor_text_bounding();
        ekg::reset(ekg::core->get_ui_timing());
        ekg::dispatch(ekg::env::redraw);
        ekg::dispatch(ekg::env::swap);
    }

    this->flag.highlight = this->flag.hovered;
    if ((this->flag.focused || this->flag.hovered || this->flag.absolute) && !this->is_high_frequency) {
        ekg::update_high_frequency(this);
    }

    if (!this->flag.hovered && (released || pressed)) {
        this->unset_focus();
    }

    if (!this->flag.focused) {
        return;
    }

    switch (sdl_event.type) {
    case SDL_TEXTINPUT:
        this->process_text(sdl_event.text.text, ekg::ui::textbox_widget::action::addtext, 1);
        break;
    case SDL_KEYDOWN:
        switch (sdl_event.key.keysym.sym) {
        case SDLK_ESCAPE:
            this->unset_focus();
            break;
        default:
            if (ekg::input::pressed("textbox-action-up")) {
                this->move_cursor(0, -1);
            } else if (ekg::input::pressed("textbox-action-down")) {
                this->move_cursor(0, 1);
            } else if (ekg::input::pressed("textbox-action-left")) {
                this->move_cursor(-1, 0);
            } else if (ekg::input::pressed("textbox-action-right")) {
                this->move_cursor(1, 0);
            } else if (ekg::input::pressed("textbox-action-delete-left")) {
                this->process_text("backspace", ekg::ui::textbox_widget::action::erasetext, -1);
            } else if (ekg::input::pressed("textbox-action-delete-right")) {
                this->process_text("delete", ekg::ui::textbox_widget::action::erasetext, 1);
            } else if (ekg::input::pressed("textbox-action-break-line")) {
                this->process_text("return", ekg::ui::textbox_widget::action::breakline, 1);
            }
            break;
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

    this->embedded_scroll.on_update();
    this->is_high_frequency = this->flag.focused || this->flag.highlight || this->flag.absolute;
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

    this->embedded_scroll.clamp_scroll();
    float x {rect.x + this->embedded_scroll.scroll.x};
    float y {rect.y + this->embedded_scroll.scroll.y};

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
    data.factor = 1;

    ekg::rect vertices {};
    ekg::rect coordinates {};
    ekg::char_data char_data {};

    ekg::vec2 cursor_pos {};
    uint64_t total_it {};
    bool cursor_out_of_str {};
    uint64_t text_size {};
    bool render_cursor {};

    this->rect_text.x = x;
    this->rect_text.y = y;

    x = 0.0f;
    y = this->text_offset;

    this->visible_chunk[0] = 0;
    this->visible_chunk[1] = this->text_chunk_list.size();

    char32_t ui32char {};
    uint8_t ui8char {};
    std::string utf8string {};
    size_t stringsize {};
    size_t indexjump {};
    float cursor_line_thickness {2.0f};

    for (int64_t it_chunk {this->visible_chunk[0]}; it_chunk < this->visible_chunk[1]; it_chunk++) {
        if (it_chunk > chunk_size) {
            continue;
        }

        text = this->text_chunk_list.at(it_chunk);
        x = cursor_line_thickness;
        stringsize = 0;
        text_size = 0;
        f_renderer.ft_uint_previous = 0;

        if (!render_cursor && text.empty() && this->cursor[2] == it_chunk && this->cursor[0] == this->cursor[1]) {
            cursor_pos.x = x;
            cursor_pos.y = y;
            render_cursor = true;
        } else {
            text_size = ekg::utf8length(text); 
        }

        for (size_t it {}; it < text.size(); it++) {
            ui8char = static_cast<uint8_t>(text.at(it));
            ui32char = 0;
            indexjump = 0;

            if (ui8char <= 0x7F) {
                ui32char = static_cast<char32_t>(ui8char);
            } else if ((ui8char & 0xE0) == 0xC0) {
                indexjump = 1;
                utf8string = text.substr(it, 2);
                ui32char = ekg::char32str(utf8string);
            } else if ((ui8char & 0xF0) == 0xE0) {
                indexjump = 2;
                utf8string = text.substr(it, 3);
                ui32char = ekg::char32str(utf8string);
            } else if ((ui8char & 0xF8) == 0xF0) {
                indexjump = 3;
                utf8string = text.substr(it, 4);
                ui32char = ekg::char32str(utf8string);
            }

            if (f_renderer.ft_bool_kerning && f_renderer.ft_uint_previous) {
                FT_Get_Kerning(f_renderer.ft_face, f_renderer.ft_uint_previous, ui32char, 0, &f_renderer.ft_vector_previous_char);
                x += static_cast<float>(f_renderer.ft_vector_previous_char.x >> 6);
            }

            char_data = f_renderer.allocated_char_data[ui32char];

            cursor_out_of_str = stringsize + 1 == text_size && this->cursor[0] == total_it + 1;
            if (!render_cursor && (cursor_out_of_str || total_it == this->cursor[0]) && this->cursor[0] == this->cursor[1] && this->cursor[2] == it_chunk) {
                cursor_pos.x = x + (char_data.wsize * cursor_out_of_str);
                cursor_pos.y = y;
                render_cursor = true;
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
            f_renderer.ft_uint_previous = ui32char;
            data.factor += static_cast<int32_t>(x + y + ui32char);
            total_it++;
            it += indexjump;
            stringsize++;
        }

        if (x > this->rect_text.w) this->rect_text.w = x;
        y += this->text_height;
    }

    allocator.bind_texture(f_renderer.texture);
    allocator.dispatch();

    if (render_cursor && this->flag.focused && !ekg::reach(ekg::core->get_ui_timing(), 500)) {
        ekg::draw::rect(rect.x + this->embedded_scroll.scroll.x + cursor_pos.x, rect.y + this->embedded_scroll.scroll.y + cursor_pos.y, cursor_line_thickness, this->text_height, theme.textbox_cursor);
    }

    this->rect_text.h = y + this->text_offset;
    this->embedded_scroll.rect_child = this->rect_text;
    this->embedded_scroll.on_draw_refresh();

    ekg::draw::bind_off_scissor();
}