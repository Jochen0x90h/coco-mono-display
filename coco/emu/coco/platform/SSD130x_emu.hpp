#include <coco/platform/Loop_emu.hpp>
#include <coco/BufferImpl.hpp>
#include <string>


namespace coco {

/**
	Implementation of an SSD1306/SSD1309 emulator that shows the display on the emulator gui
*/
class SSD130x_emu : public BufferImpl, public Loop_emu::GuiHandler {
public:
	/**
		Constructor
		@param loop event loop
		@param width width of emulated display
		@param height height of emulated display
	*/
	SSD130x_emu(Loop_emu &loop, int width, int height);
	~SSD130x_emu() override;

	bool setHeader(const uint8_t *data, int size) override;
	using BufferImpl::setHeader;
	bool startInternal(int size, Op op) override;
	void cancel() override;

protected:
	void handle(Gui &gui) override;

	int width;
	int height;

	Op op;

	bool enabled;
	uint8_t contrast = 255;
	bool allOn = false;
	bool inverse = false;

	// grayscale display image
	uint8_t *image;
};

} // namespace coco
