#pragma once

#include <coco/platform/Gui.hpp>


namespace coco {

/**
	Monochrome display on the emulator gui.
	Usage: gui.draw<GuiDisplay>(image, width, height);
*/
class GuiDisplay : public Gui::Renderer {
public:
	GuiDisplay();

	float2 draw(float2 position, const uint8_t *buffer, int width, int height);

protected:
	GLuint texture;
};

} // namespace coco
