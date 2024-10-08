#include "Bitmap.hpp"


namespace coco {

void Bitmap::fillRectangle(int x, int y, int width, int height, DrawMode mode) {
	if (mode == DrawMode::KEEP)
		return;

	uint8_t *data = this->data;
	int w = this->width;
	int h = this->height;

	// clamp to border
	if (x < 0) {
		width += x;
		x = 0;
	}
	if (y < 0) {
		height += y;
		y = 0;
	}
	if (x + width > w) {
		width = w - x;
	}
	if (y + height > h) {
		height = h - y;
	}
	if (width <= 0 || height <= 0)
		return;

	int endRows = (y + height) & 7;
	uint8_t *page = &data[(y >> 3) * w] + x;
	uint8_t *pageEnd = &data[((y + height) >> 3) * w] + x;

	// first page
	{
		uint8_t mask = 0xff << (y & 7);
		if (page == pageEnd && endRows > 0) {
			mask &= 0xff >> (8 - endRows);
			endRows = 0;
		}
		for (int i = 0; i < width; ++i) {
			switch (mode) {
			case DrawMode::CLEAR:
				page[i] &= ~mask;
				break;
			case DrawMode::FLIP:
				page[i] ^= mask;
				break;
			default:
				page[i] |= mask;
				break;
			}
		}
		page += w;
	}

	// full pages
	while (page < pageEnd) {
		for (int i = 0; i < width; ++i) {
			switch (mode) {
			case DrawMode::CLEAR:
				page[i] = 0;
				break;
			case DrawMode::FLIP:
				page[i] ^= 0xff;
				break;
			default:
				page[i] = 0xff;
				break;
			}
		}
		page += w;
	}

	// last page
	if (endRows > 0) {
		uint8_t mask = 0xff >> (8 - endRows);
		for (int i = 0; i < width; ++i) {
			switch (mode) {
			case DrawMode::CLEAR:
				page[i] &= ~mask;
				break;
			case DrawMode::FLIP:
				page[i] ^= mask;
				break;
			default:
				page[i] |= mask;
				break;
			}
		}
	}
}

void Bitmap::copyBitmapH(int x, int y, int width, int height, const uint8_t *bitmap,
	DrawMode mode)
{
	if (mode == DrawMode::KEEP)
		return;

	uint8_t *data = this->data;
	int w = this->width;
	int h = this->height;

	// number of bytes in a row of the bitmap
	int bitmapStride = (width + 7) >> 3;

	// clamp to border
	int o = 0;
	if (x < 0) {
		o = -x;
		width += x;
		x = 0;
	}
	if (y < 0) {
		bitmap += -y * bitmapStride;
		height += y;
		y = 0;
	}
	if (x + width > w) {
		width = w - x;
	}
	if (y + height > h) {
		height = h - y;
	}

	// bitmap
	for (int j = 0; j < height; ++j) {
		uint8_t *page = &data[((y + j) >> 3) * w] + x;

		for (int i = 0; i < width; ++i) {
			uint8_t bit = 1 << ((y + j) & 7);
			if (bitmap[(o + i) >> 3] & (0x80 >> ((o + i) & 7))) {
				switch (mode/* & DrawMode::FORE_MASK*/) {
				case DrawMode::CLEAR:
					page[i] &= ~bit;
					break;
				case DrawMode::FLIP:
					page[i] ^= bit;
					break;
				case DrawMode::SET:
					page[i] |= bit;
				default:
					break;
				}
			} else {
				/*switch (mode & DrawMode::BACK_MASK) {
				case DrawMode::BACK_CLEAR:
					page[i] &= ~bit;
					break;
				case DrawMode::BACK_FLIP:
					page[i] ^= bit;
					break;
				case DrawMode::BACK_SET:
					page[i] |= bit;
				default:
					break;
				}*/
			}
		}
		bitmap += bitmapStride;
	}
}

int Bitmap::drawText(int x, int y, const Font &font, String text, DrawMode mode) {
	for (auto info : font.glyphRange(text)) {
		if (!info.printable()) {
			// special character
			x += info.width() + font.gapWidth;
		} else {
			auto glyph = info.glyph();

			// draw character
			copyBitmapH(x, y + glyph.y, glyph.size.x, glyph.size.y, font.data + glyph.offset, mode);

			// add character width
			x += glyph.size.x;

			// add gap between characters
			x += font.gapWidth;
		}
	}
	return x;
}

} // namespace coco
