#include "Menu.hpp"


namespace coco {

bool Menu::begin(InputDevice &buttons) {
    if (init_) {
        init_ = false;
        seq_ = buttons.get(buttonState_);
        delta_ = 0;
    } else {
        // get button state
        int8_t buttonState[2];
        seq_ = buttons.get(buttonState);

        // detect change of up/down button or rotary knob
        int delta = int8_t(buttonState[0] - buttonState_[0]);
        if (delta != 0) {
            buttonState_[0] = buttonState[0];

            // move selection
            if (editIndex_ == 0) {
                int selected = selected_ + delta; // up decrements selected entry
                if (selected < 0) {
                    selected = 0;

                    // also clear yOffset in case the menu has a non-selectable header
                    offsetY_ = 0;
                } else if (selected >= entryCount_) {
                    selected = entryCount_ - 1;
                }
                selected_ = selected;
            }
        }

        // set delta for edit mode
        delta_ = delta;

        // detect change of right/left button or button of rotary button
        int x = int8_t(buttonState[1] - buttonState_[1]);
        if (x != 0) {
            buttonState_[1] = buttonState[1];
            if (x > 0) {
                // activate
                activated_ = true;
            } else {
                // exit edit or menu
                if (editIndex_ > 0)
                    --editIndex_;
                else
                    return true;
            }
        }
    }

    // clear bitmap
    display_.bitmap().clear();
    return false;
}

Awaitable<Buffer::Events> Menu::show() {
    section_ = Section::END;

    // need redraw if an entry was selected
    bool redraw = init_;

    // adjust yOffset so that selected entry is visible
    const int lineHeight = font_.height + 4;
    int upper = selectedY_;
    int lower = upper + lineHeight;
    if (upper < offsetY_) {
        offsetY_ = upper;
        redraw = true;
    }
    if (lower > offsetY_ + display_.height()) {
        offsetY_ = lower - display_.height();
        redraw = true;
    }

    // capture number of entries in menu
    entryCount_ = entryIndex_;

    // clear for next menu drawing
    entryIndex_ = 0;
    entryY_ = 0;
    activated_ = false;

    if (redraw) {
        // prevent waiting for new input
        --seq_;

        // don't draw and continue
        return {};
    } else {
        // show bitmap on display (user needs to co_await before calling begin() again)
        return display_.show();
    }
}


void Menu::line() {
    int x = 10;
    int y = entryY_ + 2 - offsetY_;
    display_.bitmap().fillRectangle(x, y, 108, 1);
    entryY_ += 1 + 4;
}

void Menu::beginSection() {
    if (section_ != Section::END)
        section_ = Section::BEGIN;
}

void Menu::endSection() {
    if (section_ == Section::BODY)
        line();
    section_ = Section::END;
}

Menu::Stream Menu::stream() {
    if (section_ == Section::BEGIN)
        line();
    section_ = Section::BODY;

    return {display_.bitmap(), font_, 10, entryY_ + 2 - offsetY_};
}

void Menu::label() {
    entryY_ += font_.height + 4;
}

bool Menu::entry() {
    const int lineHeight = font_.height + 4;
    int y = entryY_ + 2 - offsetY_;

    bool selected = entryIndex_ == selected_;
    if (selected) {
        if (editIndex_ == 0)
            display_.bitmap().drawText(0, y, font_, ">");
        selectedY_ = entryY_;
    }

    ++entryIndex_;
    entryY_ += lineHeight;

    // check if this menu entry was activated
    bool activated = selected && activated_;
    if (activated) {
        // yes: force redraw because something may get changed or a sub menu is entered
        init_ = true;
    }

    return activated;
}

int Menu::edit(int editCount) {
    // check if the next entry is selected
    if (selected_ == entryIndex_) {
        // cycle edit index if activated (e.g. button pressed)
        if (activated_) {
            ++editIndex_;

            // "consume" activation
            activated_ = false;
        }
        if (editIndex_ > editCount)
            editIndex_ = 0;
        return editIndex_;
    }
    return 0;
}

} // namespace coco
