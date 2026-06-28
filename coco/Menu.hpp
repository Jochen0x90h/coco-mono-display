#pragma once

#include "SSD130x.hpp"
#include <coco/Font.hpp>
#include <coco/InputDevice.hpp>


namespace coco {

/// @brief Menu for monochrome display
///
class Menu {
public:
    using Stream = BitmapStream;

    Menu(SSD130x &display, const LinearFont &font) : display_(display), font_(font) {}

    /// @brief Begin a menu
    /// @param buttons buttons that control the menu, either d-pad or rotary button
    /// @return true when the menu should be exited
    bool begin(InputDevice &buttons);

    /// @brief Show the menu on the display. Should be awaited before begin() gets called again
    /// @return use co_await and select on the return value to wait for redraw and user input, e.g. buttons
    [[nodiscard]] Awaitable<CoroutineTask<Buffer::Events>> show();

    /// @brief Wait for user input
    ///
    [[nodiscard]] auto untilInput(InputDevice &buttons) {return buttons.untilInput(seq_);}


    /// @brief Add a divider line to the menu
    ///
    void line();
    void beginSection();
    void endSection();


    Stream stream();

    /// @brief Add a label to the menu that can not be selected
    ///
    void label();

    /// @brief Add a label to the menu that can not be selected
    /// @param markup Text with markup (e.g. underline)
    template <typename T>
    void label(T markup) {
        Stream s = stream();
        s << markup;
        label();
    }

    /// @brief Add a menu entry
    ///
    bool entry();

    /// @brief Add a menu entry
    /// @param markup text with markup (e.g. underline)
    template <typename T>
    bool entry(T markup) {
        Stream s = stream();
        s << markup;
        return entry();
    }

    /// @brief Return the currently selected menu entry
    ///
    int getSelected() const {return selected_;}

    /// @brief Returns true if the current menu entry is selected
    ///
    bool isSelected() const {
        return selected_ == entryIndex_;
    }

    /// @brief Get edit state.
    /// Returns 0 if not in edit mode or not the entry being edited, otherwise returns the 1-based index
    /// of the field being edited
    int edit(int editCount = 1);
    int delta() const {return delta_;}

    void remove() {--selected_;}


protected:

    SSD130x &display_;
    const LinearFont &font_;
    bool init_ = true;

    int seq_;
    int8_t buttonState_[2];

    bool activated_ = false;
    int delta_;

    enum class Section : uint8_t {
        BEGIN,
        BODY,
        END
    };
    Section section_ = Section::END;

    // index of selected menu entry
    int selected_ = 0;

    // y coordinate of selected menu entry
    int selectedY_ = 0;

    // starting y coodinate of display
    int offsetY_ = 0;


    // index of current menu entry
    int entryIndex_ = 0;

    // y coordinate of current menu entry
    int entryY_ = 0;

    // index of selected element to edit, 0 when edit mode is off
    int editIndex_ = 0;


    // number of entries in the menu, captured after the last run
    int entryCount_;

    CoroutineTaskList<> dummyTasks_;
};


// underline stream token
template <typename A>
struct Underline {
    const A &a;
    bool on;
};

template <typename A>
Menu::Stream &operator <<(Menu::Stream &s, Underline<A> underline) {
    if (underline.on)
        s << Menu::Stream::Command::UNDERLINE_ON;
    s << underline.a;
    if (underline.on)
        s << Menu::Stream::Command::UNDERLINE_OFF;
    return s;
}

template <typename A>
constexpr Underline<A> underline(const A &a, bool on = true) {
    return {a, on};
}

} // namespace coco
