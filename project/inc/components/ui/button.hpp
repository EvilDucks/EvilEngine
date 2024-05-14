//
// Created by Bartek on 14.05.2024.
//

#ifndef EVILENGINE_BUTTON_HPP
#define EVILENGINE_BUTTON_HPP

#include <iostream>

namespace UI::BUTTON {

    const int DEFAULT_STATE = 0;
    const int HOVERED_STATE = 1;
    const int PRESSED_STATE = 2;

    struct Button {
        unsigned int state = 0;
        std::string name;
    };
}

#endif //EVILENGINE_BUTTON_HPP
