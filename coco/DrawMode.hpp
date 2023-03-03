#pragma once


namespace coco {

// draw modes
enum class DrawMode : uint8_t {
	// foreground
	CLEAR = 0,
	FLIP = 0x1,
	KEEP = 0x2,
	SET = 0x3,
	MASK = 0x3,
};

} // namespace coco
