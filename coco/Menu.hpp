#pragma once

#include "font/tahoma_8pt.hpp"
#include "SSD130x.hpp"
#include <coco/Input.hpp>


namespace coco {

class Menu {
public:

	Menu(SSD130x &display) : display(display) {
	}

	/**
		Begin a menu
		@param buttons buttons that control the menu, either d-pad or rotary button
		@return true when the menu should be exited
	*/
	bool begin(Input &buttons);

	/**
		Add a divider line to the menu
	*/
	void line();
	void beginSection();
	void endSection();

	enum class Command {
		UNDERLINE_ON = 1,
		UNDERLINE_OFF = 2,
		INVERT_ON = 3,
		INVERT_OFF = 4,
	};

	class Stream {//} : public ::Stream {
	public:
		int x;
		int y;
		Bitmap bitmap;
		int16_t underlineCount = 0;
		int16_t underlineStart;
		int16_t invertCount = 0;
		int16_t invertStart;


		Stream(int x, int y, const Bitmap &bitmap) : x(x), y(y), bitmap(bitmap) {}

		//~Stream() override {}

		Stream &operator <<(char ch) {// override {
			this->x = this->bitmap.drawText(this->x, this->y, tahoma_8pt, String(&ch, 1));
			return *this;
		}

		Stream &operator <<(String const &str) {//override {
			this->x = this->bitmap.drawText(this->x, this->y, tahoma_8pt, str);
			return *this;
		}

		Stream &operator <<(Command command) {//override {
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
							this->bitmap.hLine(x, this->y + tahoma_8pt.height, this->x - x - 1);
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
							this->bitmap.fillRectangle(x - 1, this->y, this->x - x + 1, tahoma_8pt.height, DrawMode::FLIP);
						}
					}
					break;
			}
			return *this;
		}
	};

	Stream stream();

	/**
		Add a label to the menu that can not be selected
 	*/
	void label();

	/**
		Add a label to the menu that can not be selected
		@param markup text with markup (e.g. underline)
	*/
	template <typename T>
	void label(T markup) {
		Stream s = stream();
		s << markup;
		label();
	}

	/**
		Add a menu entry
	*/
	bool entry();

	/**
		Add a menu entry
		@param markup text with markup (e.g. underline)
	*/
	template <typename T>
	bool entry(T markup) {
		Stream s = stream();
		s << markup;
		return entry();
	}

	/**
		Return the currently selected menu entry
	*/
	int getSelected() const {return this->selected;}

	/**
		Returns true if the current menu entry is selected
	*/
	bool isSelected() const {
		return this->selected == this->entryIndex;
	}

	/**
		Get edit state. Returns 0 if not in edit mode or not the entry being edited, otherwise returns the 1-based index
		of the field being edited
	*/
	int edit(int editCount = 1);
	int delta() const {return this->d;}

	void remove() {--this->selected;}

	/**
		Show the menu on the display
		@return use co_await and select on the return value to wait for redraw and user input, e.g. buttons
	*/
	[[nodiscard]] Awaitable<> show();

protected:

	enum class State {
		INIT,

		// display is ready and can be filled with content
		READY,

		// redraw is needed
		DIRTY,

		// transfer is in progress
		BUSY,
	};

	Bitmap bitmap() {
		// return a dummy bitmap if state is dirty or a transfer is in progress
		if (this->state != State::READY)
			return Bitmap();
		return this->display.bitmap();
	}

	SSD130x &display;
	State state = State::INIT;

	int8_t buttonState[2];

	bool activated = false;
	int d;

	enum class Section : uint8_t {
		BEGIN,
		BODY,
		END
	};
	Section section = Section::END;

	// index of selected menu entry
	int selected = 0;

	// y coordinate of selected menu entry
	int selectedY = 0;

	// starting y coodinate of display
	int offsetY = 0;


	// index of current menu entry
	int entryIndex = 0;

	// y coordinate of current menu entry
	int entryY = 0;

	// index of selected element to edit, 0 when edit mode is off
	int editIndex = 0;


	// number of entries in the menu, captured after the last run
	int entryCount;

	CoroutineTaskList<> dummyTasks;
};


// underline stream token
template <typename A>
struct Underline {
	A const &a;
	bool on;
};

template <typename A>
Menu::Stream &operator <<(Menu::Stream &s, Underline<A> underline) {
	if (underline.on)
		s << Menu::Command::UNDERLINE_ON;
	s << underline.a;
	if (underline.on)
		s << Menu::Command::UNDERLINE_OFF;
	return s;
}

template <typename A>
constexpr Underline<A> underline(A const &a, bool on = true) {
	return {a, on};
}

} // namespace coco
