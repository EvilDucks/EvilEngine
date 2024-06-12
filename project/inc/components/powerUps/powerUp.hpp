//
// Created by Bartek on 11.06.2024.
//

#ifndef EVILENGINE_POWERUP_HPP
#define EVILENGINE_POWERUP_HPP

namespace POWER_UP {

    enum class PowerUpType {
        NONE,
        BOUNCE,
        SPEED
    };

    struct PowerUp {
        PowerUpType type;
        float duration = 10.f;
        float timeLeft;
    };

    struct Bounce {
        float strength = 5.f;
        float duration =  1.f;
    };

    struct Speed {
        float speedMultiplier = 2.f;
    };

}

#endif //EVILENGINE_POWERUP_HPP
