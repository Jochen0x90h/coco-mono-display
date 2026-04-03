#pragma once

#include "config.hpp"
#include <coco/SSD130x.hpp>
#include <coco/platform/InputDevice_EXTI_TIM.hpp>
#include <coco/platform/Loop_TIM2.hpp>
#include <coco/platform/SpiMaster_SPI_DMA.hpp>
#include <coco/platform/SpiDisplayChannel_SPI_DMA.hpp>
#include <coco/platform/OutputPort_GPIO.hpp>


using namespace coco;
using namespace coco::literals;

constexpr int DISPLAY_WIDTH = 128;
constexpr int DISPLAY_HEIGHT = 64;
constexpr SSD130x::Flags DISPLAY_FLAGS = SSD130x::Flags::UG_2864ASWPG01_SPI;


/// @brief Drivers for MenuTest on LedControl board with UG_2864ASWPG01 connected via SPI
///
struct Drivers {
    Loop_TIM2 loop{APB1_TIMER_CLOCK};

    // display
    using SpiMaster = SpiMaster_SPI_DMA;
    SpiMaster displaySpi{loop,
        gpio::PA5 | gpio::AF5 | gpio::Config::SPEED_MEDIUM, // SPI1 SCK (DISP_SCK)
        gpio::PA7 | gpio::AF5 | gpio::Config::SPEED_MEDIUM, // SPI1 MOSI (DISP_MOSI)
        gpio::NONE, // no MISO, send only
        spi::SPI1_INFO,
        dma::DMA1_CH1_CH2_INFO};
    SpiDisplayChannel_SPI_DMA displayChannel{displaySpi,
        gpio::PA4 | gpio::Config::SPEED_MEDIUM | gpio::Config::INVERT, // DISP_nCS
        gpio::PA0 | gpio::Config::SPEED_MEDIUM, false, 0x40, // DISP_D/nC
        spi::Format::CLOCK_DIV_32 | spi::Format::PHA1_POL1 | spi::Format::DATA_8};
    SpiMaster::Buffer<1, DISPLAY_WIDTH * DISPLAY_HEIGHT / 8> displayBuffer{displayChannel};

    // display reset pin
    static constexpr OutputPort_GPIO::Config outputConfig[] {
        {gpio::PC15 | gpio::Config::SPEED_MEDIUM | gpio::Config::INVERT, false}, // DISP_nRST
    };
    OutputPort_GPIO resetPin{outputConfig};


    // rotary knob with push button
    using InputDevice = InputDevice_EXTI_TIM;
    static constexpr gpio::Config inputPinConfigs[] {
        gpio::PC13 | gpio::Config::PULL_DOWN, // rotary knob A (ENC_A)
        gpio::PC14 | gpio::Config::PULL_DOWN, // rotary knob B (ENC_B)
        gpio::PB8 | gpio::Config::PULL_DOWN, // push button (SW)`
    };
    static constexpr InputDevice::Config inputConfigs[] {
        {0, 0, InputDevice::Init::INPUT, InputDevice::Action::DECREMENT_WHEN_ENABLED, InputDevice::Action::INCREMENT_WHEN_ENABLED, 1ms, 1ms}, // quadrature decoder (inputs 0 and 1, counter 0)
        {2, 1, InputDevice::Init::LOW, InputDevice::Action::INCREMENT, InputDevice::Action::NONE, 10ms, 10ms}, // button press (input 2, counter 1)
        {2, 2, InputDevice::Init::LOW, InputDevice::Action::INCREMENT, InputDevice::Action::NONE, 3s, 10ms}, // button long press (input 2, counter 2)
    };
    InputDevice input{loop,
        inputPinConfigs,
        inputConfigs,
        timer::TIM4_INFO,
        APB1_TIMER_CLOCK};
};

Drivers drivers;

extern "C" {

// display
void DMA1_Channel1_IRQHandler() {
    drivers.displaySpi.DMA_Rx_IRQHandler();
}

// rotary knob with push button
void EXTI9_5_IRQHandler() {
    drivers.input.EXTI_IRQHandler();
}
void EXTI15_10_IRQHandler() {
    drivers.input.EXTI_IRQHandler();
}
void TIM4_IRQHandler() {
    drivers.input.TIM_IRQHandler();
}

}
