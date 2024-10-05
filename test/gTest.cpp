#include <gtest/gtest.h>
#include <coco/Bitmap.hpp>
#include <coco/SSD130x.hpp>


using namespace coco;


// Bitmap
// ------

TEST(cocoTest, Bitmap) {
	uint8_t buffer[SSD130x::bufferSize(128, 64)];
	Bitmap bitmap(buffer, 128, 64);

	bitmap.clear();
	EXPECT_EQ(buffer[0], 0);
	EXPECT_EQ(buffer[1], 0);
	EXPECT_EQ(buffer[2], 0);

	bitmap.fillRectangle(0, 0, 2, 2);
	EXPECT_EQ(buffer[0], 3);
	EXPECT_EQ(buffer[1], 3);
	EXPECT_EQ(buffer[2], 0);
}


// test code for glyph lookup
// --------------------------

struct TestGlyph {
	String code;
};

bool operator <(const String &s, const TestGlyph &g) {
	return s < g.code;
}

const TestGlyph glyphs[] = {
	{"a"},
	{"b"},
	{"c"},
	{"f"},
	{"ft"},
	{"g"},
	//{"ä"},
	{"ö"},
	//{"ü"},
};

void drawText(String text) {
	while (text.size() > 0) {
		auto begin = std::begin(glyphs) + 1;
		auto glyph = std::upper_bound(begin, std::end(glyphs), text);
		--glyph;

		if (text.startsWith(glyph->code)) {
			text = text.substring(glyph->code.size());
		} else {
			// unknown character

			// remove first utf-8 character
			text = Font::removeFirst(text);
		}
	}
}

TEST(cocoTest, drawText) {
	drawText("aftgäü");
}


int main(int argc, char **argv) {
	testing::InitGoogleTest(&argc, argv);
	int success = RUN_ALL_TESTS();
	return success;
}
