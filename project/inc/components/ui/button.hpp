//
// Created by Bartek on 14.05.2024.
//

#ifndef EVILENGINE_BUTTON_HPP
#define EVILENGINE_BUTTON_HPP

#include <iostream>
#include "ui.hpp"

namespace UI::BUTTON {

//    enum class STATE {
//        DEFAULT,
//        HOVERED,
//        PRESSED
//    };

    const int DEFAULT_STATE = 0;
    const int HOVERED_STATE = 1;
    const int PRESSED_STATE = 2;

    struct Position {
        int x = 0;
        int y = 0;
    };

    struct Base {
        std::string name;
        int state = DEFAULT_STATE;
        Position position;
        int width;
        int height;
        UI::ElementType elementType = UI::ElementType::BUTTON;
        std::string buttonText;
    };

    struct Button {
        u16 id;
        Base local;
    };
}

#endif //EVILENGINE_BUTTON_HPP
