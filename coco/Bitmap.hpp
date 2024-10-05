#pragma once

#include "DrawMode.hpp"
#include <coco/assert.hpp>
#include <coco/String.hpp>
#include <coco/Font.hpp>


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
		@param mode draw mode
		@return x coordinate of end of text
	*/
	int drawText(int x, int y, const Font &font, String text, DrawMode mode = DrawMode::SET);

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


class BitmapStream {
public:
	// stream command
	enum class Command {
		UNDERLINE_ON = 1,
		UNDERLINE_OFF = 2,
		INVERT_ON = 3,
		INVERT_OFF = 4,
	};


	Bitmap bitmap;
	const Font &font;
	int x;
	int y;
	int16_t underlineCount = 0;
	int16_t underlineStart;
	int16_t invertCount = 0;
	int16_t invertStart;


	BitmapStream(const Bitmap &bitmap, const Font &font, int x, int y) : bitmap(bitmap), font(font), x(x), y(y) {}

	//~Stream() override {}

	BitmapStream &operator <<(char ch) {// override {
		this->x = this->bitmap.drawText(this->x, this->y, this->font, String(&ch, 1));
		return *this;
	}

	BitmapStream &operator <<(String const &str) {//override {
		this->x = this->bitmap.drawText(this->x, this->y, this->font, str);
		return *this;
	}

	BitmapStream &operator <<(Command command) {//override {
		switch (command) {
			case Command::UNDERLINE_ON:
				if (this->underlineCount == 0)
					this->underlineStart = this->x;
				++this->underlineCount;
				break;
			case Command::UNDERLINE_OFF:
				if (this->underlineCount > 0) {
					if (--this->underlineCount == 0) {
						int x = this->underlineStart;
						this->bitmap.hLine(x, this->y + this->font.height, this->x - x - 1);
					}
				}
				break;
			case Command::INVERT_ON:
				if (this->invertCount == 0)
					this->invertStart = this->x;
				++this->invertCount;
				break;
			case Command::INVERT_OFF:
				if (this->invertCount > 0) {
					if (--this->invertCount == 0) {
						int x = this->invertStart;
						this->bitmap.fillRectangle(x - 1, this->y, this->x - x + 1, this->font.height, DrawMode::FLIP);
					}
				}
				break;
		}
		return *this;
	}
};

} // namespace coco
