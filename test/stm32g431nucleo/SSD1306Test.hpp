#pragma once

#include <coco/SSD130x.hpp>
#include <coco/platform/Loop_TIM2.hpp>
#include <coco/platform/I2cMaster_I2C_DMA.hpp>
#include <coco/platform/OutputPort_GPIO.hpp>
#include <coco/board/config.hpp>


using namespace coco;
using namespace coco::literals;

constexpr int DISPLAY_WIDTH = 128;
constexpr int DISPLAY_HEIGHT = 64;
constexpr SSD130x::Flags DISPLAY_FLAGS = SSD130x::Flags::ADAFRUIT_OLED_BONNET;

static const OutputPort_GPIO::Config outConfig[] {
	{gpio::Config::PB10 | gpio::Config::SPEED_MEDIUM | gpio::Config::INVERT, false}, // reset (CN9 7)
};

/**
 * Drivers for SSD1306Test with Adafruit OLED bonnet
 * Use pcb/Display or other SPI display module and connect it to the nucleo board like this:
 * todo
 */
struct Drivers {
	Loop_TIM2 loop{APB1_TIMER_CLOCK};

	using I2cMaster = I2cMaster_I2C_DMA;
	I2cMaster i2c{loop,
		gpio::Config::PA15 | gpio::Config::AF4 | gpio::Config::DRIVE_DOWN | gpio::Config::PULL_UP, // SCL (CN7 38)
		gpio::Config::PB7 | gpio::Config::AF4 | gpio::Config::DRIVE_DOWN | gpio::Config::PULL_UP, // SDA (CN7 21)
		i2c::I2C1_INFO,
		dma::DMA1_CH3_CH4_INFO,
		//0x00303D5B}; // timing for 100kHz I2C and 16MHz clock from STM32Cube
		//0x0010061A}; // timing for 400kHz I2C and 16MHz clock from STM32Cube
		//0x00000107}; // timing for 1MHz I2C and 16MHz clock from STM32Cube
		//0x00404C74}; // timing for 100kHz I2C and 20MHz clock from STM32Cube
		0x00100822}; // timing for 400kHz I2C and 20MHz clock from STM32Cube
	I2cMaster::Channel displayChannel{i2c, 0x3c}; // SSD1306 display (address 011 1100)
	I2cMaster::Buffer<4 + DISPLAY_WIDTH * DISPLAY_HEIGHT / 8> displayBuffer{displayChannel};

	OutputPort_GPIO resetPin{outConfig};

	AwaitableCoroutine resetDisplay() {
		resetPin.set(1, 1);
		co_await loop.sleep(10ms);
		resetPin.set(0, 1);
		co_return;
	}
};

Drivers drivers;

extern "C" {
void I2C1_EV_IRQHandler() {
	drivers.i2c.I2C_IRQHandler();
}
}
