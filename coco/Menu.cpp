#include "Menu.hpp"


namespace coco {

bool Menu::begin(InputDevice &buttons) {
	if (this->init) {
		this->init = false;
		this->seq = buttons.get(this->buttonState);
		this->d = 0;
	} else {
		// get button state
		int8_t buttonState[2];
		this->seq = buttons.get(buttonState);

		// detect change of up/down button or rotary knob
		int delta = buttonState[0] - this->buttonState[0];
		if (delta != 0) {
			this->buttonState[0] = buttonState[0];

			// move selection
			if (this->editIndex == 0) {
				int selected = this->selected + delta; // up decrements selected entry
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

		// set delta for edit mode
		this->d = delta;

		// detect change of right/left button or button of rotary button
		int x = buttonState[1] - this->buttonState[1];
		if (x != 0) {
			this->buttonState[1] = buttonState[1];
			if (x > 0) {
				// activate
				this->activated = true;
			} else {
				// exit edit or menu
				if (this->editIndex > 0)
					--this->editIndex;
				else
					return true;
			}
		}
	}

	// clear bitmap
	this->display.bitmap().clear();
	return false;
}

Awaitable<Buffer::Events> Menu::show() {
	this->section = Section::END;

	// need redraw if an entry was selected
	bool redraw = this->init;

	// adjust yOffset so that selected entry is visible
	const int lineHeight = this->font.height + 4;
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
		// prevent waiting for new input
		--this->seq;

		// don't draw and continue
		return {};
	} else {
		// show bitmap on display (user needs to co_await before calling begin() again)
		return this->display.show();
	}
}


void Menu::line() {
	int x = 10;
	int y = this->entryY + 2 - this->offsetY;
	this->display.bitmap().fillRectangle(x, y, 108, 1);
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

	return {this->display.bitmap(), this->font, 10, this->entryY + 2 - this->offsetY};
}

void Menu::label() {
	this->entryY += this->font.height + 4;
}

bool Menu::entry() {
	const int lineHeight = this->font.height + 4;
	int y = this->entryY + 2 - this->offsetY;

	bool selected = this->entryIndex == this->selected;
	if (selected) {
		if (this->editIndex == 0)
			this->display.bitmap().drawText(0, y, this->font, ">");
		this->selectedY = this->entryY;
	}

	++this->entryIndex;
	this->entryY += lineHeight;

	// check if this menu entry was activated
	bool activated = selected && this->activated;
	if (activated) {
		// yes: force redraw because something may get changed or a sub menu is entered
		this->init = true;
	}

	return activated;
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

} // namespace coco
