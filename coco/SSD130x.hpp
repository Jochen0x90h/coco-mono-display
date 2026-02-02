#pragma once

#include "Bitmap.hpp"
#include <coco/Buffer.hpp>
#include <coco/Coroutine.hpp>
#include <coco/Loop.hpp>


namespace coco {

/// @brief Display controller for SSD1306 and SSD1309
/// Also see https://github.com/adafruit/Adafruit_SSD1306
///
/// A display of resulution 128x64 is organized as 128 colums and 8 pages where each byte contains 8 pixels of a column
/// (64 rows = 8 pages * 8 bytes)
class SSD130x {
public:
    // flags
    enum class Flags {
        // driver chip
        SSD1306 = 0,
        SSD1309 = 1,

        // display is connected via SPI (needs clock phase = 1 and polarity = 1)
        SPI = 0,

        // display is connected via I2C
        I2C = 2,

        // flip options
        FLIP_X = 4,
        FLIP_Y = 8,

        // com pins configuration
        COM0 = 0 << 4,
        COM1 = 1 << 4,
        COM2 = 2 << 4,
        COM3 = 3 << 4,

        // display modules
        ADAFRUIT_OLED_BONNET = SSD1306 | I2C | FLIP_X | FLIP_Y | COM1, // 128x64 1.3"
        UG_2864ASWPG01_SPI = SSD1309 | SPI | FLIP_X | FLIP_Y | COM1,
        UG_2864ASWPG01_I2C = SSD1309 | I2C | FLIP_X | FLIP_Y | COM1,
    };

    static constexpr int bufferSize(int width, int height) {return width * ((height + 7) >> 3);}

    static int bufferSize2(int width, int height, bool i2c) {
        int wh = width * ((height + 7) >> 3);
        return (i2c ? 1 : 0) + wh;
    }

    /// @brief Constructor
    /// @param buffer Buffer with header capacity 0 for SPI and 1 for I2C and capacity of DISPLAY_WIDTH * DISPLAY_HEIGHT / 8
    /// @param width Width of display
    /// @param height Height of display
    /// @param flags Flags
    SSD130x(Buffer &buffer, int width, int height, Flags flags);

    int width() {return this->width_;}
    int height() {return this->height_;}

    /**
     * Reset the display
     */
    //[[nodiscard]] AwaitableCoroutine reset(Loop &loop, OutputPins &resetOutput, int resetPin = 1);

    /// @brief Initializate the display
    /// @param loop event loop
    /// @return use co_await on return value to await end of initialization
    [[nodiscard]] AwaitableCoroutine init();

    /// @brief Power up Vcc (if supported) and enable the display
    /// @return use co_await on return value to await end of operation
    [[nodiscard]] AwaitableCoroutine enable();

    /// @brief Disable the display and power down Vcc (if supported)
    /// @return use co_await on return value to await end of operation
    [[nodiscard]] AwaitableCoroutine disable();

    /// @brief Obtains the underlying buffer
    ///
    Buffer &buffer() {return this->buffer_;}

    /// @brief Set contrast of display
    /// @return use co_await on return value to await end of operation
    [[nodiscard]] AwaitableCoroutine setContrast(uint8_t contrast);

    /// @brief Obtain the display bitmap to draw something onto it
    ///
    Bitmap bitmap() {
        return {this->buffer_.data(), this->width_, this->height_};
    }

    /// @brief Show the contents of the bitmap on the display
    /// @return use co_await on return value to await end of operation
    [[nodiscard]] Awaitable<Buffer::Events> show();

    /// @brief Start writing the bitmap to the display. The application can co_await display.buffer().untilReady() to wait
    /// for end of write operation
    void startWrite();

protected:
    Buffer &buffer_;
    int width_;
    int height_;
    Flags flags_;
};
COCO_ENUM(SSD130x::Flags);

} // namespace coco
