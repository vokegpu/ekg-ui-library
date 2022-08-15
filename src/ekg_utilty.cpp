/**
 * EKG-LICENSE - this software/library LICENSE can not be modified in any instance.
 *
 * --
 * ANY NON-OFFICIAL MODIFICATION IS A CRIME.
 * DO NOT SELL THIS CODE SOFTWARE, FOR USE EKG IN A COMMERCIAL PRODUCT ADD EKG-LICENSE TO PROJECT,
 * RESPECT THE COPYRIGHT TERMS OF EKG, NO SELL WITHOUT EKG-LICENSE (IT IS A CRIME).
 * DO NOT FORK THE PROJECT SOURCE WITHOUT EKG-LICENSE.
 *
 * END OF EKG-LICENSE.
 **/
#include "ekg/ekg.hpp"
#include "ekg/api/ekg_utility.hpp"

#include <math.h>

std::vector<ekgmath::rect> ekgtext::box::selected_column_list;

void ekgutil::log(const std::string &log) {
    ekgapi::send_output(log.c_str());
}

bool ekgutil::contains(uint16_t &flags, uint16_t target) {
    return (flags & target) != 0;
}

bool ekgutil::remove(uint16_t &flags, uint16_t target) {
    bool flag_contains = flags & target;
    flags &= ~(target);
    return flag_contains;
}

bool ekgutil::add(uint16_t &flags, uint16_t val_flag) {
    if (ekgutil::contains(flags, val_flag)) {
        return false;
    }

    flags |= val_flag;
    return true;
}

bool ekgutil::find_axis_dock(uint16_t &target, float px, float py, float offset, ekgmath::rect &rect) {
    target = ekg::dock::UNDEFINED;

    if (!rect.collide_aabb_with_point(px, py)) {
        return false;
    }

    float x = rect.x;
    float y = rect.y;
    float w = rect.w;
    float h = rect.h;

    bool phase1 = px > x && px < x + offset;
    bool phase2 = py > y && py < y + offset;
    bool phase3 = px > x + w - offset && px < x + w;
    bool phase4 = py > y + h - offset && py < y + h;
    bool phase5 = px > x + (w / 2) - offset && px < x + (w / 2) + offset &&
                  py > y + (h / 2) - offset && py > y + (h / 2) + offset;

    target = phase1 ? target | ekg::dock::LEFT   : target;
    target = phase2 ? target | ekg::dock::TOP    : target;
    target = phase3 ? target | ekg::dock::RIGHT  : target;
    target = phase4 ? target | ekg::dock::BOTTOM : target;
    target = phase5 ? target | ekg::dock::CENTER : target;

    return target != ekg::dock::UNDEFINED;
}

bool ekgutil::contains(const std::string &str1, const std::string &str2) {
    return str1.find(str2) != std::string::npos;
}

bool ekgutil::stack::contains(uint32_t id) {
    for (uint32_t &_ids : this->ids) {
        if (_ids == id) {
            return true;
        }
    }

    return false;
}

bool ekgutil::stack::remove(uint32_t id) {
    int32_t index = -1;

    for (uint32_t i = 0; i < this->ids.size(); i++) {
        if (this->ids.at(i) == id) {
            index = (int32_t) i;
            break;
        }
    }

    if (index != -1) {
        this->ids.erase(this->ids.begin() + index);
        return true;
    }

    return false;
}

void ekgutil::stack::add(uint32_t id) {
    this->ids.push_back(id);
}

void ekgutil::stack::add(ekgutil::stack &stack) {
    this->ids.insert(this->ids.end(), stack.ids.begin(), stack.ids.end());
}

bool ekgmath::rect::collide_aabb_with_point(float px, float py) {
    return px > this->x && px < this->x + this->w && py > this->y && py < this->y + this->h;
}

void ekgmath::rect::copy(ekgmath::rect &r) {
    this->x = r.x;
    this->y = r.y;
    this->w = r.w;
    this->h = r.h;
}

bool ekgmath::rect::collide_aabb_with_rect(const ekgmath::rect &r) {
    return this->x < r.x + r.w && this->x + this->w > r.x && this->y < r.y + r.h && this->y + this->w > r.y;
}

float ekgmath::lerpf(float a, float b, float t) {
    return a + (b - a) * t;
}

float ekgmath::clampf(float val, float min, float max) {
    return val > min ? (val < max ? val : max) : min;
}

void ekgmath::clamp_aabb_with_screen_size(float &minx, float &miny, float maxx, float maxy) {
    float width = ekg::core->get_screen_width();
    float height = ekg::core->get_screen_height();

    if (minx < 0) {
        minx = 0;
    }

    if (miny < 0) {
        miny = 0;
    }

    if (minx + maxx > width) {
        minx = width - maxx;
    }

    if (miny + maxy > height) {
        miny = height - maxy;
    }
}

void ekgmath::ortho2d(float *mat, float left, float right, float bottom, float top) {
    const float z_near = -1.0f;
    const float z_far = 1.0f;
    const float inv_z = 1.0f / (z_far - z_near);
    const float inv_y = 1.0f / (top - bottom);
    const float inv_x = 1.0f / (right - left);

    uint8_t i = 0;

    mat[i++] = 2.0f * inv_x;
    mat[i++] = 0.0f;
    mat[i++] = 0.0f;
    mat[i++] = 0.0f;

    mat[i++] = 0.0f;
    mat[i++] = 2.0f * inv_y;
    mat[i++] = 0.0f;
    mat[i++] = 0.0f;

    mat[i++] = 0.0f;
    mat[i++] = 0.0f;
    mat[i++] = -2.0f * inv_z;
    mat[i++] = 0.0f;

    mat[i++] = -(right + left) * inv_x;
    mat[i++] = -(top + bottom) * inv_y;
    mat[i++] = -(z_far + z_near) * inv_z;
    mat[i++] = 1.0f;
}

bool ekgmath::collide_aabb_with_point(float x, float y, float w, float h) {
    return ekgapi::display_interact_x > x && ekgapi::display_interact_x < x + w &&
           ekgapi::display_interact_y > y && ekgapi::display_interact_y < y + h;
}

void ekgmath::smoothf(float &val, float duration, uint32_t ticks) {
    duration = static_cast<float>(ticks) / duration;
    val = ekgmath::clampf(6 * pow(duration, 5) - (15 * pow(duration, 4)) + (10 * pow(duration, 3)), 0.0f, 1.0f);
}

ekgmath::vec2f::vec2f(float x, float y) {
    this->x = x;
    this->y = y;
}

ekgmath::vec4f::vec4f(float x, float y, float z, float w) {
    this->x = x;
    this->y = y;
    this->z = z;
    this->w = w;
}

ekgmath::vec4f::vec4f(int32_t red, int32_t green, int32_t blue, int32_t alpha) {
    this->color(red, green, blue, alpha);
}

void ekgmath::vec4f::color(int32_t red, int32_t green, int32_t blue, int32_t alpha) {
    this->x = static_cast<float>(red) / 255;
    this->y = static_cast<float>(green) / 255;
    this->z = static_cast<float>(blue) / 255;
    this->w = static_cast<float>(alpha) / 255;
}

void ekgtext::get_rows(ekgtext::box &box, int32_t &rows, int32_t column_target) {
    if (box.rows_per_columns.empty()) {
        column_target = 0;
        return;
    }

    column_target = column_target > box.rows_per_columns.size() ? (int32_t) box.rows_per_columns.size() : column_target;
    int32_t sub_previous = column_target - 1 < 0 ? 0 : box.rows_per_columns[column_target - 1];
    int32_t concurrent_rows = box.rows_per_columns[column_target] - sub_previous;

    rows = concurrent_rows;
}

void ekgtext::get_char_index(ekgtext::box &box, int32_t &index, int32_t rows, int32_t column) {
    if (box.rows_per_columns.empty()) {
        index = -1;
        return;
    }

    index = column - 1 < 0 ? rows : box.rows_per_columns[column - 1] + rows;
}

void ekgtext::process_text_rows(ekgtext::box &box, std::string &text, const std::string &raw_text) {
    text.clear();
    box.rows_per_columns.clear();

    int32_t rows_in = 0;
    int32_t columns_in = 0;
    int32_t total_rows_in = 0;

    bool end = false;
    bool jump_line = false;

    for (uint32_t i = 0; i < raw_text.size(); i++) {
        end = i + 1 == raw_text.size();

        if (i < box.break_line_list.size()) {
            jump_line = rows_in == box.break_line_list[columns_in];
        } else {
            jump_line = false;
            box.break_line_list.push_back(-1);
        }

        if (rows_in > box.max_rows || jump_line || end) {
            total_rows_in += rows_in + end;
            box.rows_per_columns.push_back(total_rows_in);

            ekgutil::log(std::to_string(rows_in));

            rows_in = 0;
            columns_in++;
        }

        text += raw_text.at(i);
        rows_in++;
    }
}

void ekgtext::should_sync_ui(ekgtext::box &box, const std::string &text, bool &should) {

}

void ekgtext::sync_ui(ekgtext::box &box, const std::string &text) {

}

void ekgtext::process_cursor_pos_relative(ekgtext::box &box, const std::string &text, float &x, float &y) {

}

void ekgtext::process_cursor_pos_index(ekgtext::box &box, int32_t row, int32_t column, int32_t max_row, int32_t max_column) {
    int32_t cursor[4] = {row, column, max_row, max_column};
    int32_t max_columns = box.rows_per_columns.size() - 1 < 0 ? 0 : (int32_t) box.rows_per_columns.size() - 1;

    for (uint8_t i = 0; i < 4; i += 2) {
        row = i;
        column = i + 1;

        cursor[column] = cursor[column] < 0 ? 0 : (cursor[column] > max_columns ? max_columns : cursor[column]);
        ekgtext::get_rows(box, max_row, cursor[column]);

        if (cursor[row] > max_row) {
            if (cursor[column] >= max_columns) {
                cursor[row] = max_row;
            } else {
                cursor[row] = 0;
                cursor[column]++;
            }
        }

        if (cursor[row] < 0) {
            if (cursor[column] > 0) {
                cursor[column]--;
                ekgtext::get_rows(box, max_row, cursor[column]);
                cursor[row] = max_row;
            } else {
                cursor[row] = 0;
            }
        }

        box.cursor[row] = cursor[row];
        box.cursor[column] = cursor[column];
    }
}

void ekgtext::process_new_text(ekgtext::box &box, std::string &previous_text, std::string text, std::string &raw_text, int32_t factor) {
    if (box.cursor[2] == box.cursor[0] && box.cursor[3] == box.cursor[1]) {
        int32_t index = 0;
        int32_t max_rows = 0;

        ekgtext::get_char_index(box, index, box.cursor[0], box.cursor[1]);
        ekgtext::get_rows(box, max_rows, box.cursor[1]);

        if (text == "\n") {
            text.clear();
            box.break_line_list[box.cursor[1]] = box.cursor[0];
        }

        int32_t row_break_line = 0;
        ekgtext::get_row_break_line(box, row_break_line, box.cursor[1]);

        if (factor == 0 && box.cursor[0] == max_rows) {
            box.cursor[0]++;
            box.cursor[2] = box.cursor[0];

            ekgtext::process_cursor_pos_index(box, box.cursor[0], box.cursor[1], box.cursor[2], box.cursor[3]);
        }

        if (factor == 1 && box.cursor[0] == max_rows) {
            box.break_line_list[box.cursor[1]] = -1;
        }

        if (box.cursor[0] < -1 || index > raw_text.size()) {
            return;
        }

        factor = index + factor > raw_text.size() ? 0 : factor;

        std::string left = raw_text.substr(0, index);
        std::string right = raw_text.substr(index + factor - (box.cursor[0] != 0 && box.cursor[0] == max_rows), raw_text.size());

        raw_text = left + text + right;
        ekgtext::process_text_rows(box, previous_text, raw_text);
    }
}

void ekgtext::process_event(ekgtext::box &box, const ekgmath::rect &rect, std::string &text, std::string &raw_text, bool &flag, SDL_Event &sdl_event) {
    switch (sdl_event.type) {
        case SDL_KEYDOWN: {
            auto k = sdl_event.key.keysym.sym;

            switch (k) {
                case SDLK_LEFT: {
                    box.cursor[0]--;
                    box.cursor[2] = box.cursor[0];
                    flag = true;
                    break;
                }

                case SDLK_RIGHT: {
                    box.cursor[0]++;
                    box.cursor[2] = box.cursor[0];
                    flag = true;
                    break;
                }

                case SDLK_UP: {
                    box.cursor[1]--;
                    box.cursor[3] = box.cursor[1];
                    flag = true;
                    break;
                }

                case SDLK_DOWN: {
                    box.cursor[1]++;
                    box.cursor[3] = box.cursor[1];
                    flag = true;
                    break;
                }

                case SDLK_BACKSPACE: {
                    box.cursor[0]--;
                    box.cursor[2] = box.cursor[0];

                    ekgtext::process_new_text(box, text, "", raw_text, 1);
                    flag = true;

                    break;
                }

                case SDLK_RETURN: {
                    std::string char_str = "\n";
                    ekgtext::process_new_text(box, text, char_str, raw_text);

                    flag = true;

                    break;
                }
            }

            if (flag) {
                ekgtext::process_cursor_pos_index(box, box.cursor[0], box.cursor[1], box.cursor[2], box.cursor[3]);
                ekgtext::reset_cursor_loop();
            }

            break;
        }

        case SDL_KEYUP: {
            break;
        }

        case SDL_TEXTINPUT: {
            std::string char_str = sdl_event.text.text;
            ekgtext::process_new_text(box, text, char_str, raw_text);

            box.cursor[0]++;
            box.cursor[2] = box.cursor[0];

            ekgtext::process_cursor_pos_index(box, box.cursor[0], box.cursor[1], box.cursor[2], box.cursor[3]);
            ekgtext::reset_cursor_loop();

            flag = true;
            break;
        }
    }

    float mx = 0;
    float my = 0;

    if (ekgapi::any_input_down(sdl_event, mx, my)) {
        int32_t begin = 0;
        int32_t end = 0;

        ekgtext::get_char_index(box, begin, box.visible[0], box.visible[1]);
        ekgtext::get_char_index(box, end, box.visible[2], box.visible[3]);

        flag = true;
        ekgtext::reset_cursor_loop();

        if (begin == -1 || end == -1) {
            return;
        }

        ekgmath::rect curr_rect;

        char char_str = 0;
        ekg_char_data char_data;

        float x = box.bounds.x;
        float y = box.bounds.y;
        float previous_x = 0;

        flag = false;
        bool flag_rows_per_columns = false;

        float impl = (static_cast<float>(ekg::core->get_font_manager().get_texture_height()) / 8) / 2;

        int32_t rows_in = 0;
        int32_t columns_in = 0;

        ekg::core->get_font_manager().get_previous_char() = 0;

        for (uint32_t i = begin; i < end + 1; i++) {
            if (i < 0 || i >= text.size()) {
                continue;
            }

            char_str = text.at(i);
            previous_x = 0;

            ekg::core->get_font_manager().set_previous_char_glyph(&char_str, previous_x);
            ekg::core->get_font_manager().at(char_data, char_str);

            x += previous_x;
            flag_rows_per_columns = rows_in > box.max_rows || (columns_in < box.rows_per_columns.size() && rows_in > box.rows_per_columns[columns_in]);

            if (flag_rows_per_columns) {
                rows_in = 0;
                columns_in++;

                x = box.bounds.x;
                y += static_cast<float>(ekg::core->get_font_manager().get_texture_height());
            }

            curr_rect.x = rect.x + x;
            curr_rect.y = rect.y + y;
            curr_rect.w = char_data.width + char_data.left + (char_str == ' ' ? char_data.texture_x : 0);
            curr_rect.h = static_cast<float>(ekg::core->get_font_manager().get_texture_height());

            flag = curr_rect.collide_aabb_with_point(mx, my);

            if (flag) {
                curr_rect.w /= 2;
                flag = curr_rect.collide_aabb_with_point(mx, my);                

                if (!flag && ((i + 1 < text.size() && !flag_rows_per_columns) || (rows_in + 1 == box.rows_per_columns[columns_in] + 1))) {
                    rows_in++;
                }

                box.cursor[0] = rows_in;
                box.cursor[1] = columns_in;
                box.cursor[2] = rows_in;
                box.cursor[3] = columns_in;

                break;
            }

            x += char_data.texture_x;
            rows_in++;

            ekg::core->get_font_manager().get_previous_char() = char_str;
        }
    }
}

void ekgtext::reset_cursor_loop() {
    ekgapi::ui_clock_text.reset();
}

void ekgtext::process_cursor_loop(bool &flag) {
    bool wait_for_500ms = !ekgapi::ui_clock_text.reach(500);

    if (ekgapi::ui_clock_text.reach(1000)) {
        ekgapi::ui_clock_text.reset();
    }

    flag = wait_for_500ms;
}

void ekgtext::process_render_box(ekgtext::box &box, const std::string &text, ekgmath::rect &rect, int32_t &scissor_id, bool &draw_cursor) {
    // Draw the background.
    ekggpu::rectangle(rect, ekg::theme().text_input_background);

    const char* char_str = text.c_str();
    uint32_t str_len = 0;

    float render_x = 0, render_y = 0, render_w = 0, render_h = 0;
    float texture_x = 0, texture_y = 0, texture_w = 0, texture_h = 0;
    float impl = (static_cast<float>(ekg::core->get_font_manager().get_texture_height()) / 8) / 2;

    int32_t diff = 666;
    const bool unique_cursor = box.cursor[2] == box.cursor[0] && box.cursor[3] == box.cursor[1];

    // Generate a GPU data.
    ekg_gpu_data &gpu_data = ekg::core->get_gpu_handler().bind();

    // The position post draw should be equals to max bitmap height divided by 2.
    gpu_data.rect[0] = static_cast<float>(static_cast<int32_t>(rect.x));
    gpu_data.rect[1] = static_cast<float>(static_cast<int32_t>(rect.y - impl));

    ekg::core->get_font_manager().get_previous_char() = 0;

    ekg_char_data char_data;
    ekgmath::rect curr_rect;
    ekgmath::rect cursor_rect;

    float x = box.bounds.x;
    float y = box.bounds.y;

    int32_t rows_in = 0;
    int32_t columns_in = 0;
    int32_t sizeof_columns = box.rows_per_columns.size() - 1 < 0 ? 0 : (int32_t) box.rows_per_columns.size() - 1;
    int32_t rows_per_column = 0;

    box.visible[0] = 0;
    box.visible[1] = 0;
    box.visible[2] = 0;
    box.visible[3] = 0;

    bool once_visible = true;
    bool visible = false;

    float prev_x = 0;

    for (const char* i = char_str; *i; i++) {
        prev_x = 0;

        ekg::core->get_font_manager().set_previous_char_glyph(i, prev_x);
        ekg::core->get_font_manager().at(char_data, *i);

        x += prev_x;

        render_w = char_data.width;
        render_h = char_data.height;

        texture_x = char_data.x;
        texture_w = render_w / static_cast<float>(ekg::core->get_font_manager().get_texture_width());
        texture_h = render_h / static_cast<float>(ekg::core->get_font_manager().get_texture_height());
        diff -= *i;

        ekgtext::get_rows(box, rows_per_column, columns_in);

        if (rows_in > box.max_rows || rows_in > rows_per_column) {
            rows_in = 0;
            columns_in++;

            x = box.bounds.x;
            y += static_cast<float>(ekg::core->get_font_manager().get_texture_height());
        }

        // Get char from list and update metrics/positions of each char.
        render_x = x + char_data.left;
        render_y = y + (static_cast<float>(ekg::core->get_font_manager().get_texture_height()) - char_data.top);

        curr_rect.x = rect.x + x;
        curr_rect.y = rect.y + y;
        curr_rect.w = render_w;
        curr_rect.h = render_h;

        if ((visible = rect.collide_aabb_with_rect(curr_rect))) {
            str_len++;

            if (once_visible) {
                box.visible[0] = rows_in;
                box.visible[1] = columns_in;
                once_visible = false;
            }

            box.visible[2] = rows_in;
            box.visible[3] = columns_in;

            ekggpu::push_arr_rect(ekg::core->get_gpu_handler().get_cached_vertices(), render_x, render_y, render_w, render_h);
            ekggpu::push_arr_rect(ekg::core->get_gpu_handler().get_cached_vertices_materials(), texture_x, texture_y, texture_w, texture_h);
        }

        bool cursor_out_of_str_range = rows_in + 1 == rows_per_column && rows_in + 1 == box.cursor[0] && columns_in == box.cursor[1];

        if (unique_cursor && (cursor_out_of_str_range || (rows_in == box.cursor[0] && columns_in == box.cursor[1]))) {
            curr_rect.x += cursor_out_of_str_range ? (char_data.width == 0 ? char_data.texture_x : char_data.width) : 0;
            curr_rect.w = 2;
            curr_rect.h = ekg::core->get_font_manager().get_texture_height() + impl;
            cursor_rect.copy(curr_rect);

            ekgutil::log(std::to_string(rows_in) + " " + std::to_string(rows_per_column));
        }

        x += char_data.texture_x;

        rows_in++;
        ekg::core->get_font_manager().get_previous_char() = *i;
    }

    // Configure
    // Each char quad has 6 vertices, so we multiply 6 by length of text.
    gpu_data.data = (GLint) (str_len * 6);

    // Pass color of texture.
    gpu_data.color[0] = ekg::theme().text_input_string.x;
    gpu_data.color[1] = ekg::theme().text_input_string.y;
    gpu_data.color[2] = ekg::theme().text_input_string.z;
    gpu_data.color[3] = ekg::theme().text_input_string.w;

    // Set the factor difference.
    gpu_data.factor = diff - ((int32_t) str_len * 6);

    // Bind the texture to GPU.
    ekg::core->get_gpu_handler().bind_texture(gpu_data, ekg::core->get_font_manager().get_bitmap_texture_id());
    ekg::core->get_gpu_handler().free(gpu_data);

    // Draw the selected chars.
    if (!ekgtext::box::selected_column_list.empty() && !unique_cursor) {
        for (ekgmath::rect &rects : ekgtext::box::selected_column_list) {
            ekggpu::rectangle(rects, ekg::theme().text_input_activy);
        }

        draw_cursor = false;
    }

    if (draw_cursor) {
        ekggpu::rectangle(cursor_rect, ekg::theme().text_input_string);
    }

    // Draw the border:
    ekggpu::rectangle(rect, ekg::theme().text_input_border, 1);
}

void ekgtext::get_row_break_line(ekgtext::box &box, int32_t &row_target, int32_t column) {
    if (box.break_line_list.size() < column) {
        row_target = -1;
        return;
    }

    row_target = box.break_line_list[column] - 1 < 0 ? -1 : box.break_line_list[column] - 1;
}
