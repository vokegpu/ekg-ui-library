#pragma once

#include <ft2build.h>
#include FT_FREETYPE_H

#ifndef EKG_FONT_H
#define EKG_FONT_H

#include "ekg_gpu.hpp"

/**
 * Store char data (position, metrics and reference).
 **/
struct ekg_char_data {
    float x = 0.0f;
    float texture_x = 0.0f;

    float width = 0.0f;
    float height = 0.0f;

    float top = 0.0f;
    float left = 0.0f;
};

/**
 * Font manager to configure, load, display/draw/render chars.
 **/
class ekg_font {
protected:
	FT_Library library;
	FT_Face face;
	FT_GlyphSlot glyph_slot;
	FT_Bool use_kerning;
	FT_UInt previous;
	FT_Vector_ previous_char_vec;

	uint32_t texture_width;
    uint32_t texture_height;

	GLuint bitmap_texture_id;
	std::string font_path;

	bool flag_font_loaded = false;
	bool flag_font_bitmap_generated = false;
    bool flag_ft_library_initialised = false;

    ekg_char_data char_list[256];
public:
	/*
	 * Get text width from font metrics.
	 */
	float get_text_width(const std::string &text);

	/*
	 * Get text height from font metrics.
	 */
	float get_text_height(const std::string &text);

	/*
	 * Prepare the EKG font manager.
	 */
	void init();

    /*
     * Free FT library.
     */
    void quit();

	/*
	 *
	 */
	bool load(const std::string &f_ont_path);

	/*
	 *  Free and load a new font bitmap.
	 */
	bool reload();

	/*
	 * Send data coordinates data to GPU.
	 */
	void render(const std::string &text, float x, float y, ekgmath::vec4f &color);
};

/*
 * Make easy the communication.
 */
namespace ekgfont {
	/*
	 * Render one simple text.
	 */
	void render(const std::string &text, float x, float y, ekgmath::vec4f &color);
};

#endif