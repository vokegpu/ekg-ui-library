#pragma once
#ifndef EKG_FONT_H
#define EKG_FONT_H

#include "ekg_gpu.h"
#include <f2build.h>
#include FT_FREETYPE_H

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
	FT_Vector_ previou_char_vec;

	float texture_width;
	float texture_height;

	GLuint bitmap_texture_id;
	const std::string &font_path;

	bool flag_font_loaded = false;
	bool flag_font_bitmap_generated = false;
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
	void render(const std::string &text, float x, float y, ekgmath::vec4 &color);
};

/*
 * Make easy the communcation.
 */
namespace ekgfont {
	/*
	 * Render one simple text.
	 */
	void render(const std::string &text, float x, float y, ekgmath::vec4 &color);
};

#endif