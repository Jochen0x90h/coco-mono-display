#pragma once

#include <coco/platform/Gui.hpp>


namespace coco {

/**
	Monochrome display on the emulator gui.
	Usage: gui.draw<Display>(image, width, height);
*/
class Display : public Gui::Renderer {
public:
	Display();

	Gui::Size draw(float x, float y, const uint8_t *buffer, int width, int height);

protected:
	GLuint texture;
};

} // namespace coco
