#include "SSD130x_cout.hpp"
#include <iostream>
#include <iomanip>


namespace coco {

SSD130x_cout::SSD130x_cout(Loop_native &loop, int width, int height)
	: BufferImpl(new uint8_t[width * ((height + 7) >> 3)], width * ((height + 7) >> 3), State::READY), loop(loop), width(width), height(height) {
}

SSD130x_cout::~SSD130x_cout() {
	delete [] this->dat;
}

bool SSD130x_cout::setHeader(const uint8_t *data, int size) {
	// I2C header
	assert(size <= 1);
	return size <= 1;
}

bool SSD130x_cout::startInternal(int size, Op op) {
	if (this->stat != State::READY) {
		assert(false);
		return false;
	}

	// check if WRITE flag is set
	assert((op & Op::WRITE) != 0);

	this->xferred = size;
	this->op = op;

	if (!this->inList())
		this->loop.yieldHandlers.add(*this);

	// set state
	setBusy();

	return true;
}

void SSD130x_cout::cancel() {
	if (this->stat != State::BUSY)
		return;

	remove();
	setReady(0);
}

void SSD130x_cout::handle() {
	remove();

	auto op = this->op;
	auto data = this->dat;
	int transferred = this->xferred;

	if ((op & Op::WRITE) != 0 && (op & Op::COMMAND) == 0 && transferred >= this->width * ((this->height + 7) >> 3)) {
		std::cout << std::endl;
		for (int y = 0; y < this->height; ++y) {
			uint8_t *line = data + (y >> 3) * this->width;
			std::cout << std::setw(2) << y << ": ";
			for (int x = 0; x < this->width; ++x) {
				bool on = (line[x] & (1 << (y & 7))) != 0;
				std::cout << (on ? 'O' : ' ');
			}
			std::cout << std::endl;
		}
	}

	setReady();
}

} // namespace coco
