/*
* MIT License
* 
* Copyright (c) 2022-2023 Rina Wilk / vokegpu@gmail.com
* 
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
* 
* The above copyright notice and this permission notice shall be included in all
* copies or substantial portions of the Software.
* 
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
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

    ekg::ui::textbox_widget::cursor_pos &target_cursor_pos {cursor.pos[3] == cursor.pos[0] ? cursor.pos[0] : cursor.pos[1]};
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

    std::string utf_string {};
    char32_t char32 {};
    uint8_t char8 {};

    x = 0;

    /*
     * the cursor index text position is utf encoded,
     * not string coded, so it should start by begin
     * and not by cursor. 
     */

    while (it < emplace_text_size) {
        char8 = static_cast<uint8_t>(emplace_text.at(it));

        if (is_dir_right && utf_it >= target_cursor_pos.text_index) {
            if (char8 != 32) {
                should_update_last_char = true;
            } else if (should_update_last_char) {
                x = utf_it - target_cursor_pos.text_index;
                break;
            }
        } else if (!is_dir_right) {
            if (utf_it == target_cursor_pos.text_index) {
                break;
            }

            if (char8 == 32) {
                should_update_last_char = true;
            } else if (should_update_last_char) {
                x = utf_it;
                should_update_last_char = false;
            }
        }

        utf_it++;
        it += ekg::utf_check_sequence(char8, char32, utf_string, emplace_text, it) + 1;
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
    if ((cursor.pos[0] == cursor.pos[3] && this->is_action_select_enable) ||
       ((x < 0 || y < 0) && cursor.pos[0] != cursor.pos[1] && !this->is_action_select_enable)) {
        target_cursor_pos = cursor.pos[0];
    } else {
        target_cursor_pos = cursor.pos[1];
    }

    if (cursor.pos[0] == cursor.pos[1] || this->is_action_select_enable) {
        this->move_cursor(target_cursor_pos, x, y);
    }

    if (!this->is_action_select_enable) {
        cursor.pos[0] = target_cursor_pos;
        cursor.pos[1] = target_cursor_pos;
        cursor.pos[2] = target_cursor_pos;
        cursor.pos[3] = target_cursor_pos;
        return;
    }

    cursor.pos[3] = target_cursor_pos;

    if (cursor.pos[3] >= cursor.pos[2]) {
        cursor.pos[0] = cursor.pos[2];
        cursor.pos[1] = target_cursor_pos;
    } else {
        cursor.pos[0] = target_cursor_pos;
        cursor.pos[1] = cursor.pos[2];
    }
}

/*
 * This method is not called all the time, the part of batching rects for rendering after,
 * works okay, but I will write a fast select rect batching.
 */
void ekg::ui::textbox_widget::update_cpu_side_batching_cursor() {
    auto p_ui {(ekg::ui::textbox*) this->data};
    auto &f_renderer {ekg::f_renderer(p_ui->get_font_size())};
    auto &rect {this->get_abs_rect()};

    this->rect_text.w = 0.0f;
    this->total_utf_chars = 0;
    std::string formated_text {};

    this->visible_text[0] = 0;
    this->visible_text[1] = 0;

    float x {};
    float y {this->text_offset};

    bool is_utf_char_last_index {};
    bool render_cursor {};

    int64_t text_size {};
    uint64_t text_utf_char_index {};

    char32_t char32 {};
    std::string utf_string {};
    uint8_t char8 {};

    uint64_t chunk_size {this->text_chunk_list.size()};
    uint64_t utf_char_index {};
    uint64_t it {};
    uint64_t text_chunk_size {this->text_chunk_list.size()};

    this->rect_cursor.w = 2.0f;
    this->rect_cursor.h = this->text_height;
    this->is_ui_enabled = p_ui->is_enabled();

    ekg::ui::textbox_widget::cursor cursor {};
    int32_t cursor_pos_index {};

    bool optimize_batching {};
    bool do_not_fill_line {};
    bool draw_additional_selected_last_char {};

    this->cursor_draw_data_list.clear();

    if (this->text_utf_char_index_list.size() != text_chunk_size) {
        this->text_utf_char_index_list.resize(text_chunk_size);
    }

    for (int64_t it_chunk {}; it_chunk < text_chunk_size; it_chunk++) {
        std::string &text {this->text_chunk_list.at(it_chunk)};

        x = this->rect_cursor.w;
        f_renderer.ft_uint_previous = 0;
        utf_char_index = 0;
        do_not_fill_line = false;

        if ((text.empty() && (cursor_pos_index = this->find_cursor(cursor, text_utf_char_index, it_chunk, false)) != -1 && 
             ((cursor.pos[0] == cursor.pos[1] || cursor.pos[1].index > text_utf_char_index) ||
             (cursor.pos[0] != cursor.pos[1] && cursor.pos[1].index > text_utf_char_index)))) {
            do_not_fill_line = true;

            this->cursor_draw_data_list.emplace_back(ekg::rect {
                x,
                y,
                this->rect_cursor.w + ((this->rect_cursor.w) * (cursor.pos[0] != cursor.pos[1])),
                text_height
            });
        }

        text_size = ekg::utf_length(text);
        for (it = 0; it < text.size(); it++) {
            char8 = static_cast<uint8_t>(text.at(it));
            it += ekg::utf_check_sequence(char8, char32, utf_string, text, it);

            if (f_renderer.ft_bool_kerning && f_renderer.ft_uint_previous) {
                FT_Get_Kerning(f_renderer.ft_face, f_renderer.ft_uint_previous, char32, 0, &f_renderer.ft_vector_previous_char);
                x += static_cast<float>(f_renderer.ft_vector_previous_char.x >> 6);
            }

            ekg::char_data &char_data {f_renderer.allocated_char_data[char32]};
            is_utf_char_last_index = utf_char_index + 1 == text_size;

            if ((cursor_pos_index = this->find_cursor(cursor, text_utf_char_index, it_chunk, is_utf_char_last_index)) != -1 &&
                ((cursor.pos[0] == cursor.pos[1] || cursor.pos[0].index > text_utf_char_index) || (cursor.pos[0] != cursor.pos[1] && cursor.pos[1].index > text_utf_char_index))) {

                draw_additional_selected_last_char = {
                    (is_utf_char_last_index &&
                    cursor.pos[0] != cursor.pos[1] &&
                    cursor.pos[1].index > text_utf_char_index &&
                    cursor.pos[0].chunk_index != cursor.pos[1].chunk_index)
                };

                if (cursor.pos[0] != cursor.pos[1] && (it_chunk == cursor.pos[0].chunk_index || it_chunk == cursor.pos[1].chunk_index)) {
                    this->cursor_draw_data_list.emplace_back(ekg::rect {
                        x,
                        y,
                        char_data.wsize + (is_utf_char_last_index * this->rect_cursor.w) + (draw_additional_selected_last_char * (this->rect_cursor.w + this->rect_cursor.w)),
                        text_height
                    });
                } else if (cursor.pos[0] == cursor.pos[1]) {
                    is_utf_char_last_index = is_utf_char_last_index && cursor.pos[cursor_pos_index].index == text_utf_char_index + 1;
                    this->cursor_draw_data_list.emplace_back(ekg::rect {
                        x + (char_data.wsize * is_utf_char_last_index),
                        y,
                        this->rect_cursor.w,
                        text_height
                    });
                }
            }

            f_renderer.ft_uint_previous = char32;
            text_utf_char_index++;
            utf_char_index++;
            x += char_data.wsize;
        }

        formated_text += text;
        formated_text += it_chunk + 1 == text_chunk_size ? 0 : '\n';

        this->rect_text.w = ekg::min(this->rect_text.w, x);
        if (!do_not_fill_line && it_chunk > cursor.pos[0].chunk_index && it_chunk < cursor.pos[1].chunk_index) {
            this->cursor_draw_data_list.emplace_back(ekg::rect {
                this->rect_cursor.w,
                y,
                x + this->rect_cursor.w + this->rect_cursor.w,
                text_height
            });
        }

        y += this->text_height;
        this->text_utf_char_index_list.at(it_chunk) = text_utf_char_index;
    }

    p_ui->unsafe_set_text(formated_text);
    this->widget_side_text = formated_text;

    this->total_utf_chars = text_utf_char_index;
    this->rect_text.w += this->text_offset * 2.0f;
}

void ekg::ui::textbox_widget::move_cursor(ekg::ui::textbox_widget::cursor_pos &cursor, int64_t x, int64_t y) {
    int64_t base_it {ekg::min(cursor.index - cursor.text_index, (int64_t) 0)};
    std::string &emplace_text {this->get_cursor_emplace_text(cursor)};
    int64_t emplace_text_size {(int64_t) ekg::utf_length(emplace_text)};

    bool text_it_chunk_bounding_size {cursor.chunk_index + 1 == this->text_chunk_list.size()};
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
        if (text_it_chunk_bounding_size) {
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
    bool out_of_index {cursor.index < base_it};

    if (out_of_index && cursor.chunk_index > 0 && check_cursor_x) {
        cursor.chunk_index--;
        y = -1;

        cursor.index = base_it;
        cursor.text_index = ekg::utf_length(this->get_cursor_emplace_text(cursor));
    }

    int64_t base_it_plus_emplace_text_size {base_it + emplace_text_size};
    out_of_index = cursor.index > base_it_plus_emplace_text_size;

    if (out_of_index && text_it_chunk_bounding_size && check_cursor_x) {
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
    auto p_ui {(ekg::ui::textbox*) this->data};
    auto &f_renderer {ekg::f_renderer(p_ui->get_font_size())};
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
    auto p_ui {(ekg::ui::textbox*) this->data};
    if (!(this->is_ui_enabled = p_ui->is_enabled()) && !(text == "clipboard" && this->is_clipboard_copy)) {
        return;
    }

    std::string &emplace_text_a {this->get_cursor_emplace_text(cursor.pos[0])};
    std::string &emplace_text_b {this->get_cursor_emplace_text(cursor.pos[1])};

    bool max_size_reached[2] {
        emplace_text_a.size() == p_ui->get_max_chars_per_line(),
        emplace_text_b.size() == p_ui->get_max_chars_per_line()
    };

    switch (action) {
    case ekg::ui::textbox_widget::action::add_text:
        if ((this->is_action_modifier_enable && !(this->is_clipboard_cut || this->is_clipboard_copy || this->is_clipboard_paste))) {
            break;
        }

        /*
         * Always the tab is pressed, the process should add the spaces instead of \t,
         * it is cached to prevent useless iteration.
         */
        if (text == "\t") {
            uint8_t ui_tab_size {p_ui->get_tab_size()};

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

                        bool not_last_line {};
                        bool last_line {};

                        while (it < cursor.pos[1].chunk_index && it < size) {
                            copy_text += this->text_chunk_list.at(it);
                            
                            last_line = it + 1 == size;
                            not_last_line = !last_line;

                            if (not_last_line || (this->text_chunk_list.at(it).empty() && last_line)) {
                                copy_text += '\n';
                            }

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

        if (max_size_reached[0]) {
            break;
        }

        emplace_text_a = ekg::utf_substr(emplace_text_a, 0, cursor.pos[0].text_index) + text.data() +
                                     ekg::utf_substr(emplace_text_b, cursor.pos[1].text_index, ekg::utf_length(emplace_text_b));

        if (cursor.pos[0].chunk_index != cursor.pos[1].chunk_index) {
            this->text_chunk_list.erase(this->text_chunk_list.begin() + cursor.pos[0].chunk_index + 1,
                                                         this->text_chunk_list.begin() + cursor.pos[1].chunk_index + 1);
        }

        if (this->is_clipboard_paste && SDL_HasClipboardText() && !(text = SDL_GetClipboardText()).empty()) {
            direction = static_cast<int64_t>(ekg::utf_length(text));

            std::vector<std::string> utf_clipboard_decoded {};
            ekg::utf_decode(text, utf_clipboard_decoded);

            uint64_t sum_decoded_size {this->text_chunk_list.size() + utf_clipboard_decoded.size() - 1};
            if (sum_decoded_size > p_ui->get_max_lines()) {
                uint64_t subtract_decoded_size {sum_decoded_size - p_ui->get_max_lines()};
                utf_clipboard_decoded.erase(utf_clipboard_decoded.begin() + subtract_decoded_size, utf_clipboard_decoded.end());
            }

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
                this->text_chunk_list.insert(this->text_chunk_list.begin() + cursor.pos[0].chunk_index + 1,
                                                             utf_clipboard_decoded.begin() + 1, 
                                                             utf_clipboard_decoded.end());

                cursor.pos[0] = cursor.pos[1];
                direction = 0;
            }
        }

        ekg_textbox_clamp_text_chunk_size(this->text_chunk_list, p_ui->get_max_lines());
        ekg_textbox_clamp_line(emplace_text_a, p_ui->get_max_chars_per_line());
        ekg_textbox_clamp_line(emplace_text_b, p_ui->get_max_chars_per_line());

        this->move_cursor(cursor.pos[0], direction, 0);
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

            this->move_cursor(direction < 0 ? cursor.pos[0] : cursor.pos[1], cursor_dir[0], cursor_dir[1]);
        }

        if (cursor.pos[0] == cursor.pos[1] && direction < 0 && (cursor.pos[0].index > 0 || cursor.pos[0].chunk_index > 0)) {
            if (cursor.pos[0].text_index - 1 < 0 && cursor.pos[0].chunk_index > 0) {
                std::string stored_text {emplace_text_a};

                this->move_cursor(cursor.pos[0], -1, 0);
                this->text_chunk_list.erase(this->text_chunk_list.begin() + cursor.pos[0].chunk_index + 1);

                std::string &upper_line_text {this->get_cursor_emplace_text(cursor.pos[0])};
                upper_line_text += stored_text;

                ekg_textbox_clamp_line(upper_line_text, p_ui->get_max_chars_per_line());
            } else {
                int64_t it {ekg::min(cursor.pos[0].text_index - 1, (int64_t) 0)};
                emplace_text_a = ekg::utf_substr(emplace_text_a, 0, it) + ekg::utf_substr(emplace_text_a, it + 1, ekg::utf_length(emplace_text_a));
                ekg_textbox_clamp_line(emplace_text_a, p_ui->get_max_chars_per_line());
                this->move_cursor(cursor.pos[0], -1, 0);
            }
        } else if (cursor.pos[0] != cursor.pos[1] && direction != 0) {
            emplace_text_a = ekg::utf_substr(emplace_text_a, 0, cursor.pos[0].text_index) +
                                         ekg::utf_substr(emplace_text_b, cursor.pos[1].text_index, ekg::utf_length(emplace_text_b));

            if (cursor.pos[0].chunk_index != cursor.pos[1].chunk_index) {
                this->text_chunk_list.erase(this->text_chunk_list.begin() + cursor.pos[0].chunk_index + 1,
                                                             this->text_chunk_list.begin() + cursor.pos[1].chunk_index + 1);
            }

            ekg_textbox_clamp_line(emplace_text_a, p_ui->get_max_chars_per_line());
        } else if (cursor.pos[0] == cursor.pos[1] && direction > 0) {
            int64_t emplace_text_size {(int64_t) ekg::utf_length(emplace_text_a)};
            bool text_it_chunk_bounding_size {cursor.pos[0].chunk_index + 1 == this->text_chunk_list.size()};

            if (cursor.pos[0].text_index >= emplace_text_size && !text_it_chunk_bounding_size) {
                emplace_text_a += this->text_chunk_list.at(cursor.pos[0].chunk_index + 1);
                this->text_chunk_list.erase(this->text_chunk_list.begin() + cursor.pos[0].chunk_index + 1);
            } else if (cursor.pos[0].text_index < emplace_text_size) {
                int64_t it {cursor.pos[0].text_index};
                emplace_text_a = ekg::utf_substr(emplace_text_a, 0, it) + ekg::utf_substr(emplace_text_a, it + 1, ekg::utf_length(emplace_text_a));
            }

            ekg_textbox_clamp_line(emplace_text_a, p_ui->get_max_chars_per_line());
        }

        break;

    case ekg::ui::textbox_widget::action::break_line:
        if (this->text_chunk_list.size() >= p_ui->get_max_lines()) {
            return;
        }

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
        ekg_textbox_clamp_text_chunk_size(this->text_chunk_list, p_ui->get_max_lines());
        this->move_cursor(cursor.pos[0], cursor_dir[0], cursor_dir[1]);

        ekg::reset(ekg::core->ui_timing);
        ekg::dispatch(ekg::env::redraw);

        break;
    }

    if (!this->is_clipboard_copy) {
        cursor.pos[2] = cursor.pos[1] = cursor.pos[0];
        cursor.pos[3] = cursor.pos[0];
        this->redraw_cursor = true;
    }

    ekg::reset(ekg::core->ui_timing);
    ekg::dispatch(ekg::env::redraw);
}

std::string &ekg::ui::textbox_widget::get_cursor_emplace_text(ekg::ui::textbox_widget::cursor_pos &cursor) {
    if (cursor.chunk_index >= this->text_chunk_list.size()) {
        return this->text_chunk_list.at(this->text_chunk_list.size() - 1);
    }

    return this->text_chunk_list.at(cursor.chunk_index);
}

void ekg::ui::textbox_widget::check_cursor_text_bounding(ekg::ui::textbox_widget::cursor &cursor, bool reset_second_cursor_pos) {
    if (!this->flag.focused) {
        return;
    }

    auto p_ui {(ekg::ui::textbox*) this->data};
    auto &rect {this->get_abs_rect()};
    auto &f_renderer {ekg::f_renderer(p_ui->get_font_size())};

    float x {};
    float y {rect.y + this->embedded_scroll.scroll.y};

    ekg::rect char_rect {};
    ekg::vec4 interact {ekg::input::interact()};

    interact.x = ekg::clamp(interact.x, rect.x + this->rect_cursor.w, rect.x + rect.w - this->rect_cursor.w);
    interact.y = ekg::clamp(interact.y, rect.y + this->rect_cursor.w, rect.y + rect.h - this->rect_cursor.w);

    int64_t it_bounding {-1};
    int64_t chunk_size {static_cast<int64_t>(this->text_chunk_list.size())};

    char32_t char32 {};
    uint8_t char8 {};

    size_t utf_char_index {};
    std::string utf_string {};
    bool is_it_chunk_at_end {};

    uint64_t text_utf_char_index {this->text_utf_char_index_list.at(this->visible_text[1]) - this->text_chunk_list.at(this->visible_text[1]).size()};
    uint64_t text_it {};
    uint64_t it_chunk {};
    uint64_t it {};

    y += (this->text_height * this->visible_text[1]);
    for (it_chunk = this->visible_text[1]; it_chunk < this->text_chunk_list.size(); it_chunk++) {
        std::string &text {this->text_chunk_list.at(it_chunk)};

        x = rect.x + this->rect_cursor.w + this->embedded_scroll.scroll.x;
        utf_char_index = 0;
        f_renderer.ft_uint_previous = 0;
        is_it_chunk_at_end = it_chunk == chunk_size - 1;

        for (it = 0; it < text.size(); it++) {
            char8 = static_cast<uint8_t>(text.at(it));
            it += ekg::utf_check_sequence(char8, char32, utf_string, text, it);

            if (f_renderer.ft_bool_kerning && f_renderer.ft_uint_previous) {
                FT_Get_Kerning(f_renderer.ft_face, f_renderer.ft_uint_previous, char32, 0, &f_renderer.ft_vector_previous_char);
                x += static_cast<float>(f_renderer.ft_vector_previous_char.x >> 6);
            }

            /*
              * Check if interact position collide with char.
              */

            char_rect.x = x;
            char_rect.y = y;
            char_rect.w = (f_renderer.allocated_char_data[char32].wsize) / 2;
            char_rect.h = is_it_chunk_at_end ? rect.h : this->text_height + 1.0f;

            if (ekg::rect_collide_vec_precisely(char_rect, interact)) {
                it_bounding = text_utf_char_index;
                text_it = utf_char_index;
                break;
            }

            char_rect.w += char_rect.w;
            if (ekg::rect_collide_vec_precisely(char_rect, interact)) {
                it_bounding = text_utf_char_index + 1;
                text_it = utf_char_index + 1;
                break;
            }

            text_utf_char_index++;
            utf_char_index++;
            f_renderer.ft_uint_previous = char32;
            x += char_rect.w;
        }

        /*
          * If interact position is not colliding with any char in this line, then set index to the end or begin of line.
          */

        char_rect.x = rect.x;
        char_rect.y = y;
        char_rect.w = rect.w;
        char_rect.h = this->text_height + 1.0f;

        if (ekg::rect_collide_vec(char_rect, interact) && it_bounding == -1) {
            char_rect.w = this->text_offset;
            if (ekg::rect_collide_vec(char_rect, interact)) {
                it_bounding = text_utf_char_index - utf_char_index;
            } else {
                it_bounding = text_utf_char_index;
                text_it = utf_char_index;
            }

            break;
        }

        if (it_bounding != -1) break;
        y += this->text_height;
        it_chunk++;
    }

    switch (reset_second_cursor_pos) {
    case true:
        if (it_bounding != -1) {
            cursor.pos[0].index = it_bounding;
            cursor.pos[0].chunk_index = it_chunk;
            cursor.pos[0].text_index = text_it;
            cursor.pos[0].last_text_index = text_it;
        } else {
            cursor.pos[0].index = text_utf_char_index;
            cursor.pos[0].chunk_index = it_chunk - (!this->text_chunk_list.empty());
            cursor.pos[0].text_index = utf_char_index;
            cursor.pos[0].last_text_index = text_it;
        }

        cursor.pos[1] = cursor.pos[0];
        cursor.pos[2] = cursor.pos[0];
        cursor.pos[3] = cursor.pos[0];
        break;
    case false:
        uint32_t it {};
        if (it_bounding != -1) {
            it = it_bounding < cursor.pos[2].index ? 0 : 1;
            cursor.pos[it].index = it_bounding;
            cursor.pos[it].chunk_index = it_chunk;
            cursor.pos[it].text_index = text_it;
            cursor.pos[it].last_text_index = text_it;
        } else {
            it = text_utf_char_index < cursor.pos[2].index ? 0 : 1;
            cursor.pos[it].index = text_utf_char_index;
            cursor.pos[it].chunk_index = it_chunk - (!this->text_chunk_list.empty());
            cursor.pos[it].text_index = utf_char_index;
            cursor.pos[it].last_text_index = text_it;
        }

        if (it == 1) {
            cursor.pos[0] = cursor.pos[2];
            cursor.pos[3] = cursor.pos[1];
        } else {
            cursor.pos[1] = cursor.pos[2];
            cursor.pos[3] = cursor.pos[0];
        }

        break;
    }

    ekg::dispatch(ekg::env::redraw);
}

void ekg::ui::textbox_widget::on_destroy() {

}

void ekg::ui::textbox_widget::on_reload() {
    auto p_ui {(ekg::ui::textbox*) this->data};
    auto &rect {this->get_abs_rect()};
    auto &f_renderer {ekg::f_renderer(p_ui->get_font_size())};
    auto scaled_height {p_ui->get_scaled_height()};

    float text_width {f_renderer.get_text_width(p_ui->get_tag())};
    float dimension_offset {this->text_height / 2};

    this->text_height = f_renderer.get_text_height();
    this->text_offset = ekg::find_min_offset(text_width, dimension_offset);

    this->dimension.w = ekg::min(this->dimension.w, text_width);
    this->dimension.h = (this->text_height + dimension_offset) * static_cast<float>(scaled_height);

    this->min_size.x = ekg::min(this->min_size.x, text_height);
    this->min_size.y = ekg::min(this->min_size.y, this->dimension.h);

    if (this->widget_side_text != p_ui->get_text() || this->widget_side_text.empty()) {
        int64_t previous_size {static_cast<int64_t>(this->text_chunk_list.size())};

        this->text_chunk_list.clear();
        this->widget_side_text = p_ui->get_text();
        ekg::utf_decode(this->widget_side_text, this->text_chunk_list);

        /*
         * Generate default empty text for say to the rest of textbox,
         * that is empty and not with null lines.
         */
        if (this->text_chunk_list.empty()) {
            this->text_chunk_list.emplace_back();
            this->text_utf_char_index_list.emplace_back();
        }

        this->redraw_cursor = true;
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
                main_cursor.pos[3] = clicked_pos.pos[0];
            } else {
                main_cursor.pos[2] = main_cursor.pos[1];
                main_cursor.pos[0] = clicked_pos.pos[0];
                main_cursor.pos[3] = clicked_pos.pos[1];
            }
        } else {
            main_cursor.pos[0] = clicked_pos.pos[0];
            main_cursor.pos[1] = clicked_pos.pos[1];
            main_cursor.pos[2] = clicked_pos.pos[2];
            main_cursor.pos[3] = clicked_pos.pos[0];
        }

        this->flag.state = this->flag.hovered;
        this->redraw_cursor = true;
    } else if (this->flag.state && motion && !this->embedded_scroll.is_dragging_bar()) {
        this->check_cursor_text_bounding(this->loaded_multi_cursor_list.at(0), false);
        this->redraw_cursor = true;
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
        this->redraw_cursor = true;
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
        this->redraw_cursor = true;
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
                this->redraw_cursor = true;
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
    this->embedded_scroll.on_update();
    this->is_high_frequency = this->embedded_scroll.check_activy_state(this->flag.focused || this->flag.hovered);
}

/*
 * The find cursor method perform bloat,
 * but how the entire selection system is CPU-batched (it means not called all the render time),
 * the performance is not necessary bad.
 */
int32_t ekg::ui::textbox_widget::find_cursor(ekg::ui::textbox_widget::cursor &target_cursor,
                                                                        int64_t text_utf_char_index, int64_t it_chunk, bool last_line_utf_char_index) {
    bool a_cursor_pos {};
    bool b_cursor_pos {};

    for (ekg::ui::textbox_widget::cursor &cursor : this->loaded_multi_cursor_list) {
        a_cursor_pos = ((last_line_utf_char_index && text_utf_char_index + 1 >= cursor.pos[0].index) || text_utf_char_index >= cursor.pos[0].index);
        b_cursor_pos = ((last_line_utf_char_index && text_utf_char_index + 1 <= cursor.pos[1].index) || text_utf_char_index <= cursor.pos[1].index);

        if ((a_cursor_pos && b_cursor_pos && it_chunk >= cursor.pos[0].chunk_index && it_chunk <= cursor.pos[1].chunk_index) ||
            (it_chunk > cursor.pos[0].chunk_index && it_chunk < cursor.pos[1].chunk_index)) {
            target_cursor = cursor;
            return b_cursor_pos;
        }
    }

    return -1;
}

void ekg::ui::textbox_widget::on_draw_refresh() {
    auto p_ui {(ekg::ui::textbox*) this->data};
    auto &rect {this->get_abs_rect()};
    auto &f_renderer {ekg::f_renderer(p_ui->get_font_size())};
    auto &theme {ekg::theme()};
    auto &allocator {ekg::core->gpu_allocator};

    ekg::draw::bind_scissor(this->data->get_id());
    ekg::draw::sync_scissor(rect, p_ui->get_parent_id());
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

    this->rect_text.x = x;
    this->rect_text.y = y;
    this->is_ui_enabled = p_ui->is_enabled();

    x = 0.0f;
    y = this->text_offset;

    char32_t char32 {};
    std::string utf_string {};
    uint8_t char8 {};

    uint64_t it {};
    uint64_t text_chunk_size {this->text_chunk_list.size()};

    /*
     * 0 == previous char wsize
     * 1 == current char wisze
     * 2 == next char wsize
     */
    this->cursor_char_wsize[0] = 0.0f;
    this->cursor_char_wsize[1] = 0.0f;
    this->cursor_char_wsize[2] = 0.0f;

    if (this->redraw_cursor) {
        this->update_cpu_side_batching_cursor();
        this->redraw_cursor = false;
    }

    this->rect_text.h = (this->text_height * text_chunk_size) + (this->text_offset * 2.0f);

    /*
     * This line of code check the renderable text chunk index value,
     * dynamically calculating the amount of scroll with the size of
     * rect text height. 
     */
    this->visible_text[1] = static_cast<int32_t>(this->embedded_scroll.scroll.y) >= -0 ? 0 :
                                       static_cast<uint64_t>((roundf(-this->embedded_scroll.scroll.y) / this->rect_text.h) * static_cast<float>(text_chunk_size));

    // Multiply with the current visible index for get the perfect y position.
    y += (this->text_height * this->visible_text[1]);

    /*
     * The text iterator jump utf 8 - 16 sequences.
     * For better performance, textbox does not render texts out of rect.
     */
    for (uint64_t it_chunk {this->visible_text[1]}; it_chunk < text_chunk_size; it_chunk++) {
        std::string &text {this->text_chunk_list.at(it_chunk)};

        x = this->rect_cursor.w;
        f_renderer.ft_uint_previous = 0;

        data.factor += static_cast<int32_t>(y) + 32;
        y_scroll = this->embedded_scroll.scroll.y + y;

        if (y_scroll > rect.h) {
            data.factor += static_cast<int32_t>(text_chunk_size) * 32;
            break;
        }

        for (it = 0; it < text.size(); it++) {
            char8 = static_cast<uint8_t>(text.at(it));
            it += ekg::utf_check_sequence(char8, char32, utf_string, text, it);

            if (f_renderer.ft_bool_kerning && f_renderer.ft_uint_previous) {
                FT_Get_Kerning(f_renderer.ft_face, f_renderer.ft_uint_previous, char32, 0, &f_renderer.ft_vector_previous_char);
                x += static_cast<float>(f_renderer.ft_vector_previous_char.x >> 6);
            }

            ekg::char_data &char_data {f_renderer.allocated_char_data[char32]};
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
                data.factor += static_cast<int32_t>(x + char32);
            }

            f_renderer.ft_uint_previous = char32;
            x += char_data.wsize;

            if (x + this->embedded_scroll.scroll.x > rect.w) {
                break;
            }
        }

        y += this->text_height;
    }

    allocator.bind_texture(f_renderer.texture);
    allocator.dispatch();

    bool draw_cursor {this->flag.focused && !ekg::reach(ekg::core->ui_timing, 500)};
    for (ekg::rect &cursor_rect : this->cursor_draw_data_list) {
        switch (static_cast<int32_t>(cursor_rect.w)) {
        case 2:
            if (draw_cursor) {
                ekg::draw::rect(cursor_rect + rect + this->embedded_scroll.scroll, theme.textbox_cursor);
            }

            break;
        default:
            ekg::draw::rect(cursor_rect + rect + this->embedded_scroll.scroll, theme.textbox_select);
            break;
        }
    }

    this->embedded_scroll.rect_child = this->rect_text;
    this->embedded_scroll.on_draw_refresh();

    ekg::draw::rect(rect, theme.textbox_outline, ekg::drawmode::outline);
    ekg::draw::bind_off_scissor();
}