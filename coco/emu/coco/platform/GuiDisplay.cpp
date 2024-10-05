#include "GuiDisplay.hpp"


namespace coco {

GuiDisplay::GuiDisplay()
	: Renderer("#version 330\n"
		"uniform sampler2D tex;\n"
		"in vec2 xy;\n"
		"out vec4 pixel;\n"
		"void main() {\n"
		"pixel = texture(tex, xy).xxxw;\n"
		"}\n")
{
	this->texture = Gui::createTexture(GL_LINEAR);
}

float2 GuiDisplay::draw(float2 position, const uint8_t *buffer, int width, int height) {
	const float2 size = {0.4f, 0.2f};

	setState(position, size);
	glBindTexture(GL_TEXTURE_2D, this->texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width, height, 0, GL_RED, GL_UNSIGNED_BYTE, buffer);
	drawAndResetState();
	glBindTexture(GL_TEXTURE_2D, 0);

	return size;
}

} // namespace coco
