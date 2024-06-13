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
        switch (type)
        {
            case 0:
                DEBUG {spdlog::info("Power up type: BOUNCE");}
                return PowerUpType::BOUNCE;
            case 1:
                DEBUG {spdlog::info("Power up type: SPEED");}
                return PowerUpType::SPEED;
            default:
                return PowerUpType::NONE;
        }
    }
}

namespace POWER_UP::BOUNCE {
    float strength = 7.f;
    float duration =  0.75f;
}

namespace POWER_UP::SPEED {
    float speedMultiplier = 1.5f;
}

#endif //EVILENGINE_POWERUP_HPP
