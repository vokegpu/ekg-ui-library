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

int32_t ekgmath::clampi(int32_t val, int32_t min, int32_t max) {
    return val < min ? min : (val > max ? max : val);
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

void ekgtext::get_chunk_size(ekgtext::box &box, int32_t &rows, int32_t column_target) {
    if (box.loaded_text_chunk_list.empty()) {
        rows = 0;
        return;
    }

    int32_t index = column_target < 0 ? 0 : (column_target < box.loaded_text_chunk_list.size() ? column_target : (ekgmath::clampi(box.loaded_text_chunk_list.size() - 1, 0, box.loaded_text_chunk_list.size())));
    rows = (int32_t) box.loaded_text_chunk_list[index].size();
}

void ekgtext::process_text_chunks(ekgtext::box &box, std::string &raw_text) {
    box.loaded_text_chunk_list.push_back(raw_text);
}

void ekgtext::should_sync_ui(ekgtext::box &box, const std::string &text, bool &should) {

}

void ekgtext::sync_ui(ekgtext::box &box, const std::string &text) {

}

void ekgtext::process_cursor_pos_relative(ekgtext::box &box, const std::string &text, float &x, float &y) {

}

void ekgtext::process_cursor_pos_index(ekgtext::box &box, int32_t row, int32_t column, int32_t max_row, int32_t max_column) {
    int32_t cursor[4] = {row, column, max_row, max_column};
    int32_t max_columns = box.loaded_text_chunk_list.size() - 1 < 0 ? 0 : (int32_t) box.loaded_text_chunk_list.size() - 1;

    // Sync rows and columns correctly.
    for (uint8_t i = 0; i < 4; i += 2) {
        row = i;
        column = i + 1;

        cursor[column] = cursor[column] < 0 ? 0 : (cursor[column] > max_columns ? max_columns : cursor[column]);
        ekgtext::get_chunk_size(box, max_row, cursor[column]);

        if (cursor[row] > max_row) {
            if (cursor[column] >= max_columns) {
                cursor[row] = max_row;
            } else {
                if (cursor[row] == max_row + 1) {
                    cursor[row] = 0;
                    cursor[column]++;
                } else {
                    cursor[row] = max_row;
                }
            }
        }

        if (cursor[row] < 0) {
            if (cursor[column] > 0) {
                cursor[column]--;
                ekgtext::get_chunk_size(box, max_row, cursor[column]);
                cursor[row] = max_row;
            } else {
                cursor[row] = 0;
            }
        }

        box.cursor[row] = cursor[row];
        box.cursor[column] = cursor[column];
    }
}

void ekgtext::process_new_empty_chunks(ekgtext::box &box, int32_t amount) {
    for (int32_t i = 0; i < amount; i++) {
        box.loaded_text_chunk_list.emplace_back("");
    }
}

void ekgtext::process_new_text(ekgtext::box &box, const std::string& new_text, ekgtext::action action) {
    switch (action) {
        case ekgtext::action::REMOVE_OPPOSITE: {
            if (box.cursor[0] == box.cursor[2]) {
                box.cursor[2]++;
            }

            break;
        }

        case ekgtext::action::REMOVE: {
            if (box.cursor[0] == box.cursor[2]) {
                box.cursor[0]--;
            }

            break;
        }

        case ekgtext::action::INSERT_LINE: {
            ekgtext::process_new_empty_chunks(box, 1);
            break;
        }
    }

    std::vector<int32_t> chunk_index_list;
    ekgtext::get_chunks_index_from_box(box, chunk_index_list, box.cursor);

    if (!chunk_index_list.empty() && (box.cursor[0] >= 0 || action == ekgtext::action::REMOVE || action == ekgtext::action::REMOVE_OPPOSITE)) {
        bool negative = false;

        if (action == ekgtext::action::REMOVE || action == ekgtext::action::REMOVE_OPPOSITE) {
            negative = (box.cursor[0] < 0 || box.cursor[2] < 0) || action == ekgtext::action::REMOVE_OPPOSITE;

            box.cursor[0] = ekgmath::clampi(box.cursor[0], 0, box.cursor[0]);
            box.cursor[2] = ekgmath::clampi(box.cursor[2], 0, box.cursor[2]);
        }

        int32_t min = box.cursor[0];
        int32_t max = box.cursor[2];

        bool begin = false;
        bool end = false;

        const std::vector<int32_t> fast_chunk_index_list = chunk_index_list;
        std::string begin_text;
        chunk_index_list.clear();

        std::string left;
        std::string right;

        int32_t index;

        for (int32_t i = 0; i < fast_chunk_index_list.size(); i++) {
            index = fast_chunk_index_list[i];
            std::string &text = box.loaded_text_chunk_list[index];

            begin = i == 0;
            end = i + 1 == fast_chunk_index_list.size();

            left.clear();
            right.clear();

            if (begin && end) {
                min = ekgmath::clampi(min, 0, (int32_t) text.size());
                max = ekgmath::clampi(max, 0, (int32_t) text.size());

                left = text.substr(0, min);
                right = text.substr(max, text.size());

                if (negative) {
                    switch (action) {
                        case ekgtext::action::REMOVE: {
                            if (!(index != 0 && (index = ekgmath::clampi(index - 1, 0, index)) > -1)) {
                                break;
                            }

                            std::string &above_text = box.loaded_text_chunk_list[index];

                            box.cursor[0] = (int32_t) above_text.size();
                            box.cursor[1] = (int32_t) index;

                            above_text += right;
                            right.clear();
                            chunk_index_list.push_back(index + 1); // add 1 because we sub 1 above.

                            break;
                        }

                        case ekgtext::action::REMOVE_OPPOSITE: {
                            if (min != text.size() || (index = ekgmath::clampi(index + 1, 0, (int32_t) box.loaded_text_chunk_list.size())) == box.loaded_text_chunk_list.size()) {
                                break;
                            }

                            std::string &down_text = box.loaded_text_chunk_list[index];
                            right += down_text;
                            down_text.clear();
                            chunk_index_list.push_back(index);

                            break;
                        }
                    }
                }
            } else if (begin) {
                min = ekgmath::clampi(box.cursor[0], 0, (int32_t) text.size());
                max = (int32_t) text.size();

                left = text.substr(0, min);
                right = text.substr(max, text.size());

                begin_text.clear();

                begin_text += left;
                begin_text += new_text;
                begin_text += right;

                chunk_index_list.push_back(index);
            } else if (end) {
                min = 0;
                max = ekgmath::clampi(box.cursor[2], 0, (int32_t) text.size());

                left = begin_text + text.substr(0, min);
                right = text.substr(max, text.size());

                begin_text.clear();

                begin_text += left;
                begin_text += new_text;
                begin_text += right;
            } else {
                text.clear();
                chunk_index_list.push_back(index);
            }

            text.clear();

            text += left;
            text += new_text;
            text += right;
        }

        for (int32_t &chunk_index : chunk_index_list) {
            if (index < 0 || index >= box.loaded_text_chunk_list.size()) {
                continue;
            }

            box.loaded_text_chunk_list.erase(box.loaded_text_chunk_list.begin() + chunk_index);
        }

        switch (action) {
            case ekgtext::action::INSERT_LINE: {
                std::string temp_text;
                bool first_iteration = true;

                for (int32_t i = box.cursor[1]; i < box.loaded_text_chunk_list.size(); i++) {
                    std::string &text = box.loaded_text_chunk_list[i];

                    if (first_iteration) {
                        left = text.substr(0, box.cursor[0]);
                        right = text.substr(box.cursor[0], text.size());
                        text = left;
                        min = (int32_t) text.size();
                        begin_text = right;
                        first_iteration = false;
                    } else {
                        temp_text = text;
                        text = begin_text;
                        begin_text = temp_text;
                    }
                }

                // Jumping line.
                box.cursor[0] = min + 1;
                break;
            }
        }
    }

    switch (action) {
        case ekgtext::action::INSERT: {
            box.cursor[0]++;
            break;
        }
    }
    
    box.cursor[2] = box.cursor[0];
    box.cursor[3] = box.cursor[1];

    ekgtext::process_cursor_pos_index(box, box.cursor[0], box.cursor[1], box.cursor[2], box.cursor[3]);
}

void ekgtext::process_event(ekgtext::box &box, ekgmath::rect &rect, std::string &raw_text, bool &flag, SDL_Event &sdl_event) {
    switch (sdl_event.type) {
        case SDL_KEYDOWN: {
            auto k = sdl_event.key.keysym.sym;
            bool flag_key_axis_y = false;

            switch (k) {
                case SDLK_ESCAPE: {
                    flag = true;
                    box.final_flag = true;
                    break;
                }

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

                    box.cursor[0] = box.most_large_size;
                    box.cursor[2] = box.cursor[0];

                    flag = true;
                    flag_key_axis_y = true;

                    break;
                }

                case SDLK_DOWN: {
                    box.cursor[1]++;
                    box.cursor[3] = box.cursor[1];

                    box.cursor[0] = box.most_large_size;
                    box.cursor[2] = box.cursor[0];

                    flag = true;
                    flag_key_axis_y = true;

                    break;
                }

                case SDLK_BACKSPACE: {
                    box.cursor[0]--;
                    ekgtext::process_new_text(box, "", ekgtext::action::REMOVE);

                    flag = true;
                    break;
                }

                case SDLK_DELETE: {
                    box.cursor[2]++;
                    ekgtext::process_new_text(box, "", ekgtext::action::REMOVE_OPPOSITE);

                    flag = true;
                    break;
                }
            }

            if (k == SDLK_RETURN2 || k == SDLK_RETURN || k == SDLK_KP_ENTER) {
                ekgtext::process_new_text(box, "", ekgtext::action::INSERT_LINE);
                flag = true;
            }

            if (flag) {
                ekgtext::process_cursor_pos_index(box, box.cursor[0], box.cursor[1], box.cursor[2], box.cursor[3]);
                ekgtext::reset_cursor_loop();

                if (!flag_key_axis_y) {
                    box.most_large_size = box.cursor[0];
                }
            }

            break;
        }

        case SDL_KEYUP: {
            break;
        }

        case SDL_TEXTINPUT: {
            std::string char_str = sdl_event.text.text;

            ekgtext::process_new_text(box, char_str, ekgtext::action::INSERT);
            ekgtext::reset_cursor_loop();

            box.most_large_size = box.cursor[0];
            flag = true;

            break;
        }
    }

    float mx = 0;
    float my = 0;

    bool motion = ekgapi::motion(sdl_event, mx, my) && box.select_flag;
    bool down = ekgapi::any_input_down(sdl_event, mx, my);
    bool up = ekgapi::any_input_up(sdl_event, mx, my);

    if (motion || down || up) {
        bool hovered = rect.collide_aabb_with_point(mx, my);

        if (up && hovered) {
            box.select_flag = false;
        }

        if (!hovered) {
            box.final_flag = true;
            box.select_flag = true;
            return;
        }

        if (down) {
            box.select_flag = true;
        }

        ekg::core->get_font_manager().get_previous_char() = 0;

        ekg_char_data char_data;
        ekgmath::rect curr_rect;

        float x = box.bounds.x;
        float y = box.bounds.y;

        float prev_x = 0;
        float text_height = ekg::core->get_font_manager().get_texture_height();

        float texture_h = 0;
        float impl = (static_cast<float>(ekg::core->get_font_manager().get_texture_height()) / 8) / 2;
        float height = text_height + impl;

        int32_t char_count = 0;
        int32_t text_chunk_size = 0;

        bool visible = false;

        flag = true;
        ekgtext::reset_cursor_loop();

        for (int32_t amount = box.min_chunk_visible; amount < box.max_chunk_visible; amount++) {
            std::string &text = box.loaded_text_chunk_list[amount];
            text_chunk_size = (int32_t) text.size();

            // TODO: Optimize iterations inside iterations (Iterate only visible area).
            for (const char* i = text.c_str(); *i; i++) {
                prev_x = 0;

                ekg::core->get_font_manager().set_previous_char_glyph(i, prev_x);
                ekg::core->get_font_manager().at(char_data, *i);

                x += prev_x;
                texture_h = char_data.height / text_height;

                curr_rect.x = rect.x + x;
                curr_rect.y = rect.y + y;
                curr_rect.w = char_data.offset;
                curr_rect.h = height;

                if ((visible = curr_rect.collide_aabb_with_point(mx, my))) {
                    curr_rect.w /= 2;

                    if (!(visible = curr_rect.collide_aabb_with_point(mx, my))) {
                        char_count++;
                    }

                    if (down) {
                        box.cursor[0] = char_count;
                        box.cursor[1] = amount;
                        box.cursor[2] = box.cursor[0];
                        box.cursor[3] = box.cursor[1];

                        box.index_a = char_count;
                        box.index_b = amount;

                        ekgtext::process_cursor_pos_index(box, box.cursor[0], box.cursor[1], box.cursor[2], box.cursor[3]);
                        box.most_large_size = box.cursor[0];
                    } else if (motion) {
                        if (char_count > box.index_a && amount > box.index_b) {
                            box.cursor[0] = box.index_a;
                            box.cursor[1] = box.index_b;
                            box.cursor[2] = char_count;
                            box.cursor[3] = amount;
                        } else if (char_count < box.index_a && amount < box.index_b) {
                            box.cursor[0] = char_count;
                            box.cursor[1] = amount;
                            box.cursor[2] = box.index_a;
                            box.cursor[3] = box.index_b;
                        }
                    }

                    break;
                }

                x += char_data.offset;
                ekg::core->get_font_manager().get_previous_char() = *i;

                char_count++;
            }

            x = box.bounds.x;
            y += height;
            char_count = 0;
        }

        std::vector<int32_t> chunk_index_list;
        ekgtext::get_chunks_index_from_box(box, chunk_index_list, box.cursor);

        int32_t amount = 0;
        char_count = 0;

        box.loaded_text_select_list.clear();

        for (int32_t i = 0; i < chunk_index_list.size(); i++) {
            amount = chunk_index_list[i];
            std::string &text = box.loaded_text_chunk_list[amount];

            for (char_count = 0; char_count < text.size(); char_count++) {
                if (char_count > box.cursor[0] && amount > box.cursor[0] && char_count < box.cursor[2] && amount < box.cursor[3]) {
                    ekgmath::rect selected_rect;
                    box.loaded_text_select_list.push_back(selected_rect);
                }
            }
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

void ekgtext::process_render_box(ekgtext::box &box, ekgmath::rect &rect, int32_t &scissor_id, bool &draw_cursor) {
    // Draw the background.
    ekggpu::rectangle(rect, ekg::theme().text_input_background);

    int32_t diff = 666;
    uint32_t str_len = 0;

    float text_height = ekg::core->get_font_manager().get_texture_height();
    float impl = (text_height / 8) / 2;
    float height = text_height + impl;

    const bool unique_cursor = box.cursor[2] == box.cursor[0] && box.cursor[3] == box.cursor[1];

    if (!unique_cursor && !box.loaded_text_select_list.empty()) {
        ekg_gpu_data &gpu_data = ekg::core->get_gpu_handler().bind();

        gpu_data.rect[0] = static_cast<float>(static_cast<int32_t>(rect.x));
        gpu_data.rect[1] = static_cast<float>(static_cast<int32_t>(rect.y - impl));

        for (ekgmath::rect rects : box.loaded_text_select_list) {
            if (rects.collide_aabb_with_rect(rect)) {
                ekggpu::push_arr_rect(ekg::core->get_gpu_handler().get_cached_vertices(), rects.x, rects.y, rects.w, rects.h);
                ekggpu::push_arr_rect(ekg::core->get_gpu_handler().get_cached_vertices_materials(), 0.0f, 0.0f, 0.0f, 0.0f);
            }
        }

        gpu_data.data = (int32_t) box.loaded_text_select_list.size();

        gpu_data.color[0] = ekg::theme().text_input_activy.x;
        gpu_data.color[1] = ekg::theme().text_input_activy.y;
        gpu_data.color[2] = ekg::theme().text_input_activy.z;
        gpu_data.color[3] = ekg::theme().text_input_activy.w;
    }

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

    float prev_x = 0;
    float render_x = 0, render_y = 0, render_w = 0, render_h = 0;
    float texture_x = 0, texture_y = 0, texture_w = 0, texture_h = 0;

    int32_t char_count = 0;
    int32_t text_chunk_size = 0;

    bool visible = false;
    bool first_char_visible = true;

    box.min_chunk_visible = 0;
    box.max_chunk_visible = 0;

    for (int32_t amount = 0; amount < box.loaded_text_chunk_list.size(); amount++) {
        std::string &text = box.loaded_text_chunk_list[amount];
        text_chunk_size = (int32_t) text.size();

        // TODO: Optimize iterations inside iterations (Iterate only visible area).
        for (const char* i = text.c_str(); *i; i++) {
            prev_x = 0;

            ekg::core->get_font_manager().set_previous_char_glyph(i, prev_x);
            ekg::core->get_font_manager().at(char_data, *i);

            x += prev_x;
            render_w = char_data.width;
            render_h = char_data.height;

            texture_x = char_data.x;
            texture_w = render_w / ekg::core->get_font_manager().get_texture_width();
            texture_h = render_h / text_height;

            // Get char from list and update metrics/positions of each char.
            render_x = x + char_data.left;
            render_y = y + (text_height - char_data.top);

            curr_rect.x = rect.x + x;
            curr_rect.y = rect.y + y;
            curr_rect.w = render_w; 
            curr_rect.h = height;

            diff -= *i;
            diff -= static_cast<int32_t>(render_y);

            if ((visible = curr_rect.collide_aabb_with_rect(rect))) {
                str_len++;

                if (first_char_visible) {
                    box.min_chunk_visible = amount;
                    first_char_visible = false;
                }

                // Increase 1 because this var is used as iteration factor.
                box.max_chunk_visible = amount + 1;

                ekggpu::push_arr_rect(ekg::core->get_gpu_handler().get_cached_vertices(), render_x, render_y, render_w, render_h);
                ekggpu::push_arr_rect(ekg::core->get_gpu_handler().get_cached_vertices_materials(), texture_x, texture_y, texture_w, texture_h);
            }

            bool cursor_out_of_str_range = char_count + 1 == text_chunk_size && char_count + 1 == box.cursor[0];

            if (unique_cursor && (visible || *i == 32) && (cursor_out_of_str_range || char_count == box.cursor[0]) && amount == box.cursor[1]) {
                curr_rect.x += cursor_out_of_str_range ? char_data.offset : 0;
                curr_rect.w = 2;
                curr_rect.h = height;
                cursor_rect.copy(curr_rect);
            }

            x += char_data.offset;
            ekg::core->get_font_manager().get_previous_char() = *i;
            char_count++;
        }

        if (text.empty() && box.cursor[1] == amount) {
            cursor_rect.x = rect.x + x;
            cursor_rect.y = rect.y + y;
            cursor_rect.w = 2;
            cursor_rect.h = height;
        }

        x = box.bounds.x;
        y += height;

        char_count = 0;
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
    gpu_data.factor = diff - ((int32_t) str_len);

    // Bind the texture to GPU.
    ekg::core->get_gpu_handler().bind_texture(gpu_data, ekg::core->get_font_manager().get_bitmap_texture_id());
    ekg::core->get_gpu_handler().free(gpu_data);

    if (draw_cursor && unique_cursor) {
        ekggpu::rectangle(cursor_rect, ekg::theme().text_input_string);
    }

    // Draw the border:
    ekggpu::rectangle(rect, ekg::theme().text_input_border, 1);
}

void ekgtext::get_chunks_index_from_box(ekgtext::box &box, std::vector<int32_t> &chunks_index, int32_t *index_box) {
    for (int32_t i = box.cursor[1]; i <= box.cursor[3]; i++) {
        if (i < 0 || i >= box.loaded_text_chunk_list.size()) {
            continue;
        }

        chunks_index.push_back(i);
    }
}
