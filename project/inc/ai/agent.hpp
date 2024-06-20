#pragma once

namespace AGENT {

    enum StateType{Wait, WindUp, Active, Recharge, Inactive};

    //opening window trap
    struct WindowData {
        GameObjectID id = 0;
        GameObjectID colliderId = 0;
        GameObjectID transformId = 0;
        StateType type = Inactive;
        bool isActive = false;
        bool isRechargable = false;
        bool isTriggered = false;
        float timer = -1.f;
        //TEMP
        glm::vec3 newPos;
    };

    void ChangeState(WindowData& window, StateType newType)
    {
        window.type = newType;
    }

}