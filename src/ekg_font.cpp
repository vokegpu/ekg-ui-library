#include "ekg/api/ekg_font.hpp"
#include <ekg/ekg.hpp>

float ekg_font::get_text_width(const std::string &text) {
    this->previous = 0;
    FT_Vector vec;

    float start_x = 0;
    float render_x = 0;
    float text_width = 0;

    for (const char* i = text.c_str(); *i; i++) {
        if (this->use_kerning && this->previous && *i) {
            FT_Get_Kerning(this->face, this->previous, *i, 0, &vec);
            start_x += (float) (vec.x >> 6);
        }

        ekg_char_data &char_data = this->char_list[*i];

        render_x = start_x + (float) char_data.left;
        start_x += char_data.texture_x;

        this->previous = (uint8_t) *i;
        text_width = render_x + char_data.width;
    }

    return text_width;
}

float ekg_font::get_text_height(const std::string &text) {
    return (float) this->texture_height + (0.2 * this->texture_height);
}

void ekg_font::init() {
    this->flag_ft_library_initialised = false;

    if (FT_Init_FreeType(&this->library)) {
        ekgutil::log("Could not init FreeType font library.");
        return;
    }

    this->flag_ft_library_initialised = true;
    this->load(this->font_path);
    this->refresh();
}

bool ekg_font::load(const std::string &f_ont_path) {
    this->font_path = f_ont_path;
    this->flag_font_loaded = false;

    if (!this->flag_ft_library_initialised) {
        return this->flag_font_loaded;
    }

    if (FT_New_Face(this->library, this->font_path.c_str(), 0, &this->face)) {
        ekgutil::log("Could not find file at `" + this->font_path + "`.");
        return false;
    }

    this->flag_font_loaded = true;
    return this->flag_font_loaded;
}

bool ekg_font::reload() {
    this->flag_font_bitmap_generated = false;

    if (!this->flag_ft_library_initialised || !this->flag_font_loaded) {
        return this->flag_font_bitmap_generated;
    }

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    if (this->bitmap_texture_id == 0) {
        glGenTextures(1, &this->bitmap_texture_id);
    }

    glBindTexture(GL_TEXTURE_2D, this->bitmap_texture_id);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, (int32_t) this->texture_width, (int32_t) this->texture_height, 0, GL_ALPHA, GL_UNSIGNED_BYTE, nullptr);
    glGenerateMipmap(GL_TEXTURE_2D);

    float offset = 0.0f;

    for (uint8_t i = 0; i < 128; i++) {
        if (FT_Load_Char(this->face, i, FT_LOAD_RENDER)) {
            continue;
        }

        ekg_char_data &char_data = this->char_list[i];

        char_data.x = float(offset) / (float) this->texture_width;
        char_data.width = (float) this->glyph_slot->bitmap.width;
        char_data.height = (float) this->glyph_slot->bitmap.rows;

        char_data.left = (float) this->glyph_slot->bitmap_left;
        char_data.top = (float) this->glyph_slot->bitmap_top;
        char_data.texture_x = (float) (this->glyph_slot->advance.x >> 6);

        glTexSubImage2D(GL_TEXTURE_2D, 0, (int32_t) offset, 0, (int32_t) char_data.width, (int32_t) char_data.height, GL_ALPHA, GL_UNSIGNED_BYTE, this->glyph_slot->bitmap.buffer);
        offset += char_data.width;
    }

    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glBindTexture(GL_TEXTURE_2D, 0);

    this->flag_font_bitmap_generated = true;
    return this->flag_font_bitmap_generated;
}

void ekg_font::render(const std::string &text, float x, float y, ekgmath::vec4f &color_vec) {
    if (!this->flag_ft_library_initialised || !this->flag_font_loaded || !this->flag_font_bitmap_generated) {
        return;
    }

    const char* char_str = text.c_str();
    const size_t str_len = strlen(char_str);

    float render_x = 0, render_y = 0, render_w = 0, render_h = 0;
    float texture_x = 0, texture_y = 0, texture_w = 0, texture_h = 0;
    float diff = 0.0f;

    // Generate a GPU data.
    ekg_gpu_data gpu_data;

    // Configure
    // Each char quad has 6 vertices, so we multiply 6 by length of text.
    gpu_data.data = (GLint) (6 * str_len);

    gpu_data.pos[0] = x;
    gpu_data.pos[1] = y;

    // Reset because we do not modify the buffer vertex.
    x = 0;
    y = 0;

    for (const char* i = char_str; *i; i++) {
        if (this->use_kerning && this->previous && *i) {
            FT_Get_Kerning(this->face, this->previous, *i, 0, &this->previous_char_vec);
            x += (float) (this->previous_char_vec.x >> 6);
        }

        // Get char from list and update metrics/positions of each char.
        ekg_char_data &char_data = this->char_list[*i];

        render_x = x + char_data.left;
        render_y = y + ((float) this->texture_height - char_data.top);

        render_w = char_data.width;
        render_h = char_data.height;

        texture_x = char_data.x;
        texture_w = render_w / (float) this->texture_width;
        texture_h = render_h / (float) this->texture_height;
        diff += texture_x;

        ekggpu::push_arr_rect(ekg::core::instance.get_gpu_handler().get_cached_vertices(), render_x, render_y, render_w, render_h);
        ekggpu::push_arr_rect(ekg::core::instance.get_gpu_handler().get_cached_vertices_materials(), texture_x, texture_y, texture_w, texture_h);

        x += char_data.texture_x;
        this->previous = (uint8_t) *i;
    }

    // Pass color of texture.
    gpu_data.color[0] = color_vec.x;
    gpu_data.color[1] = color_vec.y;
    gpu_data.color[2] = color_vec.z;
    gpu_data.color[3] = color_vec.w;

    gpu_data.factor = ((float) str_len + diff);

    // Send data to GPU.
    ekg::core::instance.get_gpu_handler().bind_texture(gpu_data, this->bitmap_texture_id);
    ekg::core::instance.get_gpu_handler().bind(gpu_data);
}

void ekg_font::quit() {
    FT_Done_FreeType(this->library);
    FT_Done_Face(this->face);
}

void ekg_font::set_size(uint8_t size) {
    if (!this->flag_font_loaded) {
        return;
    }

    this->font_size = size;
}

void ekg_font::refresh() {
    if (this->font_size == 0) {
        this->set_size(36);
    }

    FT_Set_Pixel_Sizes(this->face, 0, this->font_size);

    this->texture_width = 0;
    this->texture_height = 0;

    this->use_kerning = FT_HAS_KERNING(this->face);
    this->glyph_slot = this->face->glyph;

    for (uint8_t i = 0; i < 128; i++) {
        if (FT_Load_Char(this->face, i, FT_LOAD_RENDER)) {
            continue;
        }

        this->texture_width += (uint32_t) this->glyph_slot->bitmap.width;
        this->texture_height = std::max(this->texture_height, (uint32_t) this->glyph_slot->bitmap.rows);
    }

    this->reload();
}

void ekgfont::render(const std::string &text, float x, float y, ekgmath::vec4f &color_vec) {
    ekg::core::instance.get_font_manager().render(text, x, y, color_vec);
}

float ekgfont::get_text_width(const std::string &text) {
    return ekg::core::instance.get_font_manager().get_text_width(text);
}

float ekgfont::get_text_height(const std::string &text) {
    return ekg::core::instance.get_font_manager().get_text_height(text);
}
