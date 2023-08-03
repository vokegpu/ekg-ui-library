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
#include "ekg/os/system_cursor.hpp"

void ekg::ui::textbox_widget::check_nearest_word(ekg::ui::textbox_widget::cursor &cursor, int64_t &x, int64_t &y) {
    if (!this->is_action_modifier_enable) {
        return;
    }

    ekg::ui::textbox_widget::cursor_pos &target_cursor_pos {cursor.target == cursor.pos[0].index ? cursor.pos[0] : cursor.pos[1]};
    std::string &emplace_text {this->get_cursor_emplace_text(target_cursor_pos)};
    uint64_t emplace_text_size {emplace_text.size()};

    if (emplace_text.empty() || (x == 0 || (x < 0 && target_cursor_pos.text_index == 0) ||
                                (x > 0 && target_cursor_pos.text_index == emplace_text_size))) {
        return;
    }

    bool is_dir_right {x > 0};
    bool should_update_last_char {};

    uint64_t it {};
    int64_t utf_it {};
    uint8_t ui8_char {};

    std::string utf_string {};
    char32_t ui32_char {};

    x = 0;

    /*
     * the cursor index text position is utf encoded,
     * not string coded, so it should start by begin
     * and not by cursor. 
     */

    while (it < emplace_text_size) {
        ui8_char = static_cast<uint8_t>(emplace_text.at(it));

        if (is_dir_right && utf_it >= target_cursor_pos.text_index) {
            if (ui8_char != 32) {
                should_update_last_char = true;
            } else if (should_update_last_char) {
                x = utf_it - target_cursor_pos.text_index;
                break;
            }
        } else if (!is_dir_right) {
            if (utf_it == target_cursor_pos.text_index) {
                break;
            }

            if (ui8_char == 32) {
                should_update_last_char = true;
            } else if (should_update_last_char) {
                x = utf_it;
                should_update_last_char = false;
            }
        }

        utf_it++;
        it += ekg::utf_check_sequence(ui8_char, ui32_char, utf_string, emplace_text, it) + 1;
    }

    /*
     * when the loop is finished, sometime it does not check any possible index,
     * so it is important to force at latest one.
     */
    if (is_dir_right && x == 0) {
        x = utf_it - target_cursor_pos.text_index;
        x = x == 0 ? 1 : x;
    } else if (!is_dir_right) {
        x = x - utf_it;
    }
}

/*
 * If the cursor is not selected, then target the first cursor,
 * therefore when the cursor is selected, the code check if moving cursor is
 * enabled, then set the A & B pos based on direction.
 */
void ekg::ui::textbox_widget::move_target_cursor(ekg::ui::textbox_widget::cursor &cursor, int64_t x, int64_t y) {
    ekg::reset(ekg::core->ui_timing);
    ekg::dispatch(ekg::env::redraw);

    ekg::ui::textbox_widget::cursor_pos target_cursor_pos {};
    if ((cursor.pos[0].index == cursor.target && this->is_action_select_enable) ||
       ((x < 0 || y < 0) && cursor.pos[0] != cursor.pos[1] && !this->is_action_select_enable)) {
        target_cursor_pos = cursor.pos[0];
    } else {
        target_cursor_pos = cursor.pos[1];
    }

    if (cursor.pos[0] == cursor.pos[1] || this->is_action_select_enable) {
        this->move_cursor(target_cursor_pos, x, y, cursor.pos[0] != cursor.pos[1]);
    }

    if (!this->is_action_select_enable) {
        cursor.pos[0] = target_cursor_pos;
        cursor.pos[1] = target_cursor_pos;
        cursor.pos[2] = target_cursor_pos;
        cursor.target = target_cursor_pos.index;
        return;
    }

    cursor.target = target_cursor_pos.index;
    if (cursor.target > cursor.pos[2].index) {
        cursor.pos[0] = cursor.pos[2];
        cursor.pos[1] = target_cursor_pos;
    } else {
        cursor.pos[0] = target_cursor_pos;
        cursor.pos[1] = cursor.pos[2];
    }
}

/*
 * This is not really optmised but I think it is okay.
 * @TODO write a better text width width largest.
 */
void ekg::ui::textbox_widget::check_largest_text_width(bool update_ui_data_text_together) {
    auto ui {(ekg::ui::textbox*) this->data};
    auto &f_renderer {ekg::f_renderer(ui->get_font_size())};

    this->rect_text.w = 0.0f;
    std::string compatibility_text {};
    this->total_utf_chars = 0;

    uint64_t text_chunk_size {this->text_chunk_list.size()};
    for (uint64_t it {}; it < text_chunk_size; it++) {
        std::string &text = this->text_chunk_list.at(it);
        this->rect_text.w = std::max(this->rect_text.w, f_renderer.get_text_width(text));
        this->total_utf_chars += ekg::utf_length(text);

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

void ekg::ui::textbox_widget::move_cursor(ekg::ui::textbox_widget::cursor_pos &cursor, int64_t x, int64_t y, bool is_selecting) {
    int64_t base_it {ekg::min(cursor.index - cursor.text_index, (int64_t) 0)};
    std::string &emplace_text {this->get_cursor_emplace_text(cursor)};
    int64_t emplace_text_size {(int64_t) ekg::utf_length(emplace_text)};

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
            emplace_text_size = (int64_t) ekg::utf_length(this->get_cursor_emplace_text(cursor));

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

            emplace_text_size = (int64_t) ekg::utf_length(this->get_cursor_emplace_text(cursor));
            cursor.text_index = cursor.last_text_index;
            cursor.text_index = ekg::max(cursor.text_index, (int64_t) emplace_text_size);
            cursor.index += cursor.text_index;
        }
    }

    /*
     * The selection should not take the size of text,
     * this is only able when the cursor A is equals to B.
     */
    bool out_of_index {cursor.index < base_it || (is_selecting && cursor.index == base_it)};

    if (out_of_index && cursor.chunk_index > 0 && check_cursor_x) {
        cursor.chunk_index--;
        y = -1;

        cursor.index = base_it;
        cursor.text_index = ekg::utf_length(this->get_cursor_emplace_text(cursor));
    }

    int64_t base_it_plus_emplace_text_size {base_it + emplace_text_size};
    out_of_index = cursor.index > base_it_plus_emplace_text_size ||
                  (is_selecting && cursor.index == base_it_plus_emplace_text_size);

    if (out_of_index && text_chunk_it_bounding_size && check_cursor_x) {
        cursor.index = base_it_plus_emplace_text_size;
        cursor.text_index = emplace_text_size;
        y = -1;
    } else if (out_of_index && check_cursor_x) {
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
        rect.x + this->embedded_scroll.scroll.x + (f_renderer.get_text_width(ekg::utf_substr(current_emplace_text, 0, cursor.text_index))),
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

    ekg::reset(ekg::core->ui_timing);
    ekg::dispatch(ekg::env::redraw);
}

void ekg::ui::textbox_widget::process_text(ekg::ui::textbox_widget::cursor &cursor,
                                           std::string_view text,
                                           ekg::ui::textbox_widget::action action, int64_t direction) {
    auto ui {(ekg::ui::textbox*) this->data};
    if (!(this->is_ui_enabled = ui->is_enabled()) && !(text == "clipboard" && this->is_clipboard_copy)) {
        return;
    }

    std::string &emplace_text_a {this->get_cursor_emplace_text(cursor.pos[0])};
    std::string &emplace_text_b {this->get_cursor_emplace_text(cursor.pos[1])};

    switch (action) {
    case ekg::ui::textbox_widget::action::add_text:
        if (this->is_action_modifier_enable && !(this->is_clipboard_cut || this->is_clipboard_copy || this->is_clipboard_paste)) {
            break;
        }

        /*
         * Always the tab is pressed, the process should add the spaces instead of \t,
         * it is cached to prevent useless iteration.
         */
        if (text == "\t") {
            uint8_t ui_tab_size {ui->get_tab_size()};

            if (this->cached_tab_size.size() != ui_tab_size) {
                this->cached_tab_size.clear();
                for (uint8_t it {}; it < ui_tab_size; it++) {
                    this->cached_tab_size += ' ';
                }
            }

            text = this->cached_tab_size;
            direction = static_cast<int64_t>(ui_tab_size);
        } else if (text == "clipboard") {
            text = "";

            if (this->is_clipboard_cut || this->is_clipboard_copy) {
                if (cursor.pos[0] != cursor.pos[1]) {
                    std::string copy_text {};
                    if (cursor.pos[0].chunk_index == cursor.pos[1].chunk_index) {
                        /*
                         * For some reason the STL substr implementation actually
                         * do the following addition for B (second parameter),
                         * (B += A), the subtract result between cursor 1 and 0
                         * should temp fix this. 
                         */
                        copy_text += ekg::utf_substr(emplace_text_a, cursor.pos[0].text_index, (cursor.pos[1].text_index - cursor.pos[0].text_index));
                    } else {
                        copy_text += ekg::utf_substr(emplace_text_a, cursor.pos[0].text_index, ekg::utf_length(emplace_text_a));
                        copy_text += '\n';

                        uint64_t it {static_cast<uint64_t>(cursor.pos[0].chunk_index + 1)};
                        uint64_t size {this->text_chunk_list.size()};

                        while (it < cursor.pos[1].chunk_index && it < size) {
                            copy_text += this->text_chunk_list.at(it);
                            if (!(it + 1 == size)) copy_text += '\n';
                            it++;
                        }

                        copy_text += ekg::utf_substr(emplace_text_b, 0, cursor.pos[1].text_index);
                    }

                    SDL_SetClipboardText(copy_text.c_str());
                }

                if (this->is_clipboard_copy) {
                    break;
                }
            }
        }

        emplace_text_a = ekg::utf_substr(emplace_text_a, 0, cursor.pos[0].text_index) + text.data() +
                         ekg::utf_substr(emplace_text_b, cursor.pos[1].text_index, ekg::utf_length(emplace_text_b));

        if (cursor.pos[0].chunk_index != cursor.pos[1].chunk_index) {
            this->text_chunk_list.erase(this->text_chunk_list.begin() + cursor.pos[0].chunk_index + 1,
                                        this->text_chunk_list.begin() + cursor.pos[1].chunk_index + 1);
        }

        if (this->is_clipboard_paste && SDL_HasClipboardText()) {
            text = SDL_GetClipboardText();
            direction = static_cast<int64_t>(ekg::utf_length(text));

            std::vector<std::string> utf_clipboard_decoded {};
            ekg::utf_decode(text, utf_clipboard_decoded);

            if (utf_clipboard_decoded.size() == 1) {
                emplace_text_a = ekg::utf_substr(emplace_text_a, 0, cursor.pos[0].text_index) + utf_clipboard_decoded.at(0) +
                                 ekg::utf_substr(emplace_text_a, cursor.pos[0].text_index, ekg::utf_length(emplace_text_a));

            } else if (utf_clipboard_decoded.size() > 1) {
                int64_t last_clipboard_list_index {static_cast<int64_t>(utf_clipboard_decoded.size() - 1)};
                std::string &last_clipboard_line {utf_clipboard_decoded.at(last_clipboard_list_index)};

                cursor.pos[1].text_index = ekg::utf_length(last_clipboard_line);
                cursor.pos[1].index = cursor.pos[0].index + direction;
                cursor.pos[1].chunk_index = cursor.pos[0].chunk_index + last_clipboard_list_index;
                cursor.pos[1].select_index = cursor.pos[1].text_index;

                std::string stored_text = ekg::utf_substr(emplace_text_a, cursor.pos[0].text_index, ekg::utf_length(emplace_text_a));
                emplace_text_a = ekg::utf_substr(emplace_text_a, 0, cursor.pos[0].text_index) + utf_clipboard_decoded.at(0);

                last_clipboard_line = last_clipboard_line + stored_text;
                this->text_chunk_list.insert(this->text_chunk_list.begin() + cursor.pos[0].chunk_index + 1, utf_clipboard_decoded.begin() + 1, utf_clipboard_decoded.end());

                cursor.pos[0] = cursor.pos[1];
                direction = 0;
            }
        }

        this->move_cursor(cursor.pos[0], direction, 0, cursor.pos[0] != cursor.pos[1]);
        break;

    case ekg::ui::textbox_widget::action::erase_text:
        if (this->is_action_modifier_enable && cursor.pos[0] == cursor.pos[1] && (cursor.pos[0].text_index > 0 || direction > 0)) {
            int64_t cursor_dir[2] {direction, 0};

            if (this->is_action_select_enable) {
                cursor_dir[0] = direction < 0 ? (-cursor.pos[0].text_index) :
                                                (static_cast<int64_t>(ekg::utf_length(emplace_text_a)) - cursor.pos[0].text_index);
            } else {
                this->check_nearest_word(cursor, cursor_dir[0], cursor_dir[1]);
            }

            this->move_cursor(direction < 0 ? cursor.pos[0] : cursor.pos[1], cursor_dir[0], cursor_dir[1], cursor.pos[0] != cursor.pos[1]);
        }

        if (cursor.pos[0] == cursor.pos[1] && direction < 0 && (cursor.pos[0].index > 0 || cursor.pos[0].chunk_index > 0)) {
            if (cursor.pos[0].text_index - 1 < 0 && cursor.pos[0].chunk_index > 0) {
                std::string stored_text {emplace_text_a};
                this->move_cursor(cursor.pos[0], -1, 0, cursor.pos[0] != cursor.pos[1]);
                this->text_chunk_list.erase(this->text_chunk_list.begin() + cursor.pos[0].chunk_index + 1);
                std::string &upper_line_text {this->get_cursor_emplace_text(cursor.pos[0])};
                upper_line_text += stored_text;
            } else {
                int64_t it {ekg::min(cursor.pos[0].text_index - 1, (int64_t) 0)};
                emplace_text_a = ekg::utf_substr(emplace_text_a, 0, it) + ekg::utf_substr(emplace_text_a, it + 1, ekg::utf_length(emplace_text_a));
                this->move_cursor(cursor.pos[0], -1, 0, cursor.pos[0] != cursor.pos[1]);
            }
        } else if (cursor.pos[0] != cursor.pos[1] && direction != 0) {
            emplace_text_a = ekg::utf_substr(emplace_text_a, 0, cursor.pos[0].text_index) +
                             ekg::utf_substr(emplace_text_b, cursor.pos[1].text_index, ekg::utf_length(emplace_text_b));

            if (cursor.pos[0].chunk_index != cursor.pos[1].chunk_index) {
                this->text_chunk_list.erase(this->text_chunk_list.begin() + cursor.pos[0].chunk_index + 1,
                                            this->text_chunk_list.begin() + cursor.pos[1].chunk_index + 1);
            }
        } else if (cursor.pos[0] == cursor.pos[1] && direction > 0) {
            int64_t emplace_text_size {(int64_t) ekg::utf_length(emplace_text_a)};
            bool text_chunk_it_bounding_size {cursor.pos[0].chunk_index + 1 == this->text_chunk_list.size()};

            if (cursor.pos[0].text_index >= emplace_text_size && !text_chunk_it_bounding_size) {
                emplace_text_a += this->text_chunk_list.at(cursor.pos[0].chunk_index + 1);
                this->text_chunk_list.erase(this->text_chunk_list.begin() + cursor.pos[0].chunk_index + 1);
            } else if (cursor.pos[0].text_index < emplace_text_size) {
                int64_t it {cursor.pos[0].text_index};
                emplace_text_a = ekg::utf_substr(emplace_text_a, 0, it) + ekg::utf_substr(emplace_text_a, it + 1, ekg::utf_length(emplace_text_a));
            }
        }

        break;

    case ekg::ui::textbox_widget::action::break_line:
        int64_t it {cursor.pos[0].text_index};
        int64_t cursor_dir[2] {0, 1};
        std::string line {};

        if (!this->is_action_modifier_enable) {
            line = ekg::utf_substr(emplace_text_a, it, ekg::utf_length(emplace_text_a));
            emplace_text_a = ekg::utf_substr(emplace_text_a, 0, it);

            cursor_dir[0] = 1;
            cursor_dir[1] = 0;
        }

        this->text_chunk_list.insert(this->text_chunk_list.begin() + cursor.pos[0].chunk_index + 1, line);
        this->move_cursor(cursor.pos[0], cursor_dir[0], cursor_dir[1], cursor.pos[0] != cursor.pos[1]);

        ekg::reset(ekg::core->ui_timing);
        ekg::dispatch(ekg::env::redraw);

        break;
    }

    if (!this->is_clipboard_copy) {
        cursor.pos[2] = cursor.pos[1] = cursor.pos[0];
        cursor.target = cursor.pos[0].index;
        this->check_largest_text_width(true);
    }

    ekg::reset(ekg::core->ui_timing);
    ekg::dispatch(ekg::env::redraw);
}

std::string &ekg::ui::textbox_widget::get_cursor_emplace_text(ekg::ui::textbox_widget::cursor_pos &cursor) {
    if (cursor.chunk_index > this->text_chunk_list.size() || this->text_chunk_list.empty()) {
        return this->text_chunk_list.emplace_back();
    }

    return this->text_chunk_list.at(cursor.chunk_index);
}

void ekg::ui::textbox_widget::check_cursor_text_bounding(ekg::ui::textbox_widget::cursor &cursor, bool reset_second_cursor_pos) {
    if (!this->flag.focused) {
        return;
    }

    auto ui {(ekg::ui::textbox*) this->data};
    auto &rect {this->get_abs_rect()};
    auto &f_renderer {ekg::f_renderer(ui->get_font_size())};

    float x {};
    float y {rect.y + this->embedded_scroll.scroll.y};

    ekg::rect char_rect {};
    ekg::vec4 interact {ekg::input::interact()};

    interact.x = ekg::clamp(interact.x, rect.x + this->rect_cursor.w, rect.x + rect.w - this->rect_cursor.w);
    interact.y = ekg::clamp(interact.y, rect.y + this->rect_cursor.w, rect.y + rect.h - this->rect_cursor.w);

    int64_t chunk_it {};
    int64_t bounding_it {-1};
    int64_t chunk_size {static_cast<int64_t>(this->text_chunk_list.size())};

    uint64_t total_it {};
    uint64_t text_it {};
    uint64_t it {};

    uint8_t ui8_char {};
    char32_t ui32_char {};

    size_t utf_char_index {};
    std::string utf_string {};
    bool is_chunk_it_at_end {};

    for (std::string &text : this->text_chunk_list) {
        x = rect.x + this->rect_cursor.w + this->embedded_scroll.scroll.x;
        utf_char_index = 0;
        f_renderer.ft_uint_previous = 0;
        is_chunk_it_at_end = chunk_it == chunk_size - 1;

        for (it = 0; it < text.size(); it++) {
            ui8_char = static_cast<uint8_t>(text.at(it));
            it += ekg::utf_check_sequence(ui8_char, ui32_char, utf_string, text, it);

            if (f_renderer.ft_bool_kerning && f_renderer.ft_uint_previous) {
                FT_Get_Kerning(f_renderer.ft_face, f_renderer.ft_uint_previous, ui32_char, 0, &f_renderer.ft_vector_previous_char);
                x += static_cast<float>(f_renderer.ft_vector_previous_char.x >> 6);
            }

            /*
             * Check if interact position collide with char.
             */

            char_rect.x = x;
            char_rect.y = y;
            char_rect.w = (f_renderer.allocated_char_data[ui32_char].wsize) / 2;
            char_rect.h = is_chunk_it_at_end ? rect.h : this->text_height + 1.0f;

            if (ekg::rect_collide_vec_precisely(char_rect, interact)) {
                bounding_it = total_it;
                text_it = utf_char_index;
                break;
            }

            char_rect.w += char_rect.w;
            if (ekg::rect_collide_vec_precisely(char_rect, interact)) {
                bounding_it = total_it + 1;
                text_it = utf_char_index + 1;
                break;
            }

            total_it++;
            utf_char_index++;
            f_renderer.ft_uint_previous = ui32_char;
            x += char_rect.w;
        }

        /*
         * If interact position is not colliding with any char in this line, then set index to the end or begin of line.
         */

        char_rect.x = rect.x;
        char_rect.y = y;
        char_rect.w = rect.w;
        char_rect.h = this->text_height + 1.0f;

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

    switch (reset_second_cursor_pos) {
    case true:
        if (bounding_it != -1) {
            cursor.pos[0].index = bounding_it;
            cursor.pos[0].chunk_index = chunk_it;
            cursor.pos[0].text_index = text_it;
            cursor.pos[0].last_text_index = text_it;
        } else {
            cursor.pos[0].index = total_it;
            cursor.pos[0].chunk_index = chunk_it - (!this->text_chunk_list.empty());
            cursor.pos[0].text_index = utf_char_index;
            cursor.pos[0].last_text_index = text_it;
        }

        cursor.pos[1] = cursor.pos[0];
        cursor.pos[2] = cursor.pos[0];
        cursor.target = cursor.pos[0].index;
        break;
    case false:
        uint32_t it {};
        if (bounding_it != -1) {
            it = bounding_it < cursor.pos[2].index ? 0 : 1;
            cursor.pos[it].index = bounding_it;
            cursor.pos[it].chunk_index = chunk_it;
            cursor.pos[it].text_index = text_it;
            cursor.pos[it].last_text_index = text_it;
        } else {
            it = total_it < cursor.pos[2].index ? 0 : 1;
            cursor.pos[it].index = total_it;
            cursor.pos[it].chunk_index = chunk_it - (!this->text_chunk_list.empty());
            cursor.pos[it].text_index = utf_char_index;
            cursor.pos[it].last_text_index = text_it;
        }

        if (it == 1) {
            cursor.pos[0] = cursor.pos[2];
            cursor.target = cursor.pos[1].index;
        } else {
            cursor.pos[1] = cursor.pos[2];
            cursor.target = cursor.pos[0].index;
        }

        break;
    }

    ekg::dispatch(ekg::env::redraw);
}

void ekg::ui::textbox_widget::on_destroy() {

}

void ekg::ui::textbox_widget::on_reload() {
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
        ekg::utf_decode(this->widget_side_text, this->text_chunk_list);

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

void ekg::ui::textbox_widget::on_pre_event(SDL_Event &sdl_event) {
    abstract_widget::on_pre_event(sdl_event);
    this->embedded_scroll.on_pre_event(sdl_event);
    this->flag.absolute = this->embedded_scroll.is_dragging_bar() || this->embedded_scroll.flag.activy || this->flag.state;
}

void ekg::ui::textbox_widget::on_event(SDL_Event &sdl_event) {
    bool pressed {ekg::input::pressed() && ekg::input::action("textbox-activy")};
    bool released {ekg::input::released()};
    bool motion {ekg::input::motion()};

    if (this->flag.hovered) {
        ekg::cursor = ekg::systemcursor::ibeam;
    }

    this->is_action_select_enable = ekg::input::action("textbox-action-select");
    if (this->flag.hovered && pressed) {
        ekg::set(this->flag.focused, this->flag.hovered);
        ekg::reset(ekg::core->ui_timing);

        auto &main_cursor {this->loaded_multi_cursor_list.at(0)};
        ekg::ui::textbox_widget::cursor clicked_pos {};
        this->check_cursor_text_bounding(clicked_pos, true);

        bool movement_input_enabled {this->flag.focused && this->is_action_select_enable};
        if (movement_input_enabled && !this->flag.state) {
            if (clicked_pos.pos[0].index > main_cursor.pos[2].index) {
                main_cursor.pos[2] = main_cursor.pos[0];
                main_cursor.pos[1] = clicked_pos.pos[0];
                main_cursor.target = clicked_pos.pos[0].index;
            } else {
                main_cursor.pos[2] = main_cursor.pos[1];
                main_cursor.pos[0] = clicked_pos.pos[0];
                main_cursor.target = clicked_pos.pos[1].index;
            }
        } else {
            main_cursor.pos[0] = clicked_pos.pos[0];
            main_cursor.pos[1] = clicked_pos.pos[1];
            main_cursor.pos[2] = clicked_pos.pos[2];
            main_cursor.target = clicked_pos.pos[0].index;
        }

        this->flag.state = this->flag.hovered;
    } else if (this->flag.state && motion && !this->embedded_scroll.is_dragging_bar()) {
        this->check_cursor_text_bounding(this->loaded_multi_cursor_list.at(0), false);
    }

    if (released) {
        this->flag.state = false;
    }

    this->embedded_scroll.on_event(sdl_event);
    this->flag.highlight = this->flag.hovered;

    if ((this->flag.focused || this->flag.hovered || this->flag.absolute) && !this->is_high_frequency) {
        ekg::update_high_frequency(this);
    }

    if (!this->flag.hovered && pressed && !ekg::input::typed()) {
        ekg::ui::textbox_widget::cursor main_cursor {this->loaded_multi_cursor_list.at(0)};
        main_cursor.pos[1] = main_cursor.pos[0];

        this->loaded_multi_cursor_list.clear();
        this->loaded_multi_cursor_list.push_back(main_cursor);  
    
        ekg::set(this->flag.focused, false);
    }

    this->is_clipboard_cut = ekg::input::action("clipboard-cut");
    this->is_clipboard_paste = ekg::input::action("clipboard-paste");
    this->is_clipboard_copy = ekg::input::action("clipboard-copy");

    bool should_process_textbox {
        this->flag.focused && (
        (sdl_event.type == SDL_KEYDOWN || sdl_event.type == SDL_TEXTINPUT) ||
        (this->is_clipboard_copy || this->is_clipboard_cut || this->is_clipboard_paste))
    };

    if (!should_process_textbox) {
        return;
    }

    if (sdl_event.type == SDL_TEXTINPUT) {
        for (ekg::ui::textbox_widget::cursor &cursor : this->loaded_multi_cursor_list) {
            this->process_text(cursor, sdl_event.text.text, ekg::ui::textbox_widget::action::add_text, 1);
        }
    } else if (sdl_event.type == SDL_KEYDOWN && sdl_event.key.keysym.sym == SDLK_ESCAPE) {
        ekg::ui::textbox_widget::cursor main_cursor {this->loaded_multi_cursor_list.at(0)};
        main_cursor.pos[1] = main_cursor.pos[0];

        this->loaded_multi_cursor_list.clear();
        this->loaded_multi_cursor_list.push_back(main_cursor);

        ekg::set(this->flag.focused, false);
    } else if (ekg::input::action("textbox-action-select-all")) {
        ekg::ui::textbox_widget::cursor main_cursor {this->loaded_multi_cursor_list.at(0)};
        main_cursor.pos[0].index = 0;
        main_cursor.pos[0].chunk_index = 0;
        main_cursor.pos[0].text_index = 0;
        main_cursor.pos[0].last_text_index = 0;

        main_cursor.pos[1].index = this->total_utf_chars;
        main_cursor.pos[1].chunk_index = ekg::min((int64_t) this->text_chunk_list.size() - 1, (int64_t) 0);
        main_cursor.pos[1].text_index = ekg::utf_length(this->get_cursor_emplace_text(main_cursor.pos[1]));
        main_cursor.pos[1].last_text_index = main_cursor.pos[1].chunk_index;

        this->loaded_multi_cursor_list.clear();
        this->loaded_multi_cursor_list.push_back(main_cursor);
    } else {
        int64_t cursor_dir[2] {};
        this->is_action_modifier_enable = ekg::input::action("textbox-action-modifier");

        for (ekg::ui::textbox_widget::cursor &cursor : this->loaded_multi_cursor_list) {
            cursor_dir[0] = cursor_dir[1] = 0;

            if (ekg::input::action("textbox-action-up")) {
                cursor_dir[1] = -1;
            } else if (ekg::input::action("textbox-action-down")) {
                cursor_dir[1] = 1;
            } else if (ekg::input::action("textbox-action-left")) {
                cursor_dir[0] = -1;
            } else if (ekg::input::action("textbox-action-right")) {
                cursor_dir[0] = 1;
            } else if (ekg::input::action("textbox-action-delete-left")) {
                this->process_text(cursor, "backspace", ekg::ui::textbox_widget::action::erase_text, -1);
            } else if (ekg::input::action("textbox-action-delete-right")) {
                this->process_text(cursor, "delete", ekg::ui::textbox_widget::action::erase_text, 1);
            } else if (ekg::input::action("textbox-action-break-line")) {
                this->process_text(cursor, "return", ekg::ui::textbox_widget::action::break_line, 1);
            } else if (ekg::input::action("textbox-action-tab")) {
                this->process_text(cursor, "\t", ekg::ui::textbox_widget::action::add_text, 1);
            } else if (this->is_clipboard_copy || this->is_clipboard_paste || this->is_clipboard_cut) {
                this->process_text(cursor, "clipboard", ekg::ui::textbox_widget::action::add_text, 0);
            }

            if (cursor_dir[0] != 0 || cursor_dir[1] != 0) {
                this->check_nearest_word(cursor, cursor_dir[0], cursor_dir[1]);
                this->move_target_cursor(cursor, cursor_dir[0], cursor_dir[1]);
            }
        }   
    }
}

void ekg::ui::textbox_widget::on_post_event(SDL_Event &sdl_event) {
    abstract_widget::on_post_event(sdl_event);
    this->embedded_scroll.flag.hovered = false;
    this->embedded_scroll.flag.activy = false;
}

void ekg::ui::textbox_widget::on_update() {
    if (ekg::reach(ekg::core->ui_timing, 500) && !this->redraw_cursor) {
        this->redraw_cursor = true;
    } else if (!ekg::reach(ekg::core->ui_timing, 500) && this->redraw_cursor) {
        this->redraw_cursor = false;
    }

    this->embedded_scroll.on_update();
    this->is_high_frequency = this->embedded_scroll.check_activy_state(this->flag.focused || this->flag.hovered);
}

int32_t ekg::ui::textbox_widget::find_cursor(ekg::ui::textbox_widget::cursor &target_cursor,
                                             int64_t total_it, int64_t it_chunk, bool last_line_utf_char_index) {
    bool a_cursor_pos {};
    bool b_cursor_pos {};

    for (ekg::ui::textbox_widget::cursor &cursor : this->loaded_multi_cursor_list) {
        a_cursor_pos = ((last_line_utf_char_index && total_it + 1 >= cursor.pos[0].index) || total_it >= cursor.pos[0].index);
        b_cursor_pos = ((last_line_utf_char_index && total_it + 1 <= cursor.pos[1].index) || total_it <= cursor.pos[1].index);
        if ((a_cursor_pos && b_cursor_pos && it_chunk >= cursor.pos[0].chunk_index && it_chunk <= cursor.pos[1].chunk_index) || (it_chunk > cursor.pos[0].chunk_index && it_chunk < cursor.pos[1].chunk_index)) {
            target_cursor = cursor;
            return 0;
        }
    }

    return -1;
}

void ekg::ui::textbox_widget::on_draw_refresh() {
    auto ui {(ekg::ui::textbox*) this->data};
    auto &rect {this->get_abs_rect()};
    auto &f_renderer {ekg::f_renderer(ui->get_font_size())};
    auto &theme {ekg::theme()};
    auto &allocator {ekg::core->gpu_allocator};

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

    bool is_utf_char_last_index {};
    bool render_cursor {};

    int64_t text_size {};
    uint64_t total_it {};

    char32_t ui32_char {};
    uint8_t ui8_char {};
    std::string utf_string {};
    std::string text {};

    uint64_t chunk_size {this->text_chunk_list.size()};
    uint64_t utf_char_index {};
    uint64_t it {};
    uint64_t text_chunk_size {this->text_chunk_list.size()};

    this->rect_cursor.w = 2.0f;
    this->rect_cursor.h = this->text_height;
    this->is_ui_enabled = ui->is_enabled();

    std::vector<ekg::rect> cursor_draw_data_list {};
    ekg::ui::textbox_widget::cursor cursor {};

    int32_t cursor_pos_index {};

    bool draw_cursor {this->flag.focused && !ekg::reach(ekg::core->ui_timing, 500)};
    bool optimize_batching {};
    bool do_not_fill_line {};

    /*
     * 0 == previous char wsize
     * 1 == current char wisze
     * 2 == next char wsize
     */
    this->cursor_char_wsize[0] = 0.0f;
    this->cursor_char_wsize[1] = 0.0f;
    this->cursor_char_wsize[2] = 0.0f;

    // @TODO change cursor batching collector to invoke firing

    /*
     * The texti iterator jump utf 8 - 16 sequences.
     * For better performance, textbox does not render texts out of rect.
     */
    for (int64_t it_chunk {}; it_chunk < text_chunk_size; it_chunk++) {
        text = this->text_chunk_list.at(it_chunk);
        x = this->rect_cursor.w;
        text_size = 0;
        f_renderer.ft_uint_previous = 0;
        utf_char_index = 0;
        do_not_fill_line = false;

        if (text.empty() && (cursor_pos_index = this->find_cursor(cursor, total_it, it_chunk, false)) != -1 && 
                            ((draw_cursor && (cursor.pos[0] == cursor.pos[1] || cursor.pos[1].index > total_it)) ||
                            (cursor.pos[0] != cursor.pos[1] && cursor.pos[1].index > total_it))) {
            optimize_batching = true;
            do_not_fill_line = true;

            cursor_draw_data_list.emplace_back(ekg::rect {
                rect.x + x + this->embedded_scroll.scroll.x,
                rect.y + y + this->embedded_scroll.scroll.y,
                this->rect_cursor.w + ((this->rect_cursor.w) * (cursor.pos[0] != cursor.pos[1])),
                text_height
            });
        } else {
            text_size = ekg::utf_length(text);
        }

        data.factor += static_cast<int32_t>(y) + 32;
        y_scroll = this->embedded_scroll.scroll.y + y;
        if (y_scroll > rect.h) {
            data.factor += static_cast<int32_t>(text_chunk_size) * 32;
            break;
        }

        y_scroll += this->text_height;
        for (it = 0; it < text.size() && (y_scroll > 0.0f || !optimize_batching); it++) {
            ui8_char = static_cast<uint8_t>(text.at(it));
            it += ekg::utf_check_sequence(ui8_char, ui32_char, utf_string, text, it);

            if (f_renderer.ft_bool_kerning && f_renderer.ft_uint_previous) {
                FT_Get_Kerning(f_renderer.ft_face, f_renderer.ft_uint_previous, ui32_char, 0, &f_renderer.ft_vector_previous_char);
                x += static_cast<float>(f_renderer.ft_vector_previous_char.x >> 6);
            }

            char_data = f_renderer.allocated_char_data[ui32_char];
            is_utf_char_last_index = utf_char_index + 1 == text_size;
            if ((cursor_pos_index = this->find_cursor(cursor, total_it, it_chunk, is_utf_char_last_index)) != -1 && ((draw_cursor && (cursor.pos[0] == cursor.pos[1] || cursor.pos[1].index > total_it)) || 
                                                                                                                  (cursor.pos[0] != cursor.pos[1] && cursor.pos[1].index > total_it))) {
                optimize_batching = true;

                if (cursor.pos[0] != cursor.pos[1] && (it_chunk == cursor.pos[0].chunk_index || it_chunk == cursor.pos[1].chunk_index)) {
                    cursor_draw_data_list.emplace_back(ekg::rect {
                        rect.x + x + this->embedded_scroll.scroll.x,
                        rect.y + y + this->embedded_scroll.scroll.y,
                        char_data.wsize + (is_utf_char_last_index * this->rect_cursor.w),
                        text_height
                    });
                } else if (cursor.pos[0] == cursor.pos[1]) {
                    is_utf_char_last_index = is_utf_char_last_index && cursor.pos[cursor_pos_index].index == total_it + 1;
                    cursor_draw_data_list.emplace_back(ekg::rect {
                        rect.x + x + (char_data.wsize * is_utf_char_last_index) + this->embedded_scroll.scroll.x,
                        rect.y + y + this->embedded_scroll.scroll.y,
                        this->rect_cursor.w,
                        text_height
                    });
                }
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
                data.factor += static_cast<int32_t>(x + ui32_char);
            }

            f_renderer.ft_uint_previous = ui32_char;
            total_it++;
            utf_char_index++;
            x += char_data.wsize;

            if (x + this->embedded_scroll.scroll.x > rect.w && render_cursor) {
                total_it += (text_size - utf_char_index);
                break;
            }
        }

        if (!do_not_fill_line && it_chunk > cursor.pos[0].chunk_index && it_chunk < cursor.pos[1].chunk_index) {
            cursor_draw_data_list.emplace_back(ekg::rect {
                rect.x + this->rect_cursor.w + this->embedded_scroll.scroll.x,
                rect.y + y + this->embedded_scroll.scroll.y,
                x,
                text_height
            });
        }

        if (y_scroll < 0.0f && optimize_batching) {
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

    this->rect_text.h = (this->text_height * text_chunk_size) + (this->text_offset * 2.0f);
    this->embedded_scroll.rect_child = this->rect_text;
    this->embedded_scroll.on_draw_refresh();

    ekg::draw::rect(rect, theme.textbox_outline, ekg::drawmode::outline);
    ekg::draw::bind_off_scissor();
}