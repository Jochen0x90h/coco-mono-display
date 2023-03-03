#include <coco/Menu.hpp>
#include <coco/SSD130x.hpp>
#include <coco/font/tahoma_8pt.hpp>
#include <coco/StreamOperators.hpp>
#include <coco/debug.hpp>
#include "MenuTest.hpp"


AwaitableCoroutine menu1(SSD130x &display, Trigger &trigger, QuadratureDecoder &quadratureDecoder) {
	Menu menu(display);
	int delta = 0;
	int value = 0;
	while (true) {
		bool edit = menu.edit(1) == 1;
		if (edit)
			value += delta;

		menu.stream() << "Value: " << underline(dec(value), edit);
		menu.entry();

		if (menu.entry("Exit"))
			co_return;

		delta = 0;
		uint32_t risingFlags = 1;
		uint32_t fallingFlags = 0;
		int s = co_await select(menu.show(), trigger.trigger(risingFlags, fallingFlags), quadratureDecoder.change(delta));
		if (s == 2)
			menu.activate();
		if (s == 3)
			menu.moveSelection(delta);
	}
}

AwaitableCoroutine menu2(SSD130x &display, Trigger &trigger, QuadratureDecoder &quadratureDecoder) {
	Menu menu(display);
	while (true) {
		if (menu.entry("Exit"))
			co_return;

		uint32_t risingFlags = 1;
		uint32_t fallingFlags = 0;
		int delta;
		int s = co_await select(menu.show(), trigger.trigger(risingFlags, fallingFlags), quadratureDecoder.change(delta));
		if (s == 2)
			menu.activate();
		if (s == 3)
			menu.moveSelection(delta);
	}
}

Awaitable<> noWait() {
	return {};
}

Coroutine mainMenu(Loop &loop, SSD130x &display, Trigger &trigger, QuadratureDecoder &quadratureDecoder) {
	// initialize
	Menu menu(display);
	int i = 0;
	while (true) {
		// build menu
		if (menu.entry("Menu1"))
			co_await menu1(display, trigger, quadratureDecoder);
		if (menu.entry("Menu2"))
			co_await menu2(display, trigger, quadratureDecoder);
		if (menu.entry("Menu3"))
			co_await menu2(display, trigger, quadratureDecoder);
		if (menu.entry("Menu4"))
			co_await menu2(display, trigger, quadratureDecoder);
		if (menu.entry("Menu5"))
			co_await menu2(display, trigger, quadratureDecoder);
		if (menu.entry("Menu6"))
			co_await menu2(display, trigger, quadratureDecoder);

		// show on display and wait for input
		if (i < 2) {
			uint32_t risingFlags = 1;
			uint32_t fallingFlags = 0;
			int delta;
			int s = co_await select(menu.show(), trigger.trigger(risingFlags, fallingFlags), quadratureDecoder.change(delta));
			if (s == 2)
				menu.activate();
			if (s == 3)
				menu.moveSelection(delta);
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
	mainMenu(drivers.loop, display, drivers.trigger, drivers.quadratureDecoder);

	drivers.loop.run();
}
