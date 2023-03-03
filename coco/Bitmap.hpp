#pragma once

#include <coco/assert.hpp>
#include <coco/String.hpp>
#include "Font.hpp"
#include "DrawMode.hpp"


namespace coco {

/**
	Bitmap with line and text drawing functions
*/
class Bitmap {
public:
	static constexpr int size(int width, int height) {return width * (height + 7) >> 3;}

	Bitmap() : data(nullptr), width(0), height(0) {}
	Bitmap(uint8_t *data, int width, int height) : data(data), width(width), height(height) {}

	void clear(int value = 0) {
		int size = this->width * this->height >> 3;
		for (int i = 0; i < size; ++i)
			this->data[i] = value;
	}

	void fillRectangle(int x, int y, int width, int height, DrawMode mode = DrawMode::SET);

	void drawRectangle(int x, int y, int width, int height, DrawMode mode = DrawMode::SET) {
		fillRectangle(x, y, width, 1, mode);
		fillRectangle(x, y + height - 1, width, 1, mode);
		fillRectangle(x, y + 1, 1, height - 2, mode);
		fillRectangle(x + width - 1, y + 1, 1, height - 2, mode);
	}

	void hLine(int x, int y, int length, DrawMode mode = DrawMode::SET) {
		fillRectangle(x, y, length, 1, mode);
	}

	void copyBitmapH(int x, int y, int width, int height, const uint8_t *bitmap, DrawMode mode);


	/**
		Draw text onto bitmap
		@param x X-coordinate of upper left point of the first character
		@param y Y-coordinate of upper left point of the first character
		@param font font to use
		@param text text to draw
		@param space space between characters
		@param mode draw mode
		@return x coordinate of end of text
	*/
	int drawText(int x, int y, const Font &font, String text, int space = 1,
		DrawMode mode = DrawMode::SET /*| DrawMode::BACK_KEEP*/)
	{
		for (unsigned char ch : text) {
			if (ch == '\t') {
				x += Font::TAB_WIDTH;
			} else {
				if (ch < font.first || ch > font.last)
					ch = '?';

				// draw character
				const Character &character = font.characters[ch - font.first];
				copyBitmapH(x, y, character.width, font.height, font.bitmap + character.offset, mode);
				x += character.width;

				// draw space
				//fillBitmap(W, H, this->data, x, y, space, font.height, DrawMode(uint8_t(mode) >> 2));
				x += space;
			}
		}
		return x;
	}

	/**
		Copy entire bitmap
		@param bitmap source bitmap
	*/
	void copy(const Bitmap &bitmap) {
		assert(bitmap.width == this->width && bitmap.height == this->height);
		std::copy(bitmap.data, bitmap.data + bitmap.width * bitmap.height, this->data);
	}

	///
	/// bitmap data, data layout: rows of 8 pixels where each byte describes a column in each row
	/// this would be the layout of a 16x16 display where each '|' is one byte
	/// ||||||||||||||||
	/// ||||||||||||||||
	uint8_t *data;
	int width;
	int height;
};

} // namespace coco
