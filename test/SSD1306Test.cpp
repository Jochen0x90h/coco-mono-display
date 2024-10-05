#include "SSD1306Test.hpp"
#include <coco/SSD130x.hpp>
#include <coco/font/tahoma8pt1bpp.hpp>
#include <coco/debug.hpp>


Coroutine draw(Loop &loop, Buffer &buffer) {
	SSD130x display(buffer, DISPLAY_WIDTH, DISPLAY_HEIGHT, DISPLAY_FLAGS);
	co_await drivers.resetDisplay();
	co_await display.init();
	co_await display.enable();

	int x = 0;
	int y = 0;
	while (true) {
		Bitmap bitmap = display.bitmap();
		bitmap.clear();
		bitmap.drawText(0, 0, tahoma8pt1bpp, "Hello World!");
		bitmap.drawText(50, 50, tahoma8pt1bpp, "SSD1306 I2C");
		bitmap.drawRectangle(x, y, 10, 10);
		x = (x + 1) & (DISPLAY_WIDTH - 1);
		y = (y + 1) & (DISPLAY_HEIGHT - 1);

		co_await display.show();
		co_await loop.sleep(200ms);

		debug::toggleRed();
		debug::toggleGreen();
	}
}


int main(void) {
	draw(drivers.loop, drivers.displayBuffer);

	drivers.loop.run();
}
