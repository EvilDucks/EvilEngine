//
// Created by user on 15.05.2024.
//

#ifndef EVILENGINE_UIMAPPINGS_HPP
#define EVILENGINE_UIMAPPINGS_HPP

#include "global.hpp"

namespace UI_MAP {
    void RegisterCallbacks(UI::MANAGER::UIM& manager) {
        UI::MANAGER::RegisterUICallback(manager, "exitButton", UI::MANAGER::UICallback{
                .Ref = "GameplayUI",
                .Func = [](unsigned int playerIndex, UI::ElementType elementType, int value) {

                    int index = UI::MANAGER::FindUIElementByName(GLOBAL::uiManager, elementType, "exitButton");
                    if (index > -1)
                    {
                        UI::BUTTON::Button button = GLOBAL::uiManager->buttons[index];
                        if (button.local.state != value)
                        {
//                            switch(value){
//                                case UI::BUTTON::DEFAULT_STATE:
//                                    DEBUG {spdlog::info("UI test button default");}
//                                    break;
//                                case UI::BUTTON::HOVERED_STATE:
//                                    DEBUG {spdlog::info("UI test button hover");}
//                                    break;
//                                case UI::BUTTON::PRESSED_STATE:
//                                    DEBUG {spdlog::info("UI test button pressed");}
//                                    break;
//                            }
                            button.local.state = value;
                            if (value == 2)
                            {
                                GLOBAL::exit = true;
                            }
                        }
                    }

                    return true;
                }
        });
    }
}





#endif //EVILENGINE_UIMAPPINGS_HPP
