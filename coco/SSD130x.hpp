#pragma once

#include "Bitmap.hpp"
#include <coco/Coroutine.hpp>
#include <coco/Buffer.hpp>


namespace coco {

/**
	Display controller for SSD1306 and SSD1309
	Also see https://github.com/adafruit/Adafruit_SSD1306

	A display of resulution 128x64 is organized as 128 colums and 8 pages where each byte contains 8 pixels of a column
	(64 rows = 8 pages * 8 bytes)
*/
class SSD130x {
public:
	// flags
	enum class Flags {
		NONE = 0,

		// display is connected via I2C
		I2C = 1,

		// external VCC for SSD1306, SSD1309 does not have this option
		EXTERNAL_VCC = 2,
	};

	static constexpr int bufferSize(int width, int height) {return width * ((height + 7) >> 3);}

	static int bufferSize2(int width, int height, bool i2c) {
		int wh = width * ((height + 7) >> 3);
		return (i2c ? 1 : 0) + wh;
	}

	// addressing mode (2 bytes)
	static constexpr int ADDRESSING_MODE = 0x20;
	static constexpr int ADDRESSING_MODE_HORIZONTAL = 0;
	static constexpr int ADDRESSING_MODE_VERTICAL = 1;
	static constexpr int ADDRESSING_MODE_PAGE = 2;

	// column start/end address (3 bytes)
	static constexpr int COLUMN_ADDRESS = 0x21;

	// page start/end address (3 bytes)
	static constexpr int PAGE_ADDRESS = 0x22;

	// start line (1 byte, START_LINE + line)
	static constexpr int START_LINE = 0x40;

	// set contrast (2 bytes)
	static constexpr int CONTRAST = 0x81;

	//
	static constexpr int CHARGEPUMP = 0x8D;

	static constexpr int SEGMENT_REMAP0 = 0xA0;
	static constexpr int SEGMENT_REMAP1 = 0xA1;

	static constexpr int ALL_ON_DISABLE = 0xA4;
	static constexpr int ALL_ON_ENABLE = 0xA5;

	static constexpr int INVERT_DISABLE = 0xA6;
	static constexpr int INVERT_ENABLE = 0xA7;

	static constexpr int MULTIPLEX = 0xA8;

	static constexpr int DISPLAY_OFF = 0xAE;
	static constexpr int DISPLAY_ON = 0xAF;

	static constexpr int COM_SCAN_INC = 0xC0;
	static constexpr int COM_SCAN_DEC = 0xC8;

	// display offset (2 bytes)
	static constexpr int DISPLAY_OFFSET = 0xD3;

	// clock divide ratio / oscillator frequency (2 bytes)
	static constexpr int CLOCK_DIV = 0xD5;

	static constexpr int PRECHARGE_PERIOD = 0xD9;

	// com pins hardware configuration (2 bytes)
	static constexpr int COM_PINS_CONFIG = 0xDA;

	static constexpr int VCOM_DETECT = 0xDB;


	SSD130x(Buffer &buffer, int width, int height, Flags flags = Flags::NONE)
		: buf(buffer), w(width), h(height), flags(flags) {}

	int width() {return this->w;}
	int height() {return this->h;}

	/**
		Initializate the display
		@return use co_await on return value to await end of initialization
	*/
	[[nodiscard]] AwaitableCoroutine init();

	/**
		Power up Vcc (if supported) and enable the display
		@return use co_await on return value to await end of operation
	*/
	[[nodiscard]] AwaitableCoroutine enable();

	/**
		Disable the display and power down Vcc (if supported)
		@return use co_await on return value to await end of operation
	*/
	[[nodiscard]] AwaitableCoroutine disable();

	/**
		Obtains the underlying buffer
	*/
	Buffer &buffer() {return this->buf;}

	/**
		Set contrast of display
		@return use co_await on return value to await end of operation
	*/
	[[nodiscard]] AwaitableCoroutine setContrast(uint8_t contrast);

	/**
		Obtain the display bitmap to draw something onto it
	*/
	Bitmap bitmap() {
		return {this->buf.data(), this->w, this->h};
	}

	/**
		Display the current contents of the bufferSet content of whole display
		@return use co_await on return value to await end of operation
	*/
	[[nodiscard]] Awaitable<> display();

	void startDisplay();

protected:
	Buffer &buf;
	int w;
	int h;
	Flags flags;
};
COCO_ENUM(SSD130x::Flags);

} // namespace coco
