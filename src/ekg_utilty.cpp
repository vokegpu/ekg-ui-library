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
    float width = ekg::the_ekg_core->get_screen_width();
    float height = ekg::the_ekg_core->get_screen_height();

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

void ekgtext::get_rows(ekgtext::box &box, int32_t &rows, int32_t &column_target) {
    if (box.rows_per_columns.empty()) {
        column_target = 0;
        return;
    }

    column_target = column_target > box.rows_per_columns.size() ? box.rows_per_columns.size() : column_target;
    uint32_t sub_previous = column_target - 1 < 0 ? 0 : box.rows_per_columns[column_target - 1];
    uint32_t concurrent_rows = box.rows_per_columns[column_target] - sub_previous;

    rows = concurrent_rows;
}

void ekgtext::get_char_index(ekgtext::box &box, int32_t &index, int32_t rows, int32_t column) {
    if (box.rows_per_columns.empty()) {
        index = -1;
        return;
    }

    index = column - 1 < 0 ? static_cast<int32_t>(rows) : static_cast<int32_t>(box.rows_per_columns[column - 1] + rows);
}

void ekgtext::process_text_rows(ekgtext::box &box, std::string &text, const std::string &raw_text) {
    text.clear();

    int32_t rows_in = 0;
    int32_t columns_in = 0;

    bool skip_line_flag = false;

    for (uint32_t i = 0; i < raw_text.size(); i++) {
        rows_in++;
        skip_line_flag = (raw_text.at(i) == '\\' && i + 1 < raw_text.size() && raw_text.at(i + 1) == 'n');

        if (rows_in > box.max_rows || skip_line_flag) {
            if (box.rows_per_columns.size() < columns_in) {
                box.rows_per_columns.push_back(rows_in);
            } else {
                box.rows_per_columns[columns_in] = rows_in;
            }
        }

        if (!skip_line_flag) {
            text += raw_text.at(i);
        }
    }
}

void ekgtext::should_sync_ui(ekgtext::box &box, const std::string &text, bool &should) {

}

void ekgtext::sync_ui(ekgtext::box &box, const std::string &text) {

}

void ekgtext::process_cursor_pos_relative(ekgtext::box &box, const std::string &text, float &x, float &y) {

}

void ekgtext::process_cursor_pos_index(ekgtext::box &box, int32_t row, int32_t column) {
    if (box.cursor[0] == row && box.cursor[1] == column && box.cursor[2] == row && box.cursor[3] == row) {
        return;
    }

    const uint32_t previous_row = row;
    get_rows(box, row, column);

    if (row < previous_row) {
        row = previous_row;
    }

    box.cursor[0] = row;
    box.cursor[1] = column;
    box.cursor[2] = row;
    box.cursor[3] = column;
}

void ekgtext::process_new_text(ekgtext::box &box, std::string &previous_text, std::string &text, std::string &raw_text) {
    int32_t cursor[4];

    cursor[0] = box.cursor[0];
    cursor[1] = box.cursor[1];

    get_rows(box, cursor[0], cursor[1]);
    get_rows(box, cursor[2], cursor[3]);

    std::string left  = raw_text.substr(0, cursor[0]);
    std::string right = raw_text.substr(cursor[2], text.size());

    raw_text = left + previous_text + right;
    ekgtext::process_text_rows(box, text, raw_text);
}

void ekgtext::process_event(ekgtext::box &box, const ekgmath::rect &rect, std::string &text, std::string &raw_text, SDL_Event &sdl_event) {
    switch (sdl_event.type) {
        case SDL_KEYDOWN: {
            break;
        }

        case SDL_KEYUP: {
            break;
        }

        case SDL_TEXTINPUT: {
            std::string char_str = sdl_event.text.text;
            ekgtext::process_new_text(box, text, char_str, raw_text);
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

        if (begin == -1 || end == -1) {
            return;
        }

        ekgmath::rect curr_rect;
        char char_str = 0;
        ekg_char_data char_data;

        float x = box.bounds.x;
        float y = box.bounds.y;

        bool hovered = false;
        float impl = (static_cast<float>(ekg::the_ekg_core->get_font_manager().get_texture_height()) / 8) / 2;

        int32_t rows_in = 0;
        int32_t columns_in = 0;

        ekg::the_ekg_core->get_font_manager().get_previous_char() = 0;

        ekgutil::log(std::to_string(end));

        for (uint32_t i = begin; i < end; i++) {
            char_str = text.at(i);

            ekg::the_ekg_core->get_font_manager().accept_char(&char_str, x);
            ekg::the_ekg_core->get_font_manager().at(char_data, char_str);

            rows_in++;

            if (rows_in > box.max_rows || (box.rows_per_columns.size() > columns_in && rows_in > box.rows_per_columns[columns_in])) {
                rows_in = 0;
                columns_in++;

                x = box.bounds.x;
                y += static_cast<float>(ekg::the_ekg_core->get_font_manager().get_texture_height());
            }

            curr_rect.x = rect.x + x;
            curr_rect.y = rect.y + y;
            curr_rect.w = char_data.width;
            curr_rect.h = char_data.height;

            if (curr_rect.collide_aabb_with_point(mx, my)) {
                box.cursor[0] = rows_in;
                box.cursor[1] = columns_in;
                box.cursor[2] = rows_in;
                box.cursor[3] = columns_in;

                ekgutil::log("hi funf");
                break;
            }

            x += char_data.texture_x;
            ekg::the_ekg_core->get_font_manager().get_previous_char() = char_str;
        }
    }
}

void ekgtext::process_render_box(ekgtext::box &box, const std::string &text, ekgmath::rect &rect, int32_t &scissor_id, bool &draw_cursor) {
    // Draw the background.
    ekggpu::rectangle(rect, ekg::theme().text_input_background);

    const char* char_str = text.c_str();
    uint32_t str_len = 0;

    float render_x = 0, render_y = 0, render_w = 0, render_h = 0;
    float texture_x = 0, texture_y = 0, texture_w = 0, texture_h = 0;
    float impl = (static_cast<float>(ekg::the_ekg_core->get_font_manager().get_texture_height()) / 8) / 2;

    int32_t diff = 1;
    const bool skip_cursor = box.cursor[2] == box.cursor[0] && box.cursor[3] == box.cursor[1];

    // Generate a GPU data.
    ekg_gpu_data &gpu_data = ekg::the_ekg_core->get_gpu_handler().bind();

    // The position post draw should be equals to max bitmap height divided by 2.
    gpu_data.rect[0] = static_cast<float>(static_cast<int32_t>(rect.x));
    gpu_data.rect[1] = static_cast<float>(static_cast<int32_t>(rect.y - impl));

    ekg::the_ekg_core->get_font_manager().get_previous_char() = 0;

    ekg_char_data char_data;
    ekgmath::rect curr_rect;
    ekgmath::rect cursor_rect;

    float x = box.bounds.x;
    float y = box.bounds.y;

    int32_t rows_in = 0;
    int32_t columns_in = 0;

    box.visible[0] = 0;
    box.visible[1] = 0;
    box.visible[2] = 0;
    box.visible[3] = 0;

    bool once_visible = true;
    bool visible = false;

    for (const char* i = char_str; *i; i++) {
        ekg::the_ekg_core->get_font_manager().accept_char(i, x);
        ekg::the_ekg_core->get_font_manager().at(char_data, *i);

        render_w = char_data.width;
        render_h = char_data.height;

        texture_x = char_data.x;
        texture_w = render_w / static_cast<float>(ekg::the_ekg_core->get_font_manager().get_texture_width());
        texture_h = render_h / static_cast<float>(ekg::the_ekg_core->get_font_manager().get_texture_height());
        diff += static_cast<int32_t>(texture_x);

        rows_in++;

        if (rows_in > box.max_rows || (box.rows_per_columns.size() > columns_in && rows_in > box.rows_per_columns[columns_in])) {
            rows_in = 0;
            columns_in++;

            x = box.bounds.x;
            y += static_cast<float>(ekg::the_ekg_core->get_font_manager().get_texture_height());
        }

        // Get char from list and update metrics/positions of each char.
        render_x = x + char_data.left;
        render_y = y + (static_cast<float>(ekg::the_ekg_core->get_font_manager().get_texture_height()) - char_data.top);

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

            ekggpu::push_arr_rect(ekg::the_ekg_core->get_gpu_handler().get_cached_vertices(), render_x, render_y, render_w, render_h);
            ekggpu::push_arr_rect(ekg::the_ekg_core->get_gpu_handler().get_cached_vertices_materials(), texture_x, texture_y, texture_w, texture_h);
        }

        if (skip_cursor && rows_in == box.cursor[0] && columns_in == box.cursor[1]) {
            curr_rect.x -= 2;
            curr_rect.w = 2;
            curr_rect.h = ekg::the_ekg_core->get_font_manager().get_texture_height() + impl;
            cursor_rect.copy(curr_rect);
        }

        x += char_data.texture_x;
        ekg::the_ekg_core->get_font_manager().get_previous_char() = *i;
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
    gpu_data.factor = (static_cast<float>(str_len)) * static_cast<float>(diff) * texture_w * x;

    // Bind the texture to GPU.
    ekg::the_ekg_core->get_gpu_handler().bind_texture(gpu_data, ekg::the_ekg_core->get_font_manager().get_bitmap_texture_id());
    ekg::the_ekg_core->get_gpu_handler().free(gpu_data);

    // Draw the selected chars.
    if (!ekgtext::box::selected_column_list.empty() && !skip_cursor) {
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