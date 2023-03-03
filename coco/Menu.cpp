#include "Menu.hpp"


namespace coco {

void Menu::line() {
	int x = 10;
	int y = this->entryY + 2 - this->offsetY;
	bitmap().fillRectangle(x, y, 108, 1);
	this->entryY += 1 + 4;
}

void Menu::beginSection() {
	if (this->section != Section::END)
		this->section = Section::BEGIN;
}

void Menu::endSection() {
	if (this->section == Section::BODY)
		line();
	this->section = Section::END;
}

Menu::Stream Menu::stream() {
	if (this->section == Section::BEGIN)
		line();
	this->section = Section::BODY;

	return {10, this->entryY + 2 - this->offsetY, bitmap()};
}

void Menu::label() {
	this->entryY += tahoma_8pt.height + 4;
}

bool Menu::entry() {
	const int lineHeight = tahoma_8pt.height + 4;
	int y = this->entryY + 2 - this->offsetY;

	bool selected = this->entryIndex == this->selected;
	if (selected) {
		bitmap().drawText(0, y, tahoma_8pt, ">", 0);
		this->selectedY = this->entryY;
	}

	++this->entryIndex;
	this->entryY += lineHeight;

	// check if this menu entry was activated
	bool activated = selected && this->activated;
	if (activated) {
		// yes: force redraw because something may get changed or a sub menu is entered
		this->state = State::DIRTY;
	}

	return activated;
}

void Menu::moveSelection(int delta) {
	// update selected entry according to delta motion of poti when not in edit mode
	if (this->editIndex == 0) {
		int selected = this->selected + delta;
		if (selected < 0) {
			selected = 0;

			// also clear yOffset in case the menu has a non-selectable header
			this->offsetY = 0;
		} else if (selected >= this->entryCount) {
			selected = this->entryCount - 1;
		}
		this->selected = selected;
	}
}

int Menu::edit(int editCount) {
	// check if the next entry is selected
	if (this->selected == this->entryIndex) {
		// cycle edit index if activated (e.g. button pressed)
		if (this->activated) {
			++this->editIndex;

			// "consume" activation
			this->activated = false;
		}
		if (this->editIndex > editCount)
			this->editIndex = 0;
		return this->editIndex;
	}
	return 0;
}

Awaitable<Buffer::State> Menu::show() {
	this->section = Section::END;

	// need redraw if display is "dirty" or still busy with a transfer
	bool redraw = this->state == State::DIRTY || this->display.buffer().busy();

	// adjust yOffset so that selected entry is visible
	const int lineHeight = tahoma_8pt.height + 4;
	int upper = this->selectedY;
	int lower = upper + lineHeight;
	if (upper < this->offsetY) {
		this->offsetY = upper;
		redraw = true;
	}
	if (lower > this->offsetY + this->display.height()) {
		this->offsetY = lower - this->display.height();
		redraw = true;
	}

	// capture number of entries in menu
	this->entryCount = this->entryIndex;

	// clear for next menu drawing
	this->entryIndex = 0;
	this->entryY = 0;
	this->activated = false;


	if (redraw) {
		// redraw when the buffer is ready (by exiting the co_await in the menu loop)
		this->state = State::BUSY;
		return this->display.buffer().untilReady();
	} else {
		// clear if menu is empty
		if (this->state == State::BUSY)
			this->display.bitmap().clear();

		// start transfer of bitmap to display
		this->display.startDisplay();
		this->state = State::BUSY;

		// wait "forever", the caller should also wait for user input (e.g. buttons) using select() in the menu loop
		return this->display.buffer().untilDisabled();
	}
}

} // namespace coco
