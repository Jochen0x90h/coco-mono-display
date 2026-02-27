#include "SSD130x_cout.hpp"
#include <iostream>
#include <iomanip>


namespace coco {

SSD130x_cout::SSD130x_cout(Loop_native &loop, int width, int height)
    : Buffer(new uint8_t[1 + width * ((height + 7) >> 3)], 1, width * ((height + 7) >> 3), State::READY) // header capacity is 1
    , loop_(loop)
    , callback_(makeCallback<SSD130x_cout, &SSD130x_cout::handle>(this))
    , width_(width), height_(height)
{
}

SSD130x_cout::~SSD130x_cout() {
    delete [] header_;
}

bool SSD130x_cout::start() {
    if (state_ != State::READY || (op_ & Op::WRITE) == 0 || size_ == 0) {
        assert(state_ != State::BUSY);
        setSuccess();
        return false;
    }

    loop_.invoke(callback_);

    // set state
    setBusy();

    return true;
}

bool SSD130x_cout::cancel() {
    if (state_ != State::BUSY)
        return false;

    callback_.remove();
    setError(std::errc::operation_canceled);
    setReady();

    return true;
}

void SSD130x_cout::handle() {
    auto op = op_;
    bool command = (header_[0] & 0x40) == 0;
    auto data = data_;
    int size = size_;

    if ((op & Op::WRITE) != 0 && !command && size >= width_ * ((height_ + 7) >> 3)) {
        std::cout << std::endl;
        for (int y = 0; y < height_; ++y) {
            uint8_t *line = data + (y >> 3) * width_;
            std::cout << std::setw(2) << y << ": ";
            for (int x = 0; x < width_; ++x) {
                bool on = (line[x] & (1 << (y & 7))) != 0;
                std::cout << (on ? 'O' : ' ');
            }
            std::cout << std::endl;
        }
    }

    setReady();
}

} // namespace coco
