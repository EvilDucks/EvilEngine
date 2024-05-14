//
// Created by Bartek on 14.05.2024.
//

#ifndef EVILENGINE_UIMANAGER_HPP
#define EVILENGINE_UIMANAGER_HPP

#include "button.hpp"
#include <vector>
#include <functional>

namespace UI::MANAGER {

    struct UIManager {
        std::vector<UI::BUTTON::Button> buttons;
    };
    using UIM = UIManager*;

    using ButtonCallbackFunc = std::function<bool(unsigned int, unsigned int)>; // player index, button state

    struct ButtonCallback {
        std::string Ref;
        ButtonCallbackFunc Func;
    };

    void LoadCanvas(UI::MANAGER::UIM manager);
    void RegisterButtonCallback (UI::MANAGER::UIM manager, const std::string& actionName, const ButtonCallback& callback);
}


#endif //EVILENGINE_UIMANAGER_HPP
