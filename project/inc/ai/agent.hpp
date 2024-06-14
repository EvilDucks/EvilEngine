#pragma once

namespace AGENT {

    enum StateType{Wait, WindUp, Active, Recharge, Inactive};

    //opening window trap
    struct WindowData{
        StateType type = Inactive;
        bool isActive = false;
        bool isRechargable = false;
        bool isTriggered = false;
        float timer = -1.f;
    };

    void ChangeState(WindowData& window, StateType newType)
    {
        window.type = newType;
    }

}