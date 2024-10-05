#include "SSD130x.hpp"
#include <coco/bits.hpp>


namespace coco {

// addressing mode (2 bytes)
constexpr int ADDRESSING_MODE = 0x20;
constexpr int ADDRESSING_MODE_HORIZONTAL = 0;
constexpr int ADDRESSING_MODE_VERTICAL = 1;
constexpr int ADDRESSING_MODE_PAGE = 2;

// column start/end address (3 bytes)
constexpr int COLUMN_ADDRESS = 0x21;

// page start/end address (3 bytes)
constexpr int PAGE_ADDRESS = 0x22;

// start line (1 byte, START_LINE + line)
constexpr int START_LINE = 0x40;

// set contrast (2 bytes)
constexpr int CONTRAST = 0x81;

//
constexpr int CHARGEPUMP = 0x8D;

constexpr int SEGMENT_REMAP0 = 0xA0; // column address 0 mapped to SEG0
constexpr int SEGMENT_REMAP1 = 0xA1; // column address 127 mapped to SEG0

constexpr int ALL_ON_DISABLE = 0xA4;
constexpr int ALL_ON_ENABLE = 0xA5;

constexpr int INVERT_DISABLE = 0xA6;
constexpr int INVERT_ENABLE = 0xA7;

constexpr int MULTIPLEX = 0xA8;

constexpr int DISPLAY_OFF = 0xAE;
constexpr int DISPLAY_ON = 0xAF;

constexpr int COM_SCAN_INC = 0xC0; // scan from COM0 to 63
constexpr int COM_SCAN_DEC = 0xC8; // scan from COM63 to 0

// display offset (2 bytes)
constexpr int DISPLAY_OFFSET = 0xD3;

// clock divide ratio / oscillator frequency (2 bytes)
constexpr int CLOCK_DIV = 0xD5;

constexpr int PRECHARGE_PERIOD = 0xD9;

// com pins hardware configuration (2 bytes)
constexpr int COM_PINS_CONFIG = 0xDA;

constexpr int VCOMH_DESELECT = 0xDB;

constexpr int COMMAND_LOCK = 0xFD;


SSD130x::SSD130x(Buffer &buffer, int width, int height, Flags flags)
	: buf(buffer), w(width), h(height), flags(flags)
{
	buffer.headerResize((flags & Flags::I2C) != 0 ? 1 : 0);
}

/*AwaitableCoroutine SSD130x::reset(Loop &loop, OutputPins &resetOutput, int resetPin) {
	// set reset pin
	resetOutput.set(resetPin, resetPin);

	// Adafruit bonnet: Wait 300ms for reset circuit (APX803) on the display board, otherwise 100ms would be sufficient
	co_await loop.sleep(300ms);

	// release reset pin
	resetOutput.set(0, resetPin);
}*/

AwaitableCoroutine SSD130x::init() {
	auto flags = this->flags;

	// set header for i2c
	if ((flags & Flags::I2C) != 0)
		this->buf.headerData()[0] = 0; // Co = 0, D/C = 0

	// initialize the display
	auto d = this->buf.data();
	int i = 0;

	if ((flags & Flags::SSD1309) != 0) {
		d[i++] = COMMAND_LOCK;
		d[i++] = 0x12; // unlock
	}

	d[i++] = DISPLAY_OFF;

	d[i++] = CLOCK_DIV;
	d[i++] = 0xA0;

	d[i++] = MULTIPLEX;
	d[i++] = this->h - 1;

	d[i++] = DISPLAY_OFFSET;
	d[i++] = 0; // no offset

	d[i++] = START_LINE + 0; // line 0

	d[i++] = (flags & Flags::FLIP_X) != 0 ? SEGMENT_REMAP1 : SEGMENT_REMAP0;

	d[i++] = (flags & Flags::FLIP_Y) != 0 ? COM_SCAN_DEC : COM_SCAN_INC;

	d[i++] = COM_PINS_CONFIG;
	d[i++] = (extract(int(flags), int(Flags::COM3)) << 4) | 0x02;

	d[i++] = CONTRAST;
	d[i++] = 0xFF;

	d[i++] = PRECHARGE_PERIOD;
	d[i++] = 0x82;//F1;

	d[i++] = VCOMH_DESELECT;
	d[i++] = 0x34;//55;

	// horizontal addressing mode
	d[i++] = ADDRESSING_MODE;
	d[i++] = ADDRESSING_MODE_HORIZONTAL;

	d[i++] = ALL_ON_DISABLE;

	d[i++] = INVERT_DISABLE;

	// write as command for 4 wire SPI variant
	co_await this->buf.write(i, Buffer::Op::COMMAND);
}

AwaitableCoroutine SSD130x::enable() {
	if ((this->flags & Flags::I2C) != 0)
		this->buf.headerData()[0] = 0; // Co = 0, D/C = 0

	//display::enableVcc(true);
	this->buf[0] = DISPLAY_ON;
	co_await this->buf.write(1, Buffer::Op::COMMAND);
	//this->p.enabled = true;
}

AwaitableCoroutine SSD130x::disable() {
	if ((this->flags & Flags::I2C) != 0)
		this->buf.headerData()[0] = 0; // Co = 0, D/C = 0

	this->buf[0] = DISPLAY_OFF;
	co_await this->buf.write(1, Buffer::Op::COMMAND);
	//this->p.enabled = true;
	//display::enableVcc(false);
}

AwaitableCoroutine SSD130x::setContrast(uint8_t contrast) {
	if ((this->flags & Flags::I2C) != 0)
		this->buf.headerData()[0] = 0; // Co = 0, D/C = 0

	this->buf[0] = CONTRAST;
	this->buf[1] = contrast;
	co_await this->buf.write(2, Buffer::Op::COMMAND);
}

Awaitable<Buffer::Events> SSD130x::show() {
	if ((this->flags & Flags::I2C) != 0)
		this->buf.headerData()[0] = 0x40; // Co = 0, D/C = 1
	return this->buf.write(bufferSize(this->w, this->h));
}

void SSD130x::startWrite() {
	if ((this->flags & Flags::I2C) != 0)
		this->buf.headerData()[0] = 0x40; // Co = 0, D/C = 1
	this->buf.startWrite(bufferSize(this->w, this->h));
}

} // namespace coco
