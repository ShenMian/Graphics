// Copyright 2021 ShenMian
// License(Apache-2.0)

#include "Font.h"
#include <stdexcept>

#include <ft2build.h>
#include FT_FREETYPE_H

namespace
{

FT_Library library;

}

Font::Font(const std::filesystem::path& path)
{
	loadFromFile(path);
}

Font::~Font()
{
	FT_Done_Face(handle);
}

void Font::loadFromFile(const std::filesystem::path& path)
{
	if(FT_New_Face(library, path.string().c_str(), 0, &handle))
		throw std::runtime_error("failed to load font");
}

Font::Glyph Font::getGlyph(uint32_t code, uint16_t size)
{
	setPixelSize(size);
	if(FT_Load_Char(handle, code, FT_LOAD_RENDER))
		throw std::runtime_error("failed to load glyph");

	uint64_t key = code;
	if(auto it = glyphs.find(key); it != glyphs.end())
		return it->second;

	const auto& bitmap = handle->glyph->bitmap;
	Glyph glyph;
	glyph.image = Image(bitmap.buffer, bitmap.pitch * bitmap.rows, {(int)bitmap.width, (int)bitmap.rows}, 1);
	glyph.advance = {(int)handle->glyph->advance.x, (int)handle->glyph->advance.y};
	glyphs.emplace(key, glyph);

	return glyph;
}

void Font::init()
{
	if(FT_Init_FreeType(&library))
		throw std::runtime_error("FreeType init failed");
}

void Font::deinit()
{
	FT_Done_FreeType(library);
}

void Font::setPixelSize(uint16_t size)
{
	auto res = FT_Set_Pixel_Sizes(handle, 0, size);
	if(res == FT_Err_Invalid_Pixel_Size && !FT_IS_SCALABLE(handle))
		throw std::runtime_error("font are not scalable, can only be set to fixed sizes");
	if(res)
		throw std::runtime_error("failed to set pixel size");
}

uint16_t Font::getPixelSize() const
{
	return handle->size->metrics.x_ppem;
}
