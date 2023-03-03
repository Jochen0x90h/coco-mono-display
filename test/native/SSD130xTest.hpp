#pragma once

#include <coco/platform/SSD130x_cout.hpp>
#include <coco/SSD130x.hpp>


using namespace coco;

constexpr int DISPLAY_WIDTH = 128;
constexpr int DISPLAY_HEIGHT = 64;
constexpr SSD130x::Flags DISPLAY_FLAGS = SSD130x::Flags::NONE;

// drivers for SSD130xTest
struct Drivers {
	Loop_native loop;
	SSD130x_cout buffer{loop, 128, 64};
};
