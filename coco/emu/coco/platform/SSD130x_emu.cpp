#include "SSD130x_emu.hpp"
#include "GuiDisplay.hpp"


namespace coco {

SSD130x_emu::SSD130x_emu(Loop_emu &loop, int width, int height)
    : Buffer(new uint8_t[1 + width * ((height + 7) >> 3)], 1, 1, width * ((height + 7) >> 3), State::READY) // header capacity is 1
    , width_(width), height_(height)
    , image_(new uint8_t[width * height * 4])
{
    std::fill(image_, image_ + width * height * 4, 0);

    loop.guiHandlers.add(*this);
}

SSD130x_emu::~SSD130x_emu() {
    delete [] header_;
    delete [] image_;
}

bool SSD130x_emu::start(Op op) {
    if (st.state != State::READY) {
        assert(st.state != State::BUSY);
        return false;
    }

    // check if WRITE flag is set
    assert((op & Op::WRITE) != 0);

    op_ = op;

    // set state
    setBusy();

    return true;
}

bool SSD130x_emu::cancel() {
    if (st.state != State::BUSY)
        return false;

    setReady(0);

    return true;
}

void SSD130x_emu::handle(Gui &gui) {
    if (st.state == State::BUSY) {
        auto op = op_;
        bool command = (header_[0] & 0x40) == 0;
        auto data = data_;
        int size = size_;
        if (command) {
            // command
            for (int i = 0; i < size; ++i) {
                switch (data[i]) {
                    // set contrast control
                case 0x81:
                    contrast_ = data[++i];
                    break;

                    // entire display on
                case 0xA4:
                    allOn_ = false;
                    break;
                case 0xA5:
                    allOn_ = true;
                    break;

                    // set normal/inverse display
                case 0xA6:
                    inverse_ = false;
                    break;
                case 0xA7:
                    inverse_ = true;
                    break;

                    // set display on/off
                case 0xAE:
                    enabled_ = false;
                    break;
                case 0xAF:
                    enabled_ = true;
                    break;
                }
            }
        } else {
            // data
            int width = width_;
            int height = height_;
            uint8_t foreground = !enabled_ ? 0 : contrast_;
            uint8_t background = (!enabled_ || allOn_) ? foreground : (48 * contrast_) / 255;
            if (inverse_)
                std::swap(foreground, background);

            // convert from bitmap to grayscale
            for (int j = 0; j < height; ++j) {
                uint8_t *line = &image_[width * j *4];
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

    gui.draw<GuiDisplay>(image_, width_ * 2, height_ * 2);
}

} // namespace coco
