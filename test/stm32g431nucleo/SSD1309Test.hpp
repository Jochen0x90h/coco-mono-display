#pragma once

#include <coco/SSD130x.hpp>
#include <coco/platform/Loop_TIM2.hpp>
#include <coco/platform/SpiMaster_SPI_DMA.hpp>
#include <coco/platform/OutputPort_GPIO.hpp>
#include <coco/board/config.hpp>


using namespace coco;
using namespace coco::literals;

constexpr int DISPLAY_WIDTH = 128;
constexpr int DISPLAY_HEIGHT = 64;
constexpr SSD130x::Flags DISPLAY_FLAGS = SSD130x::Flags::UG_2864ASWPG01_SPI;

static const OutputPort_GPIO::Config outConfig[] {
	{gpio::Config::PB10 | gpio::Config::SPEED_MEDIUM | gpio::Config::INVERT, false}, // reset (CN9 7)
};

/**
 * Drivers for SSD1309Test with UG_2864ASWPG01 connected via SPI
 * Use pcb/Display or other SPI display module and connect it to the nucleo board like this:
 * GND -> CN5 7 or CN6 6
 * 3V3 -> CN6 4
 * 12V -> CN6 5 (the nucleo board provides only 5V which is sufficient for testing)
 * SCK -> CN9 4
 * MOSI -> CN9 5
 * DC -> CN9 8
 * CS -> CN5 1
 * Reset -> CN9 7 (or reset the display module by hand by connecting RESET to GND, then reset the nucleo board)
 */
struct Drivers {
	Loop_TIM2 loop{APB1_TIMER_CLOCK};

	using SpiMaster = SpiMaster_SPI_DMA;
	SpiMaster displaySpi{loop,
		gpio::Config::PB3 | gpio::Config::AF5 | gpio::Config::SPEED_MEDIUM, // SPI1 SCK (CN9 4) (don't forget to lookup the alternate function number in the data sheet!)
		gpio::Config::NONE, // SPI1 MISO (CN9 6), not used
		gpio::Config::PB5 | gpio::Config::AF5 | gpio::Config::SPEED_MEDIUM, // SPI1 MOSI (CN9 5)
		gpio::Config::PA8 | gpio::Config::SPEED_MEDIUM, // DC (CN9 8)
		spi::SPI1_INFO,
		dma::DMA1_CH1_CH2_INFO,
		spi::Config::CLOCK_DIV128 | spi::Config::PHA1_POL1 | spi::Config::DATA_8};
	SpiMaster::Channel displayChannel{displaySpi, gpio::Config::PA9 | gpio::Config::SPEED_MEDIUM | gpio::Config::INVERT, true}; // nCS (CN5 1)
	SpiMaster::Buffer<DISPLAY_WIDTH * DISPLAY_HEIGHT / 8> displayBuffer{displayChannel};

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
void DMA1_Channel1_IRQHandler() {
	drivers.displaySpi.DMA_Rx_IRQHandler();
}
}
