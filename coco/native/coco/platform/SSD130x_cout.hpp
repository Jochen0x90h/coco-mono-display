#include <coco/Buffer.hpp>
#include <coco/platform/Loop_native.hpp>
#include <string>


namespace coco {

/**
	Implementation of an SSD1306/SSD1309 emulator that prints the dipslay contents to std::cout
*/
class SSD130x_cout : public Buffer {
public:
	/**
		Constructor
		@param loop event loop
		@param width width of emulated display
		@param height height of emulated display
	*/
	SSD130x_cout(Loop_native &loop, int width, int height);
	~SSD130x_cout() override;

	bool start(Op op) override;
	bool cancel() override;

protected:
	void handle();

	Loop_native &loop;
	TimedTask<Callback> callback;

	int width;
	int height;

	Op op;
};

} // namespace coco
