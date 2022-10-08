#include "ekg/draw/font.hpp"
#include "ekg/util/env.hpp"

FT_Library ekg::draw::font_renderer::ft_library {};

float ekg::draw::font_renderer::get_text_width(const std::string &text) {
	FT_Vector ft_vec {};
	this->ft_uint_previous = 0;

	float start_x {};
	float render_x {};
	float text_width {};

	for (const char* chars = text.c_str(); *chars; chars++) {
		if (this->ft_bool_kerning && this->ft_uint_previous && *chars) {
			FT_Get_Kerning(this->ft_face, this->ft_uint_previous, *chars, 0, &ft_vec);
			start_x += static_cast<float>(ft_vec.x >> 6);
		}

		ekg::char_data &char_data = this->allocated_char_data[*chars];

		render_x = start_x + char_data.left;
		start_x += char_data.texture_x;

		this->ft_uint_previous = *chars;
		text_width = render_x + char_data.w;
	}

    return text_width;
}

float ekg::draw::font_renderer::get_text_height() {
    return this->text_height;
}

void ekg::draw::font_renderer::set_font(const std::string &path) {
	if (this->font_path != path) {
		this->font_path = path;

        ekg::log("trying to load new font... " + path);
		this->reload();
	}
}

void ekg::draw::font_renderer::set_size(uint8_t size) {
	if (this->font_size != size) {
		this->font_size = size;
		this->reload();
	}
}

void ekg::draw::font_renderer::reload() {
	if (!this->flag_first_time) {
		FT_Done_Face(this->ft_face);
	}

    ekg::log("trying to create font face... " + this->font_path);

	this->flag_unloaded = FT_New_Face(ekg::draw::font_renderer::ft_library, this->font_path.c_str(), 0, &this->ft_face);
	this->flag_first_time = false;

	if (this->flag_unloaded) {
		return;
	}

	FT_Set_Pixel_Sizes(this->ft_face, 0, this->font_size);

	/* Phase of getting bitmap texture bounds. */
	this->full_width = 0;
	this->full_height = 0;

	this->ft_bool_kerning = FT_HAS_KERNING(this->ft_face);
	this->ft_glyph_slot = this->ft_face->glyph;

	for (uint8_t char_codes = 0; char_codes < 128; char_codes++) {
		if (FT_Load_Char(this->ft_face, char_codes, FT_LOAD_RENDER)) {
			continue;
		}

		this->full_width += this->ft_glyph_slot->bitmap.width;
		this->full_height = std::max(this->full_height, static_cast<float>(this->ft_glyph_slot->bitmap.rows));
	}
	
	this->text_height = static_cast<float>(this->full_height);
	this->offset_text_height = (this->text_height / 8) / 2;
	this->text_height += this->text_height;

	/* Phase of getting chars metrics. */
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	if (this->texture != 0) {
		glDeleteTextures(1, &this->texture);
	}

	glGenTextures(1, &this->texture);
	glBindTexture(GL_TEXTURE_2D, this->texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, (GLint) this->full_width, (GLint) this->full_height, 0, GL_RED, GL_UNSIGNED_BYTE, nullptr);

	float offset {};

	for (uint8_t char_codes = 0; char_codes < 128; char_codes++) {
		if (FT_Load_Char(this->ft_face, char_codes, FT_LOAD_RENDER)) {
			continue;
		}

		ekg::char_data &char_data = this->allocated_char_data[char_codes];

		char_data.x = offset / static_cast<float>(this->full_width);
		char_data.w = static_cast<float>(this->ft_glyph_slot->bitmap.width);
		char_data.h = static_cast<float>(this->ft_glyph_slot->bitmap.rows);

		char_data.left = static_cast<float>(this->ft_glyph_slot->bitmap_left);
		char_data.top = static_cast<float>(this->ft_glyph_slot->bitmap_top);
		char_data.texture_x = static_cast<float>(this->ft_glyph_slot->advance.x >> 6);

		glTexSubImage2D(GL_TEXTURE_2D, 0, static_cast<GLint>(offset), 0, static_cast<GLsizei>(char_data.w), static_cast<GLsizei>(char_data.h), GL_RED, GL_UNSIGNED_BYTE, this->ft_glyph_slot->bitmap.buffer);
		offset += char_data.w;
	}

	GLint swizzle_format[] {GL_ZERO, GL_ZERO, GL_ZERO, GL_RED};

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_RGBA, swizzle_format);
    glBindTexture(GL_TEXTURE_2D, 0);

	ekg::log("font " + this->font_path + " successfully loaded");
}

void ekg::draw::font_renderer::bind_allocator(ekg::gpu::allocator *gpu_allocator) {
	this->allocator = gpu_allocator;
}

void ekg::draw::font_renderer::blit(std::string_view text, float x, float y, const ekg::vec4 &color) {
    if (this->allocator == nullptr || this->flag_unloaded) {
        return;
    }

    x = static_cast<float>(static_cast<int32_t>(x));
    y = static_cast<float>(static_cast<int32_t>(y - this->offset_text_height));

    ekg::gpu::data &data = this->allocator->bind_current_data();

    data.rect_area[0] = x;
    data.rect_area[1] = y;
    data.rect_area[2] = 0;
    data.rect_area[3] = 0;

    data.colored_area[0] = color.x / 255;
    data.colored_area[1] = color.y / 255;
    data.colored_area[2] = color.z / 255;
    data.colored_area[3] = color.w / 255;

    ekg::rect vertices {};
    ekg::rect coordinates {};

    x = 0;
    y = 0;

    data.factor = 1;

    for (const char &chars : text) {
    	if (this->ft_bool_kerning && this->ft_uint_previous && chars) {
    		FT_Get_Kerning(this->ft_face, this->ft_uint_previous, chars, 0, &this->ft_vector_previous_char);
    		x += static_cast<float>(this->ft_vector_previous_char.x >> 6);
    	}

    	ekg::char_data &char_data = this->allocated_char_data[chars];

        vertices.x = static_cast<float>(x + char_data.left);
        vertices.y = y + static_cast<float>(this->full_height) - char_data.top;

        vertices.w = char_data.w;
        vertices.h = char_data.h;

        coordinates.x = char_data.x;
        coordinates.w = vertices.w / static_cast<float>(this->full_width);
        coordinates.h = vertices.h / static_cast<float>(this->full_height);

    	this->allocator->vertex2f(vertices.x, vertices.y);
    	this->allocator->vertex2f(vertices.x, vertices.y + vertices.h);
    	this->allocator->vertex2f(vertices.x + vertices.w, vertices.y + vertices.h);
    	this->allocator->vertex2f(vertices.x + vertices.w, vertices.y + vertices.h);
    	this->allocator->vertex2f(vertices.x + vertices.w, vertices.y);
    	this->allocator->vertex2f(vertices.x, vertices.y);

    	this->allocator->coord2f(coordinates.x, coordinates.y);
    	this->allocator->coord2f(coordinates.x, coordinates.y + coordinates.h);
    	this->allocator->coord2f(coordinates.x + coordinates.w, coordinates.y + coordinates.h);
    	this->allocator->coord2f(coordinates.x + coordinates.w, coordinates.y + coordinates.h);
    	this->allocator->coord2f(coordinates.x + coordinates.w, coordinates.y);
    	this->allocator->coord2f(coordinates.x, coordinates.y);

    	x += char_data.texture_x;
    	this->ft_uint_previous = chars;
        data.factor += x + chars;
    }

    this->allocator->bind_texture(this->texture);
    this->allocator->dispatch();
}

void ekg::draw::font_renderer::init() {

}

void ekg::draw::font_renderer::quit() {
	FT_Done_Face(this->ft_face);
}
