#pragma once

#include <coco/String.hpp>
#include <cstdint>


namespace coco {

struct Character {
	//int code;
	
	// character width
	uint8_t width;
	
	// character offset in glyph bitmap data
	uint16_t offset;
};

struct Font {
	static int const TAB_WIDTH = 5;
	
	// common character height
	uint8_t height;

	// space between characters
	uint8_t space;

	// first/last characters in font
	unsigned char first;
	unsigned char last;
	
	// characters supported by the font
	Character const *characters;

	// glyph bitmap data
	uint8_t const *bitmap;
	
	int calcWidth(String text, int space = 1);
};

} // namespace coco
