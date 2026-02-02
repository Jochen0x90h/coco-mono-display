#pragma once

#include <coco/SSD130x.hpp>
#include <coco/platform/Loop_TIM2.hpp>
#include <coco/platform/SpiMaster_SPI_DMA.hpp>
#include <coco/platform/SpiDisplayChannel_SPI_DMA.hpp>
#include <coco/platform/OutputPort_GPIO.hpp>
#include <coco/board/config.hpp>


using namespace coco;
using namespace coco::literals;

constexpr int DISPLAY_WIDTH = 128;
constexpr int DISPLAY_HEIGHT = 64;
constexpr SSD130x::Flags DISPLAY_FLAGS = SSD130x::Flags::UG_2864ASWPG01_SPI;

static const OutputPort_GPIO::Config outConfig[] {
    {gpio::PB10 | gpio::Config::SPEED_MEDIUM | gpio::Config::INVERT, false}, // nRST (CN9 7)
};

/// @brief Drivers for SSD1309Test with UG_2864ASWPG01 connected via SPI
/// Use pcb/Display or other SPI display module and connect it to the nucleo board like this:
/// GND -> CN5 7 or CN6 6
/// 3V3 -> CN6 4
/// 12V -> CN6 5 (the nucleo board provides only 5V which is sufficient for testing)
/// SCK -> CN9 4
/// MOSI -> CN9 5
/// D/nC -> CN9 8
/// nCS -> CN5 1
/// nRST -> CN9 7 (or reset the display module by hand by connecting RESET to GND, then reset the nucleo board)
struct Drivers {
    Loop_TIM2 loop{APB1_TIMER_CLOCK};

    using SpiMaster = SpiMaster_SPI_DMA;
    SpiMaster displaySpi{loop,
        gpio::PB3 | gpio::AF5 | gpio::Config::SPEED_MEDIUM, // SPI1 SCK (CN9 4) (don't forget to lookup the alternate function number in the data sheet!)
        gpio::PB5 | gpio::AF5 | gpio::Config::SPEED_MEDIUM, // SPI1 MOSI (CN9 5)
        gpio::NONE, // SPI1 MISO (CN9 6), not used
        spi::SPI1_INFO,
        dma::DMA1_CH1_CH2_INFO};
    SpiDisplayChannel_SPI_DMA displayChannel{displaySpi,
        gpio::PA9 | gpio::Config::SPEED_MEDIUM | gpio::Config::INVERT, // nCS (CN5 1)
        gpio::PA8 | gpio::Config::SPEED_MEDIUM, false, 0x40, // D/nC (CN9 8)
        spi::Format::CLOCK_DIV_128 | spi::Format::PHA1_POL1 | spi::Format::DATA_8};

    SpiMaster::Buffer<1, DISPLAY_WIDTH * DISPLAY_HEIGHT / 8> displayBuffer{displayChannel};

    OutputPort_GPIO resetPin{outConfig};
};

Drivers drivers;

extern "C" {
void DMA1_Channel1_IRQHandler() {
    drivers.displaySpi.DMA_Rx_IRQHandler();
}
}
