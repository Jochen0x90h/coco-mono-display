#include "SSD130x_emu.hpp"
#include "GuiDisplay.hpp"


namespace coco {

SSD130x_emu::SSD130x_emu(Loop_emu &loop, int width, int height)
	: BufferImpl(new uint8_t[width * ((height + 7) >> 3)], width * ((height + 7) >> 3), State::READY), width(width), height(height)
	, image(new uint8_t[width * height])
{
	std::fill(this->image, this->image + width * height, 0);

	loop.guiHandlers.add(*this);
}

SSD130x_emu::~SSD130x_emu() {
	delete [] this->dat;
	delete [] this->image;
}

bool SSD130x_emu::setHeader(const uint8_t *data, int size) {
	// I2C header
	assert(size <= 1);
	return size <= 1;
}

bool SSD130x_emu::startInternal(int size, Op op) {
	if (this->stat != State::READY) {
		assert(false);
		return false;
	}

	// check if WRITE flag is set
	assert((op & Op::WRITE) != 0);

	this->xferred = size;
	this->op = op;

	// set state
	setBusy();

	return true;
}

void SSD130x_emu::cancel() {
	if (this->stat != State::BUSY)
		return;

	setReady(0);
}

void SSD130x_emu::handle(Gui &gui) {
	if (this->stat == State::BUSY) {
		auto op = this->op;
		auto data = this->dat;
		int transferred = this->xferred;
		if ((op & Op::COMMAND) != 0) {
			// command
			for (int i = 0; i < transferred; ++i) {
				switch (data[i]) {
					// set contrast control
				case 0x81:
					this->contrast = data[++i];
					break;

					// entire display on
				case 0xA4:
					this->allOn = false;
					break;
				case 0xA5:
					this->allOn = true;
					break;

					// set normal/inverse display
				case 0xA6:
					this->inverse = false;
					break;
				case 0xA7:
					this->inverse = true;
					break;

					// set display on/off
				case 0xAE:
					this->enabled = false;
					break;
				case 0xAF:
					this->enabled = true;
					break;
				}
			}
		} else {
			// data
			int width = this->width;
			int height = this->height;
			uint8_t foreground = !this->enabled ? 0 : this->contrast;
			uint8_t background = (!this->enabled || this->allOn) ? foreground : (48 * this->contrast) / 255;
			if (this->inverse)
				std::swap(foreground, background);

			// convert from bitmap to grayscale
			for (int j = 0; j < height; ++j) {
				uint8_t *line = &this->image[width * j];
				for (int i = 0; i < width; ++i) {
					bool bit = (data[i + width * (j >> 3)] & (1 << (j & 7))) != 0;
					line[i] = bit ? foreground : background;
				}
			}
		}

		setReady();
	}

	gui.draw<GuiDisplay>(this->image, this->width, this->height);
}

} // namespace coco
