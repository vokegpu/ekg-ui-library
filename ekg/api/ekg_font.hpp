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
    float x = 0;
    float texture_x = 0;

    float width = 0;
    float height = 0;

    float top = 0;
    float left = 0;
};

/**
 * Store results of render box.
 **/
struct ekg_render_box_result {
	uint32_t colws_reach;
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

	uint32_t texture_width = 0;
    uint32_t texture_height = 0;

	GLuint bitmap_texture_id;
	std::string font_path;

	bool flag_font_loaded = false;
	bool flag_font_bitmap_generated = false;
    bool flag_ft_library_initialised = false;

    ekg_char_data char_list[256];
    uint8_t font_size = 16;
public:
    /*
     * Set size of font.
     */
    void set_size(uint8_t size);

    /*
     * Refresh font.
     */
    void refresh();

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
	 * Load font.
	 */
	bool load(const std::string &f_ont_path);

	/*
	 *  Free and load a new font bitmap.
	 */
	bool reload();

	/*
	 * Send data coordinates data to GPU.
	 */
	void render(const std::string &text, float x, float y, ekgmath::vec4f &color_vec);
};

/*
 * Make easy the communication.
 */
namespace ekgfont {
    /*
     * Get the width size of text displayed.
     */
    float get_text_width(const std::string &text);

    /*
     * Get the height size of text displayed.
     */
    float get_text_height(const std::string &text);

	/*
	 * Render one simple text.
	 */
	void render(const std::string &text, float x, float y, ekgmath::vec4f &color_vec);
};

#endif