#include <coco/Menu.hpp>
#include <coco/SSD130x.hpp>
#include <coco/font/tahoma8pt1bpp.hpp>
#include <coco/StreamOperators.hpp>
#include <coco/debug.hpp>
#include "MenuTest.hpp"
#include <algorithm>


AwaitableCoroutine menu1(SSD130x &display, InputDevice &buttons) {
	Menu menu(display, coco::tahoma8pt1bpp);
	int value = 0;
	while (true) {
		if (menu.begin(buttons))
			break;

		bool edit = menu.edit(1) == 1;
		if (edit)
			value += menu.delta();

		menu.stream() << "Value: " << underline(dec(value), edit);
		menu.entry();

		if (menu.entry("Exit"))
			break;

		// show menu on display and wait for new input
		co_await menu.show();
		co_await menu.untilInput(buttons);
	}
}

AwaitableCoroutine menu2(SSD130x &display, InputDevice &buttons) {
	Menu menu(display, coco::tahoma8pt1bpp);
	while (true) {
		if (menu.begin(buttons))
			break;

		if (menu.entry("Exit"))
			break;

		// show menu on display and wait for new input
		co_await menu.show();
		co_await menu.untilInput(buttons);
	}
}

Awaitable<> noWait() {
	return {};
}

Coroutine mainMenu(Loop &loop, SSD130x &display, InputDevice &buttons) {
	// initialize and enable the display
	co_await drivers.resetDisplay();
	co_await display.init();
	co_await display.enable();

	// menu
	Menu menu(display, coco::tahoma8pt1bpp);
	int i = 0;
	while (true) {
		// indicate when a redraw occurs
		debug::toggleGreen();

		// build menu
		menu.begin(buttons);
		if (menu.entry("Menu1"))
			co_await menu1(display, buttons);
		if (menu.entry("Menu2"))
			co_await menu2(display, buttons);
		if (menu.entry("Menu3"))
			co_await menu2(display, buttons);
		if (menu.entry("Menu4"))
			co_await menu2(display, buttons);
		if (menu.entry("Menu5"))
			co_await menu2(display, buttons);
		if (menu.entry("Menu6"))
			co_await menu2(display, buttons);

		// show menu on display and wait for new input
		co_await menu.show();
		co_await menu.untilInput(buttons);
	}
}


int main(void) {
	SSD130x display(drivers.buffer, DISPLAY_WIDTH, DISPLAY_HEIGHT, DISPLAY_FLAGS);
	mainMenu(drivers.loop, display, drivers.buttons);

	drivers.loop.run();
}
