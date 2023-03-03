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

int main(int argc, char **argv) {
	testing::InitGoogleTest(&argc, argv);
	int success = RUN_ALL_TESTS();
	return success;
}
