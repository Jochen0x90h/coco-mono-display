#include "SSD130x.hpp"


namespace coco {

AwaitableCoroutine SSD130x::init() {
	bool externalVcc = (this->flags & Flags::EXTERNAL_VCC) != 0;

	if ((this->flags & Flags::I2C) != 0)
		this->buf.setHeader<uint8_t>(0); // Co = 0, D/C = 0

	auto d = this->buf.data();
	int i = 0;
	d[i++] = DISPLAY_OFF;

	d[i++] = CLOCK_DIV;
	d[i++] = 0x80;

	d[i++] = MULTIPLEX;
	d[i++] = this->h - 1;

	d[i++] = DISPLAY_OFFSET;
	d[i++] = 0; // no offset

	d[i++] = START_LINE + 0; // line 0

	d[i++] = CHARGEPUMP;
	d[i++] = externalVcc ? 0x10 : 0x14; // enable when no external vcc

	d[i++] = ADDRESSING_MODE;
	d[i++] = 0; // horizontal

	d[i++] = SEGMENT_REMAP1;

	d[i++] = COM_SCAN_DEC;

	uint8_t comPins = 0x02;
	uint8_t contrast = 0x8F;
	if (this->w == 128 && this->h == 32) {
		comPins = 0x02;
		contrast = 0x8F;
	} else if (this->w == 128 && this->h == 64) {
		comPins = 0x12;
		contrast = externalVcc ? 0x9F : 0xCF;
	} else if ((this->w == 96) && (this->h == 16)) {
		comPins = 0x2; // ada x12
		contrast = externalVcc ? 0x10 : 0xAF;
	} else {
		// Other screen varieties -- TBD

	}

	d[i++] = COM_PINS_CONFIG;
	d[i++] = comPins;

	d[i++] = CONTRAST;
	d[i++] = contrast;

	d[i++] = PRECHARGE_PERIOD;
	d[i++] = externalVcc ? 0x22 : 0xF1;

	d[i++] = VCOM_DETECT;
	d[i++] = 0x40;

	d[i++] = ALL_ON_DISABLE;

	d[i++] = INVERT_DISABLE;

	//d[i++] = DISPLAY_ON;

	// write as command for 4 wire SPI variant
	co_await this->buf.write(i, Buffer::Op::COMMAND);
}

AwaitableCoroutine SSD130x::enable() {
	if ((this->flags & Flags::I2C) != 0)
		this->buf.setHeader<uint8_t>(0); // Co = 0, D/C = 0

	//display::enableVcc(true);
	this->buf[0] = DISPLAY_ON;
	co_await this->buf.write(1, Buffer::Op::COMMAND);
	//this->p.enabled = true;
}

AwaitableCoroutine SSD130x::disable() {
	if ((this->flags & Flags::I2C) != 0)
		this->buf.setHeader<uint8_t>(0); // Co = 0, D/C = 0

	this->buf[0] = DISPLAY_OFF;
	co_await this->buf.write(1, Buffer::Op::COMMAND);
	//this->p.enabled = true;
	//display::enableVcc(false);
}

AwaitableCoroutine SSD130x::setContrast(uint8_t contrast) {
	if ((this->flags & Flags::I2C) != 0)
		this->buf.setHeader<uint8_t>(0); // Co = 0, D/C = 0

	this->buf[0] = CONTRAST;
	this->buf[1] = contrast;
	co_await this->buf.write(2, Buffer::Op::COMMAND);
}

Awaitable<Buffer::State> SSD130x::display() {
	if ((this->flags & Flags::I2C) != 0)
		this->buf.setHeader<uint8_t>(0x40); // Co = 0, D/C = 1
	return this->buf.write(bufferSize(this->w, this->h));
}

void SSD130x::startDisplay() {
	if ((this->flags & Flags::I2C) != 0)
		this->buf.setHeader<uint8_t>(0x40); // Co = 0, D/C = 1
	this->buf.start(bufferSize(this->w, this->h), Buffer::Op::WRITE);
}

} // namespace coco
