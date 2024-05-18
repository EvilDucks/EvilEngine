//
// Created by Bartek on 14.05.2024.
//

#ifndef EVILENGINE_UIMANAGER_HPP
#define EVILENGINE_UIMANAGER_HPP

#include "button.hpp"
#include "types.hpp"
#include <vector>
#include <functional>

namespace UI::MANAGER {

    using UICallbackFunc = std::function<bool(unsigned int, UI::ElementType type, int value)>; // player index, element type, value

    struct UIEvent {
        std::string eventName; // event name = element name
        unsigned int playerIndex;
        UI::ElementType elementType;
        int value;
    };

    struct UICallback {
        std::string Ref;
        UICallbackFunc Func;
    };

    struct UIManager {
        UI::BUTTON::Button* buttons;
        u16 buttonsCount;
        std::unordered_map<std::string, std::vector<UICallback>> _uiCallbacks {};
        int currentHoverIndex = -1;
        UI::ElementType currentHoverType = UI::ElementType::UNKNOWN;
    };
    using UIM = UIManager*;

    void LoadCanvas(UI::MANAGER::UIM manager);
    void RegisterUICallback (UI::MANAGER::UIM manager, const std::string& elementName, const UICallback& callback);
    void PropagateUIEvent (UI::MANAGER::UIM manager, UIEvent event);
    int FindUIElementByName(UI::MANAGER::UIM manager, UI::ElementType elementType, std::string elementName);

    void RegisterUICallback (UI::MANAGER::UIM manager, const std::string& elementName, const UICallback& callback)
    {
        manager->_uiCallbacks[elementName].emplace_back(callback);
    }

    void PropagateUIEvent (UI::MANAGER::UIM manager, UI::MANAGER::UIEvent event)
    {
        for (size_t i = manager->_uiCallbacks[event.eventName].size() - 1; i >= 0; i--) {
            auto& uiCallback = manager->_uiCallbacks[event.eventName][i];

            if (uiCallback.Func(event.playerIndex, event.elementType, event.value)) break;
        }
    }

    int FindUIElementByName(UI::MANAGER::UIM manager, UI::ElementType elementType, std::string elementName)
    {
        switch (elementType){
            case UI::ElementType::BUTTON:
                for (int i = 0; i < manager->buttonsCount; i++)
                {
                    if (manager->buttons[i].local.name == elementName)
                        return i;
                }
                break;
            default:
                return -1;
        }
        return -1;
    }
}


#endif //EVILENGINE_UIMANAGER_HPP
