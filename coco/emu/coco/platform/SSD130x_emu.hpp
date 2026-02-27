#include <coco/platform/Loop_emu.hpp>
#include <coco/Buffer.hpp>
#include <string>


namespace coco {

/// @brief Implementation of an SSD1306/SSD1309 emulator that shows the display on the emulator gui
///
class SSD130x_emu : public Buffer, public Loop_emu::GuiHandler {
public:
    /// @brief Constructor
    /// @param loop event loop
    /// @param width width of emulated display
    /// @param height height of emulated display
    SSD130x_emu(Loop_emu &loop, int width, int height);
    ~SSD130x_emu() override;

    bool start() override;
    bool cancel() override;

protected:
    void handle(Gui &gui) override;

    int width_;
    int height_;

    bool enabled_;
    uint8_t contrast_ = 255;
    bool allOn_ = false;
    bool inverse_ = false;

    // grayscale display image (one byte for each pixel)
    uint8_t *image_;
};

} // namespace coco
