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
void ekg::ui::textboxwidget::check_largest_text_width(bool update_ui_data_text_together) {
    auto ui {(ekg::ui::textbox*) this->data};
    auto &f_renderer {ekg::f_renderer(ui->get_font_size())};

    this->rect_text.w = 0.0f;
    std::string compatibility_text {};

    uint64_t text_chunk_size {this->text_chunk_list.size()};
    for (uint64_t it {}; it < text_chunk_size; it++) {
        std::string &text = this->text_chunk_list.at(it);
        this->rect_text.w = std::max(this->rect_text.w, f_renderer.get_text_width(text));

        if (!update_ui_data_text_together) {
            continue;
        }

        compatibility_text += text;
        if (it < text_chunk_size - 1) {
            compatibility_text += '\n';
        }
    }

    if (update_ui_data_text_together) {
        ui->set_text(compatibility_text);
        this->widget_side_text = compatibility_text;
    }

    this->rect_text.w += this->text_offset * 2.0f;
}

void ekg::ui::textboxwidget::move_cursor(ekg::ui::textboxwidget::cursorpos &cursor, int64_t x, int64_t y) {
    int64_t base_it {ekg::min(cursor.index - cursor.text_index, (int64_t) 0)};
    std::string &emplace_text {this->get_cursor_emplace_text(cursor)};
    int64_t emplace_text_size {(int64_t) ekg::utf8length(emplace_text)};

    bool text_chunk_it_bounding_size {cursor.chunk_index + 1 == this->text_chunk_list.size()};
    bool check_cursor_x {x != 0};

    if (x < 0) {
        x = abs(x);
        cursor.index -= x;
        cursor.text_index -= x;
        x = -1;
    } else if (x > 0) {
        x = abs(x);
        cursor.index += x;
        cursor.text_index += x;
        x = 1;
    }

    if (y < 0) {
        if (cursor.chunk_index == 0) {
            cursor.index = 0;
            cursor.text_index = 0;
        } else {
            cursor.chunk_index--;
            emplace_text_size = (int64_t) ekg::utf8length(this->get_cursor_emplace_text(cursor));

            cursor.text_index = cursor.last_text_index;
            cursor.text_index = ekg::max(cursor.text_index, (int64_t) emplace_text_size);
            cursor.index = base_it - (emplace_text_size - cursor.text_index); 
        }
    } else if (y > 0) {
        if (text_chunk_it_bounding_size) {
            cursor.index = base_it + emplace_text_size;
            cursor.text_index = emplace_text_size;
        } else {
            cursor.index = (base_it + emplace_text_size);
            cursor.chunk_index++;

            emplace_text_size = (int64_t) ekg::utf8length(this->get_cursor_emplace_text(cursor));
            cursor.text_index = cursor.last_text_index;
            cursor.text_index = ekg::max(cursor.text_index, (int64_t) emplace_text_size);
            cursor.index += cursor.text_index;
        }
    }

    if (cursor.index < base_it && cursor.chunk_index > 0 && check_cursor_x) {
        cursor.chunk_index--;
        cursor.index = base_it;
        cursor.text_index = ekg::utf8length(this->get_cursor_emplace_text(cursor));
        y = -1;
    }

    int64_t base_it_plus_emplace_text_size {base_it + emplace_text_size};
    if (cursor.index > base_it_plus_emplace_text_size && text_chunk_it_bounding_size && check_cursor_x) {
        cursor.index = base_it_plus_emplace_text_size;
        cursor.text_index = emplace_text_size;
        y = -1;
    } else if (cursor.index > base_it_plus_emplace_text_size && check_cursor_x) {
        cursor.index = base_it_plus_emplace_text_size;
        cursor.chunk_index++;
        cursor.text_index = 0;
        y = 1;
    }

    cursor.index = ekg::min(cursor.index, (int64_t) 0);
    cursor.text_index = ekg::min(cursor.text_index, (int64_t) 0);
    cursor.chunk_index = ekg::max(cursor.chunk_index, (int64_t) this->text_chunk_list.size());

    if (check_cursor_x) {
        cursor.last_text_index = cursor.text_index;
    }

    std::string &current_emplace_text {this->get_cursor_emplace_text(cursor)};
    auto ui {(ekg::ui::textbox*) this->data};
    auto &f_renderer {ekg::f_renderer(ui->get_font_size())};
    auto &rect {this->get_abs_rect()};

    const ekg::vec2 cursor_pos {
        rect.x + this->embedded_scroll.scroll.x + (f_renderer.get_text_width(ekg::utf8substr(current_emplace_text, 0, cursor.text_index))),
        rect.y + this->embedded_scroll.scroll.y + this->text_offset + (static_cast<float>(cursor.chunk_index) * this->text_height)
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

void ekg::ui::textboxwidget::process_text(ekg::ui::textboxwidget::cursor &cursor, std::string_view text, ekg::ui::textboxwidget::action action, int64_t direction) {
    auto ui {(ekg::ui::textbox*) this->data};
    if (!(this->is_ui_enabled = ui->is_enabled())) {
        return;
    }

    switch (action) {
    case ekg::ui::textboxwidget::action::addtext: {
        std::string &emplace_text {this->get_cursor_emplace_text(cursor.pos[0])};
        if (cursor.pos[0] == cursor.pos[1] && !text.empty()) {
            int64_t it {cursor.pos[0].text_index};
            emplace_text = ekg::utf8substr(emplace_text, 0, it) + text.data() + ekg::utf8substr(emplace_text, it, ekg::utf8length(emplace_text));
            this->move_cursor(cursor.pos[0], 1, 0);
        }

        break;
    }

    case ekg::ui::textboxwidget::action::erasetext: {
        if (cursor.pos[0] == cursor.pos[1] && direction < 0 && (cursor.pos[0].index > 0 || cursor.pos[0].chunk_index > 0)) {
            if (cursor.pos[0].text_index - 1 < 0 && cursor.pos[0].chunk_index > 0) {
                std::string stored_text {this->get_cursor_emplace_text(cursor.pos[0])};
                this->move_cursor(cursor.pos[0], -1, 0);
                this->text_chunk_list.erase(this->text_chunk_list.begin() + cursor.pos[0].chunk_index + 1);
                std::string &upper_line_text {this->get_cursor_emplace_text(cursor.pos[0])};
                upper_line_text += stored_text;
            } else {
                std::string &emplace_text {this->get_cursor_emplace_text(cursor.pos[0])};
                int64_t it {ekg::min(cursor.pos[0].text_index - 1, (int64_t) 0)};
                emplace_text = ekg::utf8substr(emplace_text, 0, it) + ekg::utf8substr(emplace_text, it + 1, ekg::utf8length(emplace_text));
                this->move_cursor(cursor.pos[0], -1, 0);
            }
        } else if (cursor.pos[0] == cursor.pos[1] && direction > 0) {
            std::string &emplace_text {this->get_cursor_emplace_text(cursor.pos[0])};
            int64_t emplace_text_size {(int64_t) ekg::utf8length(emplace_text)};
            bool text_chunk_it_bounding_size {cursor.pos[0].chunk_index + 1 == this->text_chunk_list.size()};

            if (cursor.pos[0].text_index >= emplace_text_size && !text_chunk_it_bounding_size) {
                emplace_text += this->text_chunk_list.at(cursor.pos[0].chunk_index + 1);
                this->text_chunk_list.erase(this->text_chunk_list.begin() + cursor.pos[0].chunk_index + 1);
            } else if (cursor.pos[0].text_index < emplace_text_size) {
                int64_t it {cursor.pos[0].text_index};
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
    case ekg::ui::textboxwidget::action::breakline: {
        int64_t it {cursor.pos[0].text_index};

        std::string &emplace_text {this->get_cursor_emplace_text(cursor.pos[0])};
        std::string previous {};
        std::string next {ekg::utf8substr(emplace_text, it, ekg::utf8length(emplace_text))};

        emplace_text = ekg::utf8substr(emplace_text, 0, it);
        this->text_chunk_list.emplace_back().clear();

        for (it = cursor.pos[0].chunk_index + 1; it < this->text_chunk_list.size(); it++) {
            previous = this->text_chunk_list.at(it);
            this->text_chunk_list[it] = next;
            next = previous;
        }

        this->move_cursor(cursor.pos[0], 1, 0);
        ekg::reset(ekg::core->get_ui_timing());
        ekg::dispatch(ekg::env::redraw);

        break;
    }
    }

    cursor.pos[1] = cursor.pos[0];
    this->check_largest_text_width(true);
}

std::string &ekg::ui::textboxwidget::get_cursor_emplace_text(ekg::ui::textboxwidget::cursorpos &cursor) {
    if (cursor.chunk_index > this->text_chunk_list.size() || this->text_chunk_list.empty()) {
        return this->text_chunk_list.emplace_back();
    }

    return this->text_chunk_list.at(cursor.chunk_index);
}

void ekg::ui::textboxwidget::check_cursor_text_bounding(ekg::ui::textboxwidget::cursor &cursor, bool reset_second_cursor_pos) {
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

    int64_t chunk_it {};
    int64_t bounding_it {-1};

    uint64_t total_it {};
    uint64_t text_it {};
    uint64_t it {};

    uint8_t ui8char {};
    char32_t ui32char {};

    size_t utf_char_index {};
    std::string utf8string {};

    for (std::string &text : this->text_chunk_list) {
        x = rect.x + this->rect_cursor.w + this->embedded_scroll.scroll.x;
        utf_char_index = 0;
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
                text_it = utf_char_index;
                break;
            }

            char_rect.w += char_rect.w;
            if (ekg::rect_collide_vec(char_rect, interact)) {
                bounding_it = total_it + 1;
                text_it = utf_char_index + 1;
                break;
            }

            total_it++;
            utf_char_index++;
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
                bounding_it = total_it - utf_char_index;
            } else {
                bounding_it = total_it;
                text_it = utf_char_index;
            }

            break;
        }

        if (bounding_it != -1) break;
        y += this->text_height;
        chunk_it++;
    }

    uint64_t cursor_pos_index {};
    if (reset_second_cursor_pos == false && cursor.pos[1].index > cursor.pos[0].index) {
        cursor_pos_index = 1;
    }
    
    if (bounding_it != -1) {
        cursor.pos[cursor_pos_index].index = bounding_it;
        cursor.pos[cursor_pos_index].chunk_index = chunk_it;
        cursor.pos[cursor_pos_index].text_index = text_it;
        cursor.pos[cursor_pos_index].last_text_index = text_it;
    } else {
        cursor.pos[cursor_pos_index].index = total_it;
        cursor.pos[cursor_pos_index].chunk_index = chunk_it - (!this->text_chunk_list.empty());
        cursor.pos[cursor_pos_index].text_index = utf_char_index;
        cursor.pos[cursor_pos_index].last_text_index = text_it;
    }

    if (reset_second_cursor_pos == false) {
        cursor.pos[cursor_pos_index] = cursor.pos[!cursor_pos_index];
    } else {
        cursor.pos[cursor_pos_index] = cursor.pos[1];
    }

    cursor.pos[1] = cursor.pos[0];

    ekg::dispatch(ekg::env::redraw);
}

void ekg::ui::textboxwidget::on_destroy() {

}

void ekg::ui::textboxwidget::on_reload() {
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

        this->check_largest_text_width(false);
        this->rect_text.h = (this->text_height * this->text_chunk_list.size()) + (this->text_offset * 2.0f);
        
        float vertical_scroll_limit {this->rect_text.h - rect.h};
        float new_text_height_diff {this->text_height * (static_cast<float>(static_cast<int64_t>(this->text_chunk_list.size()) - previous_size) + 1.0f)};

        /*
         * If the difference between the new and old texts,
         * is nearest of scrolling y, it should follow the scrolling.
         */
        if (vertical_scroll_limit > 0 && this->embedded_scroll.scroll.w < -(vertical_scroll_limit - new_text_height_diff)) {
            this->embedded_scroll.scroll.w = -vertical_scroll_limit;
        }

        if (!this->is_high_frequency) {
            ekg::update_high_frequency(this);
        }
    }

    this->embedded_scroll.rect_child = this->rect_text;
    this->embedded_scroll.rect_mother = &rect;
    this->embedded_scroll.acceleration.y = this->text_height + (this->text_offset * 2.0f);
    this->embedded_scroll.widget_id = this->data->get_id();
    this->embedded_scroll.on_reload();
}

void ekg::ui::textboxwidget::on_pre_event(SDL_Event &sdl_event) {
    abstract_widget::on_pre_event(sdl_event);
    this->embedded_scroll.on_pre_event(sdl_event);
    this->flag.absolute = this->embedded_scroll.is_dragging_bar() || this->embedded_scroll.flag.activy || this->flag.state;
}

void ekg::ui::textboxwidget::on_event(SDL_Event &sdl_event) {
    bool pressed {ekg::input::pressed() && ekg::input::action("textbox-activy")};
    bool released {ekg::input::released()};
    bool motion {ekg::input::motion()};
    auto &rect {this->get_abs_rect()};

    this->embedded_scroll.on_event(sdl_event);

    if (this->flag.hovered && pressed) {
        ekg::set(this->flag.focused, this->flag.hovered);
        ekg::reset(ekg::core->get_ui_timing());
        ekg::dispatch(ekg::env::redraw);
        ekg::dispatch(ekg::env::swap);
        this->check_cursor_text_bounding(this->loaded_multi_cursor_list.at(0), true);
        this->flag.state = this->flag.hovered;
    } else if (this->flag.state && motion) {
        this->check_cursor_text_bounding(this->loaded_multi_cursor_list.at(0), false);
    }

    if (released) {
        this->flag.state = false;
    }

    this->flag.highlight = this->flag.hovered;
    if ((this->flag.focused || this->flag.hovered || this->flag.absolute) && !this->is_high_frequency) {
        ekg::update_high_frequency(this);
    }

    if (!this->flag.hovered && (released || pressed)) {
        ekg::set(this->flag.focused, false);
    }

    if (!this->flag.focused) {
        return;
    }

    switch (sdl_event.type) {
    case SDL_TEXTINPUT:
        for (ekg::ui::textboxwidget::cursor &cursor : this->loaded_multi_cursor_list) {
            this->process_text(cursor, sdl_event.text.text, ekg::ui::textboxwidget::action::addtext, 1);
        }
        break;
    case SDL_KEYDOWN:
        for (ekg::ui::textboxwidget::cursor &cursor : this->loaded_multi_cursor_list) {
            switch (sdl_event.key.keysym.sym) {
            case SDLK_ESCAPE:
                ekg::set(this->flag.focused, false);
                break;
            default:
                int64_t cursor_dir[2] {};
                if (ekg::input::action("textbox-action-up")) {
                    cursor_dir[1] = -1;
                } else if (ekg::input::action("textbox-action-down")) {
                    cursor_dir[1] = 1;
                } else if (ekg::input::action("textbox-action-left")) {
                    cursor_dir[0] = -1;
                } else if (ekg::input::action("textbox-action-right")) {
                    cursor_dir[0] = 1;
                } else if (ekg::input::action("textbox-action-delete-left")) {
                    this->process_text(cursor, "backspace", ekg::ui::textboxwidget::action::erasetext, -1);
                } else if (ekg::input::action("textbox-action-delete-right")) {
                    this->process_text(cursor, "delete", ekg::ui::textboxwidget::action::erasetext, 1);
                } else if (ekg::input::action("textbox-action-break-line")) {
                    this->process_text(cursor, "return", ekg::ui::textboxwidget::action::breakline, 1);
                }
    
                if (cursor_dir[0] != 0 || cursor_dir[1] != 0) {
                    this->move_cursor(cursor.pos[0], cursor_dir[0], cursor_dir[1]);
                    cursor.pos[1] = cursor.pos[0];
                }

                break;
            }

            break;
        }
    }
}

void ekg::ui::textboxwidget::on_post_event(SDL_Event &sdl_event) {
    abstract_widget::on_post_event(sdl_event);
    this->embedded_scroll.flag.hovered = false;
    this->embedded_scroll.flag.activy = false;
}

void ekg::ui::textboxwidget::on_update() {
    if (ekg::reach(ekg::core->get_ui_timing(), 500) && !this->redraw_cursor) {
        this->redraw_cursor = true;
    } else if (!ekg::reach(ekg::core->get_ui_timing(), 500) && this->redraw_cursor) {
        this->redraw_cursor = false;
    }

    this->embedded_scroll.on_update();
    this->is_high_frequency = this->embedded_scroll.check_activy_state(this->flag.focused || this->flag.hovered);
}

int32_t ekg::ui::textboxwidget::find_cursor(ekg::ui::textboxwidget::cursor &target_cursor, int64_t total_it, int64_t it_chunk, bool last_line_utf_char_index) {
    bool main_index {};
    for (ekg::ui::textboxwidget::cursor &cursor : this->loaded_multi_cursor_list) {
        main_index = ((last_line_utf_char_index && cursor.pos[0].index == total_it + 1) || cursor.pos[0].index == total_it) && cursor.pos[0].chunk_index == it_chunk;
        if (main_index || (((last_line_utf_char_index && cursor.pos[1].index == total_it + 1) || cursor.pos[1].index == total_it) && cursor.pos[1].chunk_index == it_chunk)) {
            target_cursor = cursor;
            return main_index ? 0 : 1;
        }
    }

    return -1;
}

void ekg::ui::textboxwidget::on_draw_refresh() {
    auto ui {(ekg::ui::textbox*) this->data};
    auto &rect {this->get_abs_rect()};
    auto &f_renderer {ekg::f_renderer(ui->get_font_size())};
    auto &theme {ekg::theme()};
    auto &allocator {ekg::core->get_gpu_allocator()};

    ekg::draw::bind_scissor(this->data->get_id());
    ekg::draw::sync_scissor(rect, ui->get_parent_id());
    ekg::draw::rect(rect, theme.textbox_background);

    this->embedded_scroll.clamp_scroll();
    float x {rect.x + this->embedded_scroll.scroll.x};
    float y {rect.y + this->embedded_scroll.scroll.y};
    float y_scroll {};

    x = static_cast<float>(static_cast<int32_t>(x));
    y = static_cast<float>(static_cast<int32_t>(y - f_renderer.offset_text_height));

    ekg::gpu::data &data {allocator.bind_current_data()};
    ekg::vec4 color {theme.textbox_string};

    if (allocator.is_out_of_scissor_rect()) {
        ekg::draw::bind_off_scissor();
        return;
    }

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

    this->rect_text.x = x;
    this->rect_text.y = y;

    x = 0.0f;
    y = this->text_offset;

    bool utf_char_last_index {};
    bool render_cursor {};

    int64_t text_size {};
    uint64_t total_it {};

    char32_t ui32char {};
    uint8_t ui8char {};
    std::string utf8string {};
    std::string text {};

    uint64_t chunk_size {this->text_chunk_list.size()};
    uint64_t utf_char_index {};
    uint64_t it {};

    this->rect_cursor.w = 2.0f;
    this->rect_cursor.h = this->text_height;
    this->is_ui_enabled = ui->is_enabled();

    if (this->loaded_multi_cursor_list.empty()) {
        this->loaded_multi_cursor_list.emplace_back();
    }

    std::vector<ekg::rect> cursor_draw_data_list {};
    ekg::ui::textboxwidget::cursor cursor {};

    int32_t cursor_pos_index {};
    bool draw_cursor {this->flag.focused && !ekg::reach(ekg::core->get_ui_timing(), 500)};


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
        utf_char_index = 0;

        if (text.empty() && draw_cursor && (cursor_pos_index = this->find_cursor(cursor, total_it, it_chunk, false)) != -1) {
            cursor_draw_data_list.emplace_back(ekg::rect {
                rect.x + x + this->embedded_scroll.scroll.x,
                rect.y + y + this->embedded_scroll.scroll.y,
                2.0f + (2.0f * (cursor.pos[0] != cursor.pos[1])),
                text_height
            });
        } else {
            text_size = ekg::utf8length(text);
        }

        data.factor += static_cast<int32_t>(x + 32);
        y_scroll = this->embedded_scroll.scroll.y + y;
        if (y_scroll > rect.h) {
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
            utf_char_last_index = utf_char_index + 1 == text_size;
            if (draw_cursor && (cursor_pos_index = this->find_cursor(cursor, total_it, it_chunk, utf_char_last_index)) != -1) {
                utf_char_last_index = utf_char_last_index && cursor.pos[cursor_pos_index].index == total_it + 1;
                if (cursor.pos[0] != cursor.pos[1] && cursor_pos_index == 0 && (utf_char_last_index || cursor.pos[cursor_pos_index].index == total_it)) {

                }

                cursor_draw_data_list.emplace_back(ekg::rect {
                    rect.x + x + (char_data.wsize * utf_char_last_index) + this->embedded_scroll.scroll.x,
                    rect.y + y + this->embedded_scroll.scroll.y,
                    2.0f,
                    text_height
                });
            }

            if (x + this->embedded_scroll.scroll.x < rect.w) {
                vertices.x = x + char_data.left;
                vertices.y = y + f_renderer.full_height - char_data.top;

                vertices.w = char_data.w;
                vertices.h = char_data.h;

                coordinates.x = char_data.x;
                coordinates.w = vertices.w / f_renderer.full_width;
                coordinates.h = vertices.h / f_renderer.full_height;

                allocator.push_back_geometry(vertices.x, vertices.y, coordinates.x, coordinates.y);
                allocator.push_back_geometry(vertices.x, vertices.y + vertices.h, coordinates.x, coordinates.y + coordinates.h);
                allocator.push_back_geometry(vertices.x + vertices.w, vertices.y + vertices.h, coordinates.x + coordinates.w, coordinates.y + coordinates.h);
                allocator.push_back_geometry(vertices.x + vertices.w, vertices.y + vertices.h, coordinates.x + coordinates.w, coordinates.y + coordinates.h);
                allocator.push_back_geometry(vertices.x + vertices.w, vertices.y, coordinates.x + coordinates.w, coordinates.y);
                allocator.push_back_geometry(vertices.x, vertices.y, coordinates.x, coordinates.y);
                data.factor += static_cast<int32_t>(x + ui32char);
            }

            f_renderer.ft_uint_previous = ui32char;
            total_it++;
            utf_char_index++;
            x += char_data.wsize;

            if (x + this->embedded_scroll.scroll.x > rect.w && render_cursor) {
                total_it += (text_size - utf_char_index);
                break;
            }
        }

        if (it_chunk > cursor.pos[0].chunk_index && it_chunk < cursor.pos[1].chunk_index) {
            cursor_draw_data_list.emplace_back(ekg::rect {
                rect.x + this->rect_cursor.w + this->embedded_scroll.scroll.x,
                rect.y + y + this->embedded_scroll.scroll.y,
                x,
                text_height
            });
        }

        if (y_scroll < 0.0f) {
            total_it += text_size;
        }

        y += this->text_height;
    }

    allocator.bind_texture(f_renderer.texture);
    allocator.dispatch();

    for (ekg::rect &rect : cursor_draw_data_list) {
        switch (static_cast<int32_t>(rect.w)) {
        case 2:
            ekg::draw::rect(rect, theme.textbox_cursor);
            break;
        default:
            ekg::draw::rect(rect, theme.textbox_select);
            break;
        }
    }

    this->rect_text.h = (this->text_height * this->text_chunk_list.size()) + (this->text_offset * 2.0f);
    this->embedded_scroll.rect_child = this->rect_text;
    this->embedded_scroll.on_draw_refresh();

    ekg::draw::rect(rect, theme.textbox_outline, ekg::drawmode::outline);
    ekg::draw::bind_off_scissor();
}