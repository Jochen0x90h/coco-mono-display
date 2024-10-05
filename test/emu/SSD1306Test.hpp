#pragma once

#include <coco/platform/SSD130x_emu.hpp>
#include <coco/SSD130x.hpp>


using namespace coco;

constexpr int DISPLAY_WIDTH = 128;
constexpr int DISPLAY_HEIGHT = 64;
constexpr SSD130x::Flags DISPLAY_FLAGS = SSD130x::Flags::SSD1306 | SSD130x::Flags::I2C;

// drivers for SSD130xTest
struct Drivers {
	Loop_emu loop;
	SSD130x_emu displayBuffer{loop, DISPLAY_WIDTH, DISPLAY_HEIGHT};

	AwaitableCoroutine resetDisplay() {co_return;}
};

Drivers drivers;
