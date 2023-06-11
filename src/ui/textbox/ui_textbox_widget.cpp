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
 * This is not really optmised but I think it is okay.
 * @TODO write a better text width width largest.
 */
void ekg::ui::textbox_widget::check_largest_text_width() {
    auto ui {(ekg::ui::textbox*) this->data};
    auto &f_renderer {ekg::f_renderer(ui->get_font_size())};
    this->rect_text.w = 0.0f;

    for (std::string &text : this->text_chunk_list) {
        this->rect_text.w = std::max(this->rect_text.w, f_renderer.get_text_width(text));
    }

    this->rect_text.w += this->text_offset * 2.0f;
}

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
        bool check_cursor_x {x != 0};

        if (x < 0) {
            x = abs(x);
            this->cursor[0] -= x;
            this->cursor[3] -= x;
            x = -1;
        } else if (x > 0) {
            x = abs(x);
            this->cursor[0] += x;
            this->cursor[3] += x;
            x = 1;
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

        if (this->cursor[0] < base_it && this->cursor[2] > 0 && check_cursor_x) {
            this->cursor[2]--;
            this->cursor[0] = base_it;
            this->cursor[3] = ekg::utf8length(this->get_cursor_emplace_text());
            y = -1;
        }

        int64_t base_it_plus_emplace_text_size {base_it + emplace_text_size};
        if (this->cursor[0] > base_it_plus_emplace_text_size && text_chunk_it_bounding_size && check_cursor_x) {
            this->cursor[0] = base_it_plus_emplace_text_size;
            this->cursor[3] = emplace_text_size;
            y = -1;
        } else if (this->cursor[0] > base_it_plus_emplace_text_size && check_cursor_x) {
            this->cursor[0] = base_it_plus_emplace_text_size;
            this->cursor[2]++;
            this->cursor[3] = 0;
            y = 1;
        }

        this->cursor[0] = ekg::min(this->cursor[0], (int64_t) 0);
        this->cursor[1] = this->cursor[0];
        this->cursor[3] = ekg::min(this->cursor[3], (int64_t) 0);
        this->cursor[2] = ekg::max(this->cursor[2], (int64_t) this->text_chunk_list.size());

        if (check_cursor_x) {
            this->cursor[4] = this->cursor[3];
        }

        std::string &current_emplace_text {this->get_cursor_emplace_text()};
        auto ui {(ekg::ui::textbox*) this->data};
        auto &f_renderer {ekg::f_renderer(ui->get_font_size())};
        auto &rect {this->get_abs_rect()};

        const ekg::vec2 cursor_pos {
            rect.x + this->embedded_scroll.scroll.x + (f_renderer.get_text_width(ekg::utf8substr(current_emplace_text, 0, this->cursor[3]))),
            rect.y + this->embedded_scroll.scroll.y + this->text_offset + (static_cast<float>(this->cursor[2]) * this->text_height)
        };

        const ekg::vec4 cursor_outspace_screen {
            rect.x - cursor_pos.x,
            rect.y - cursor_pos.y,
            (cursor_pos.x + this->cursor_char_wsize[1] + this->cursor_char_wsize[2]) - (rect.x + rect.w - this->rect_cursor.w),
            (cursor_pos.y + this->rect_cursor.h) - (rect.y + rect.h - this->text_offset)
        };

        if (cursor_outspace_screen.x > 0.0f && (x < 0 || cursor_outspace_screen.x > this->cursor_char_wsize[0] + this->rect_cursor.w)) {
            this->embedded_scroll.scroll.z += cursor_outspace_screen.x;
        } else if (cursor_outspace_screen.z > 0.0f && (x > 0 || cursor_outspace_screen.z > this->cursor_char_wsize[1] + this->cursor_char_wsize[2] + this->rect_cursor.w)) {
            this->embedded_scroll.scroll.z -= cursor_outspace_screen.z;
        }

        /*
         * Instead of targeting the scroll using the render cursor pos, this use the current cursor chunk it.
         */

        if (cursor_outspace_screen.y > 0.0f && (y < 0 || cursor_outspace_screen.y > this->rect_cursor.h + this->text_offset)) {
            this->embedded_scroll.scroll.w += cursor_outspace_screen.y;
        } else if (cursor_outspace_screen.w > 0.0f && (y > 0 || cursor_outspace_screen.w > this->rect_cursor.h + this->rect_cursor.h + this->text_offset)) {
            this->embedded_scroll.scroll.w -= cursor_outspace_screen.w;
        }

        ekg::reset(ekg::core->get_ui_timing());
        ekg::dispatch(ekg::env::redraw);
    }
}

void ekg::ui::textbox_widget::process_text(std::string_view text, ekg::ui::textbox_widget::action action, int64_t direction) {
    std::string changed_text {};

    auto ui {(ekg::ui::textbox*) this->data};
    if (!(this->is_ui_enabled = ui->is_enabled())) {
        return;
    }

    switch (action) {
    case ekg::ui::textbox_widget::action::addtext: {
        std::string &emplace_text {this->get_cursor_emplace_text()};
        if (this->cursor[0] == this->cursor[1] && !text.empty()) {
            int64_t it {this->cursor[3]};
            emplace_text = ekg::utf8substr(emplace_text, 0, it) + text.data() + ekg::utf8substr(emplace_text, it, ekg::utf8length(emplace_text));
            this->move_cursor(1, 0);
        }

        changed_text = emplace_text;
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
                changed_text = upper_line_text;
            } else {
                std::string &emplace_text {this->get_cursor_emplace_text()};
                int64_t it {ekg::min(this->cursor[3] - 1, (int64_t) 0)};
                emplace_text = ekg::utf8substr(emplace_text, 0, it) + ekg::utf8substr(emplace_text, it + 1, ekg::utf8length(emplace_text));
                this->move_cursor(-1, 0);
                changed_text = emplace_text;
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

            changed_text = emplace_text;
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

        changed_text = emplace_text;
        this->move_cursor(1, 0);
        break;
    }
    }

    this->check_largest_text_width();
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
    ekg::vec4 &interact {ekg::interact()};
    uint64_t total_it {};

    int64_t bounding_it {-1};
    int64_t chunk_it {};
    uint64_t text_it {};
    uint64_t it {};

    char32_t ui32char {};
    uint8_t ui8char {};
    std::string utf8string {};
    size_t utf8_char_index {};

    for (std::string &text : this->text_chunk_list) {
        x = rect.x + this->rect_cursor.w + this->embedded_scroll.scroll.x;
        utf8_char_index = 0;
        f_renderer.ft_uint_previous = 0;

        for (it = 0; it < text.size(); it++) {
            ui8char = static_cast<uint8_t>(text.at(it));
            it += ekg::utf8checksequence(ui8char, ui32char, utf8string, text, it);

            if (f_renderer.ft_bool_kerning && f_renderer.ft_uint_previous) {
                FT_Get_Kerning(f_renderer.ft_face, f_renderer.ft_uint_previous, ui32char, 0, &f_renderer.ft_vector_previous_char);
                x += static_cast<float>(f_renderer.ft_vector_previous_char.x >> 6);
            }

            /*
             * Check if interact position collide with char.
             */

            char_rect.x = x;
            char_rect.y = y;
            char_rect.w = f_renderer.allocated_char_data[ui32char].wsize / 2;
            char_rect.h = this->text_height;

            if (ekg::rect_collide_vec(char_rect, interact)) {
                bounding_it = total_it;
                text_it = utf8_char_index;
                break;
            }

            char_rect.w += char_rect.w;
            if (ekg::rect_collide_vec(char_rect, interact)) {
                bounding_it = total_it + 1;
                text_it = utf8_char_index + 1;
                break;
            }

            total_it++;
            utf8_char_index++;
            f_renderer.ft_uint_previous = ui32char;
            x += char_rect.w;
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
                bounding_it = total_it - utf8_char_index;
            } else {
                bounding_it = total_it;
                text_it = utf8_char_index;
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
        this->cursor[3] = utf8_char_index;
        this->cursor[4] = this->cursor[3];
    }

    ekg::dispatch(ekg::env::redraw);
}

void ekg::ui::textbox_widget::unset_focus() {
    ekg::set(this->flag.focused, false);
    auto ui {(ekg::ui::textbox*) this->data};

    if (this->flag.focused && (this->is_ui_enabled = ui->is_enabled())) {
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

    this->dimension.w = ekg::min(this->dimension.w, text_width);
    this->dimension.h = (this->text_height + dimension_offset) * static_cast<float>(scaled_height);

    this->min_size.x = ekg::min(this->min_size.x, text_height);
    this->min_size.y = ekg::min(this->min_size.y, this->dimension.h);

    if (this->widget_side_text != ui->get_text()) {
        int64_t previous_size {static_cast<int64_t>(this->text_chunk_list.size())};

        this->text_chunk_list.clear();
        this->widget_side_text = ui->get_text();
        ekg::utf8read(this->widget_side_text, this->text_chunk_list);

        this->check_largest_text_width();
        this->rect_text.h = (this->text_height * this->text_chunk_list.size()) + (this->text_offset * 2.0f);
        
        float vertical_scroll_limit {this->rect_text.h - rect.h};
        float new_text_height_diff {this->text_height * (static_cast<float>(static_cast<int64_t>(this->text_chunk_list.size()) - previous_size) + 1.0f)};

        /*
         * If the difference between the new and old texts,
         * is nearest of scrolling y, it should follow the scrolling.
         */
        if (vertical_scroll_limit > 0 && this->embedded_scroll.scroll.w < -(vertical_scroll_limit - new_text_height_diff)) {
            this->embedded_scroll.scroll.w = -vertical_scroll_limit;
        
            if (!this->is_high_frequency) {
                ekg::update_high_frequency(this);
            }
        }
    }

    this->embedded_scroll.rect_mother = &rect;
    this->embedded_scroll.acceleration.y = this->text_height + (this->text_offset * 2.0f);
    this->embedded_scroll.on_reload();
}

void ekg::ui::textbox_widget::on_pre_event(SDL_Event &sdl_event) {
    abstract_widget::on_pre_event(sdl_event);
    this->flag.absolute = this->embedded_scroll.is_dragging_bar() || (this->flag.hovered && this->embedded_scroll.is_vertical_enabled);
}

void ekg::ui::textbox_widget::on_event(SDL_Event &sdl_event) {
    abstract_widget::on_event(sdl_event);

    bool pressed {ekg::input::pressed() && ekg::input::action("textbox-activy")};
    bool released {ekg::input::released()};
    bool motion {ekg::input::motion()};
    auto &rect {this->get_abs_rect()};

    this->embedded_scroll.flag.hovered = this->flag.hovered;
    this->embedded_scroll.flag.activy = this->flag.hovered;
    this->embedded_scroll.on_event(sdl_event);

    if (this->flag.hovered && pressed) {
        ekg::set(this->flag.focused, this->flag.hovered);
        ekg::reset(ekg::core->get_ui_timing());
        ekg::dispatch(ekg::env::redraw);
        ekg::dispatch(ekg::env::swap);
        this->check_cursor_text_bounding();
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
            if (ekg::input::action("textbox-action-up")) {
                this->move_cursor(0, -1);
            } else if (ekg::input::action("textbox-action-down")) {
                this->move_cursor(0, 1);
            } else if (ekg::input::action("textbox-action-left")) {
                this->move_cursor(-1, 0);
            } else if (ekg::input::action("textbox-action-right")) {
                this->move_cursor(1, 0);
            } else if (ekg::input::action("textbox-action-delete-left")) {
                this->process_text("backspace", ekg::ui::textbox_widget::action::erasetext, -1);
            } else if (ekg::input::action("textbox-action-delete-right")) {
                this->process_text("delete", ekg::ui::textbox_widget::action::erasetext, 1);
            } else if (ekg::input::action("textbox-action-break-line")) {
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
    } else if (!ekg::reach(ekg::core->get_ui_timing(), 500) && this->redraw_cursor) {
        this->redraw_cursor = false;
    }

    this->embedded_scroll.on_update();
    this->is_high_frequency = this->embedded_scroll.check_activy_state(this->flag.focused || this->flag.highlight || this->flag.absolute);
}

void ekg::ui::textbox_widget::on_draw_refresh() {
    abstract_widget::on_draw_refresh();
    auto ui {(ekg::ui::textbox*) this->data};
    auto &rect {this->get_abs_rect()};
    auto &f_renderer {ekg::f_renderer(ui->get_font_size())};
    auto &theme {ekg::theme()};
    auto &allocator {ekg::core->get_gpu_allocator()};

    ekg::draw::bind_scissor(this->data->get_id());
    ekg::draw::sync_scissor(rect, ui->get_parent_id());
    ekg::draw::rect(rect, theme.textbox_background);
    
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

    data.material_color[0] = color.x;
    data.material_color[1] = color.y;
    data.material_color[2] = color.z;
    data.material_color[3] = color.w;
    data.factor = 1;

    ekg::rect vertices {};
    ekg::rect coordinates {};
    ekg::char_data char_data {};

    uint64_t total_it {};
    bool cursor_out_of_str {};
    bool render_cursor {};

    this->rect_text.x = x;
    this->rect_text.y = y;

    x = 0.0f;
    y = this->text_offset;

    char32_t ui32char {};
    uint8_t ui8char {};
    std::string utf8string {};
    int64_t text_size {};
    size_t utf8_char_index {};
    size_t it {};
    float y_scroll {};


    /*
     * Visible chunk is used to reduce GPU allocament.
     */
    this->visible_chunk[0] = -1;
    this->visible_chunk[1] = -1;

    this->rect_cursor.w = 2.0f;
    this->rect_cursor.h = this->text_height;
    this->is_ui_enabled = ui->is_enabled();

    /*
     * 0 == previous char wsize
     * 1 == current char wisze
     * 2 == next char wsize
     */
    this->cursor_char_wsize[0] = 0.0f;
    this->cursor_char_wsize[1] = 0.0f;
    this->cursor_char_wsize[2] = 0.0f;

    /*
     * The texti iterator jump utf8 sequences.
     * For better performance, textbox does not render texts out of rect.
     */
    for (int64_t it_chunk {}; it_chunk < this->text_chunk_list.size(); it_chunk++) {
        text = this->text_chunk_list.at(it_chunk);
        x = this->rect_cursor.w;
        text_size = 0;
        f_renderer.ft_uint_previous = 0;
        utf8_char_index = 0;

        if (!render_cursor && text.empty() && this->cursor[2] == it_chunk && this->cursor[0] == this->cursor[1]) {
            this->rect_cursor.x = rect.x + x + this->embedded_scroll.scroll.x;
            this->rect_cursor.y = rect.y + y + this->embedded_scroll.scroll.y;
            render_cursor = true;
        } else {
            text_size = ekg::utf8length(text);
        }

        y_scroll = this->embedded_scroll.scroll.y + y;
        if (y_scroll > rect.h) {
            this->visible_chunk[1] = it_chunk;
            break;
        }

        switch (this->visible_chunk[0]) {
        case -1:
            if (y_scroll > 0.0f) {
                this->visible_chunk[0] = it_chunk;
            }
            break;
        }

        y_scroll += this->text_height;
        for (it = 0; it < text.size() && y_scroll > 0.0f; it++) {
            ui8char = static_cast<uint8_t>(text.at(it));
            it += ekg::utf8checksequence(ui8char, ui32char, utf8string, text, it);

            if (f_renderer.ft_bool_kerning && f_renderer.ft_uint_previous) {
                FT_Get_Kerning(f_renderer.ft_face, f_renderer.ft_uint_previous, ui32char, 0, &f_renderer.ft_vector_previous_char);
                x += static_cast<float>(f_renderer.ft_vector_previous_char.x >> 6);
            }

            char_data = f_renderer.allocated_char_data[ui32char];
            cursor_out_of_str = utf8_char_index + 1 == text_size && this->cursor[0] == total_it + 1;

            if (!render_cursor && (cursor_out_of_str || total_it == this->cursor[0]) && this->cursor[0] == this->cursor[1] && this->cursor[2] == it_chunk) {
                this->rect_cursor.x = rect.x + x + (char_data.wsize * cursor_out_of_str) + this->embedded_scroll.scroll.x;
                this->rect_cursor.y = rect.y + y + this->embedded_scroll.scroll.y; 
                this->cursor_char_wsize[1] = char_data.wsize;
                render_cursor = true;
            } else if (!render_cursor) {
                this->cursor_char_wsize[0] = char_data.wsize * (utf8_char_index != 0);
            } else if (this->cursor_char_wsize[2] == 0.0f) {
                this->cursor_char_wsize[2] = char_data.wsize;
            }

            if (x + char_data.wsize + this->embedded_scroll.scroll.x < -char_data.wsize) {
                this->visible_chunk[2] = this->cursor[2] == it_chunk ? utf8_char_index : this->visible_chunk[2];
            } else if (x + this->embedded_scroll.scroll.x < rect.w) {
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
                data.factor += static_cast<int32_t>(x + y + ui32char);
            }

            f_renderer.ft_uint_previous = ui32char;
            total_it++;
            utf8_char_index++;
            x += char_data.wsize;

            if (x + this->embedded_scroll.scroll.x > rect.w && render_cursor) {
                total_it += (text_size - utf8_char_index);
                break;
            }
        }

        if (y_scroll < 0.0f) total_it += text_size;
        y += this->text_height;
    }

    allocator.bind_texture(f_renderer.texture);
    allocator.dispatch();

    if (render_cursor && this->flag.focused && !ekg::reach(ekg::core->get_ui_timing(), 500)) {
        ekg::draw::rect(this->rect_cursor, theme.textbox_cursor);
    }

    this->rect_text.h = (this->text_height * this->text_chunk_list.size()) + (this->text_offset * 2.0f);
    this->embedded_scroll.rect_child = this->rect_text;
    this->embedded_scroll.on_draw_refresh();

    ekg::draw::rect(rect, theme.textbox_outline, ekg::drawmode::outline);
    ekg::draw::bind_off_scissor();
}