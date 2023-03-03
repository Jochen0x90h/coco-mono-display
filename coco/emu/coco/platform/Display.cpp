#include "Display.hpp"


namespace coco {

Display::Display()
	: Renderer("#version 330\n"
		"uniform sampler2D tex;\n"
		"in vec2 xy;\n"
		"out vec4 pixel;\n"
		"void main() {\n"
		"pixel = texture(tex, xy).xxxw;\n"
		"}\n")
{
	this->texture = Gui::createTexture();
}

Gui::Size Display::draw(float x, float y, const uint8_t *buffer, int width, int height) {
	const float w = 0.4f;
	const float h = 0.2f;

	setState(x, y, w, h);
	glBindTexture(GL_TEXTURE_2D, this->texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width, height, 0, GL_RED, GL_UNSIGNED_BYTE, buffer);
	drawAndResetState();
	glBindTexture(GL_TEXTURE_2D, 0);

	return {w, h};
}

} // namespace coco
