//
// Created by Bartek on 11.06.2024.
//

#ifndef EVILENGINE_POWERUP_HPP
#define EVILENGINE_POWERUP_HPP

#include "effolkronium/random.hpp"
using Random = effolkronium::random_static;

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

    PowerUpType PickUpPowerUp ()
    {
        int type = Random::get(0, 1);
        switch (0)
        {
            case 0:
                return PowerUpType::BOUNCE;
            case 1:
                return PowerUpType::SPEED;
            default:
                return PowerUpType::NONE;
        }
    }
}

namespace POWER_UP::BOUNCE {
    float strength = 5.f;
    float duration =  1.f;
}

namespace POWER_UP::SPEED {
    float speedMultiplier = 2.f;
}

#endif //EVILENGINE_POWERUP_HPP
