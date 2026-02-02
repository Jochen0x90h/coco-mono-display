#pragma once

#include <coco/SSD130x.hpp>
#include <coco/platform/Loop_TIM2.hpp>
#include <coco/platform/SpiMaster_SPI_DMA.hpp>
#include <coco/platform/SpiDisplayChannel_SPI_DMA.hpp>
#include <coco/platform/OutputPort_GPIO.hpp>
#include "config.hpp"


using namespace coco;
using namespace coco::literals;

constexpr int DISPLAY_WIDTH = 128;
constexpr int DISPLAY_HEIGHT = 64;
constexpr SSD130x::Flags DISPLAY_FLAGS = SSD130x::Flags::UG_2864ASWPG01_SPI;

static const OutputPort_GPIO::Config outConfig[] {
    {gpio::PC15 | gpio::Config::SPEED_MEDIUM | gpio::Config::INVERT, false}, // DISP_nRST
};

/// @brief Drivers for LedControl board with UG_2864ASWPG01 connected via SPI
///
struct Drivers {
    Loop_TIM2 loop{APB1_TIMER_CLOCK};

    using SpiMaster = SpiMaster_SPI_DMA;
    SpiMaster displaySpi{loop,
        gpio::PA5 | gpio::AF5 | gpio::Config::SPEED_MEDIUM, // SPI1 DISP_SCK (don't forget to lookup the alternate function number in the data sheet!)
        gpio::PA7 | gpio::AF5 | gpio::Config::SPEED_MEDIUM, // SPI1 DISP_MOSI
        gpio::NONE, // SPI1 MISO, not used
        spi::SPI1_INFO,
        dma::DMA1_CH1_CH2_INFO};
    SpiDisplayChannel_SPI_DMA displayChannel{displaySpi,
        gpio::PA4 | gpio::Config::SPEED_MEDIUM | gpio::Config::INVERT, // DISP_nCS
        gpio::PA0 | gpio::Config::SPEED_MEDIUM, false, 0x40, // DISP_D/nC
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
