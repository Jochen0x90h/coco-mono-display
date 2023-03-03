#include <coco/BufferImpl.hpp>
#include <coco/platform/Loop_native.hpp>
#include <string>


namespace coco {

/**
	Implementation of an SSD1306/SSD1309 emulator that prints the dipslay contents to std::cout
*/
class SSD130x_cout : public BufferImpl, public Loop_native::YieldHandler {
public:
	/**
		Constructor
		@param loop event loop
		@param width width of emulated display
		@param height height of emulated display
	*/
	SSD130x_cout(Loop_native &loop, int width, int height);
	~SSD130x_cout() override;

	bool setHeader(const uint8_t *data, int size) override;
	using BufferImpl::setHeader;
	bool startInternal(int size, Op op) override;
	void cancel() override;

protected:
	void handle() override;

	Loop_native &loop;
	int width;
	int height;

	Op op;
};

} // namespace coco
