#include <coco/Menu.hpp>
#include <coco/SSD130x.hpp>
#include <coco/font/tahoma_8pt.hpp>
#include <coco/StreamOperators.hpp>
#include <coco/debug.hpp>
#include "MenuTest.hpp"


AwaitableCoroutine menu1(SSD130x &display, Input &buttons) {
	Menu menu(display);
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

		co_await select(menu.show(), buttons.stateChange());
	}
}

AwaitableCoroutine menu2(SSD130x &display, Input &buttons) {
	Menu menu(display);
	while (true) {
		if (menu.begin(buttons))
			break;

		if (menu.entry("Exit"))
			break;

		co_await select(menu.show(), buttons.stateChange());
	}
}

Awaitable<> noWait() {
	return {};
}

Coroutine mainMenu(Loop &loop, SSD130x &display, Input &buttons) {
	// wait for reset circuit (APX803) on the display board
	co_await loop.sleep(300ms);

	// initialize and enable the display
	co_await display.init();
	co_await display.enable();

	// menu
	Menu menu(display);
	int i = 0;
	while (true) {
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

		// show on display and wait for input
		if (i < 2) {
			co_await select(menu.show(), buttons.stateChange());
			++i;
		} else {
			// simulate interruption by a user input
			co_await select(menu.show(), noWait());
			debug::toggleBlue();
			i = 0;
		}
	}
}


int main(void) {
	debug::init();
	Drivers drivers;

	SSD130x display(drivers.buffer, DISPLAY_WIDTH, DISPLAY_HEIGHT, DISPLAY_FLAGS);
	mainMenu(drivers.loop, display, drivers.buttons);

	drivers.loop.run();
}
