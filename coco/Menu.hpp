#pragma once

#include "SSD130x.hpp"
#include <coco/Font.hpp>
#include <coco/InputDevice.hpp>


namespace coco {

class Menu {
public:
	using Stream = BitmapStream;

	Menu(SSD130x &display, const Font &font) : display(display), font(font) {
	}

	/**
		Begin a menu
		@param buttons buttons that control the menu, either d-pad or rotary button
		@return true when the menu should be exited
	*/
	bool begin(InputDevice &buttons);

	/**
		Show the menu on the display. Should be awaited before begin() gets called again
		@return use co_await and select on the return value to wait for redraw and user input, e.g. buttons
	*/
	[[nodiscard]] Awaitable<Buffer::Events> show();

	/**
	 * Wait for user input
	 */
	[[nodiscard]] auto untilInput(InputDevice &buttons) {return buttons.untilInput(this->seq);}


	/**
		Add a divider line to the menu
	*/
	void line();
	void beginSection();
	void endSection();


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


protected:

	SSD130x &display;
	const Font &font;
	bool init = true;

	int seq;
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
		s << Menu::Stream::Command::UNDERLINE_ON;
	s << underline.a;
	if (underline.on)
		s << Menu::Stream::Command::UNDERLINE_OFF;
	return s;
}

template <typename A>
constexpr Underline<A> underline(A const &a, bool on = true) {
	return {a, on};
}

} // namespace coco
