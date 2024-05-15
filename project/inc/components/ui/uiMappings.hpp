//
// Created by user on 15.05.2024.
//

#ifndef EVILENGINE_UIMAPPINGS_HPP
#define EVILENGINE_UIMAPPINGS_HPP

#include "global.hpp"

namespace UI_MAP {
    void RegisterCallbacks(UI::MANAGER::UIM manager) {
        UI::MANAGER::RegisterUICallback(manager, "testButton", UI::MANAGER::UICallback{
                .Ref = "GameplayUI",
                .Func = [](unsigned int playerIndex, UI::ElementType elementType, int value) {

                    int index = UI::MANAGER::FindUIElementByName(GLOBAL::uiManager, elementType, "testButton");
                    if (index > -1)
                    {
                        UI::BUTTON::Button button = GLOBAL::uiManager->buttons[index];
                        if (button.base.state != value)
                        {
                            switch(value){
                                case UI::BUTTON::DEFAULT_STATE:
                                    std::cout << "default" << std::endl;
                                    break;
                                case UI::BUTTON::HOVERED_STATE:
                                    std::cout << "hovered" << std::endl;
                                    break;
                                case UI::BUTTON::PRESSED_STATE:
                                    std::cout << "pressed" << std::endl;
                                    break;
                            }
                            button.base.state = value;
                        }
                    }

                    return true;
                }
        });
    }
}





#endif //EVILENGINE_UIMAPPINGS_HPP
