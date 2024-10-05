#include "SSD130x_cout.hpp"
#include <iostream>
#include <iomanip>


namespace coco {

SSD130x_cout::SSD130x_cout(Loop_native &loop, int width, int height)
	: Buffer(new uint8_t[1 + width * ((height + 7) >> 3)], 1 + width * ((height + 7) >> 3), State::READY)
	, loop(loop)
	, callback(makeCallback<SSD130x_cout, &SSD130x_cout::handle>(this))
	, width(width), height(height)
{
}

SSD130x_cout::~SSD130x_cout() {
	delete [] this->p.data;
}

bool SSD130x_cout::start(Op op) {
	if (this->st.state != State::READY) {
		assert(this->st.state != State::BUSY);
		return false;
	}

	// check if WRITE flag is set
	assert((op & Op::WRITE) != 0);

	this->op = op;

	this->loop.invoke(this->callback);

	// set state
	setBusy();

	return true;
}

bool SSD130x_cout::cancel() {
	if (this->st.state != State::BUSY)
		return false;

	this->callback.remove();
	setReady(0);

	return true;
}

void SSD130x_cout::handle() {
	auto op = this->op;
	auto data = this->p.data + this->p.headerSize;
	int size = this->p.size - this->p.headerSize;

	if ((op & Op::WRITE) != 0 && (op & Op::COMMAND) == 0 && size >= this->width * ((this->height + 7) >> 3)) {
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
