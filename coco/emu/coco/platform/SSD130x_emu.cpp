#include "SSD130x_emu.hpp"
#include "GuiDisplay.hpp"


namespace coco {

SSD130x_emu::SSD130x_emu(Loop_emu &loop, int width, int height)
	: Buffer(new uint8_t[1 + width * ((height + 7) >> 3)], 1 + width * ((height + 7) >> 3), State::READY), width(width), height(height)
	, image(new uint8_t[width * height * 4])
{
	std::fill(this->image, this->image + width * height * 4, 0);

	loop.guiHandlers.add(*this);
}

SSD130x_emu::~SSD130x_emu() {
	delete [] this->p.data;
	delete [] this->image;
}

bool SSD130x_emu::start(Op op) {
	if (this->st.state != State::READY) {
		assert(this->st.state != State::BUSY);
		return false;
	}

	// check if WRITE flag is set
	assert((op & Op::WRITE) != 0);

	this->op = op;

	// set state
	setBusy();

	return true;
}

bool SSD130x_emu::cancel() {
	if (this->st.state != State::BUSY)
		return false;

	setReady(0);

	return true;
}

void SSD130x_emu::handle(Gui &gui) {
	if (this->st.state == State::BUSY) {
		auto op = this->op;
		auto data = this->p.data + this->p.headerSize;
		int size = this->p.size - this->p.headerSize;
		if ((op & Op::COMMAND) != 0) {
			// command
			for (int i = 0; i < size; ++i) {
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
				uint8_t *line = &this->image[width * j *4];
				for (int i = 0; i < width; ++i) {
					bool bit = (data[i + width * (j >> 3)] & (1 << (j & 7))) != 0;
					uint8_t pixel = bit ? foreground : background;
					//line[i] = pixel;
					line[i * 2] = pixel;
					line[i * 2 + 1] = pixel;
					line[i * 2 + width * 2] = pixel;
					line[i * 2 + width * 2 + 1] = pixel;
				}
			}
		}

		setReady();
	}

	gui.draw<GuiDisplay>(this->image, this->width * 2, this->height * 2);
}

} // namespace coco
