//
// Created by Bartek on 11.06.2024.
//

#ifndef EVILENGINE_POWERUP_HPP
#define EVILENGINE_POWERUP_HPP

#include "effolkronium/random.hpp"
using Random = effolkronium::random_static;

namespace BOUNCE {

    struct BounceAnimation {
        u16 segmentIndex;
        u16 transformIndex;
        float totalTime = 0.f;
        glm::vec3 savedSize;

        float duration = 0.5f;
        float strength = 0.25f;
    };

    void CheckPreviousBounces(std::vector<BOUNCE::BounceAnimation>& bounces, BOUNCE::BounceAnimation& addedBounce)
    {
        for (int i = bounces.size() - 1; i >= 0; i--)
        {
            auto& bounce = bounces[i];
            if (bounce.transformIndex == addedBounce.transformIndex && bounce.segmentIndex == addedBounce.segmentIndex)
            {
                bounce.totalTime = bounce.duration;
            }
        }
    }
}

namespace POWER_UP::BOUNCE {
    float strength = 7.f;
    float bounceDuration =  0.75f;
    float duration = 10.f;
}

namespace POWER_UP::SPEED {
    float speedMultiplier = 1.5f;
    float duration = 10.f;
}

namespace POWER_UP::GHOST {
    float duration = 5.f;
}

namespace POWER_UP {

    enum class PowerUpType {
        NONE,
        BOUNCE,
        SPEED,
        GHOST
    };

    struct PowerUp {
        PowerUpType type;
        float duration = 10.f;
        float timeLeft;
    };

    PowerUp PickUpPowerUp ()
    {
        int type = Random::get(0, 2);
        PowerUp powerUp;
        switch (type)
        {
            case 0:
                DEBUG {spdlog::info("Power up type: BOUNCE");}
                powerUp.type = PowerUpType::BOUNCE;
                powerUp.duration = POWER_UP::BOUNCE::duration;
                return powerUp;
            case 1:
                DEBUG {spdlog::info("Power up type: SPEED");}
                powerUp.type = PowerUpType::SPEED;
                powerUp.duration = POWER_UP::SPEED::duration;
                return powerUp;
            case 2:
                DEBUG {spdlog::info("Power up type: GHOST");}
                powerUp.type = PowerUpType::GHOST;
                powerUp.duration = POWER_UP::GHOST::duration;
                return powerUp;
            default:

                return powerUp;
        }
    }

    GLuint PowerUpIcon (POWER_UP::PowerUpType type, GLuint t0, GLuint t1, GLuint t2, GLuint t3)
    {
        switch (type)
        {
            case PowerUpType::BOUNCE:
                return t1;
            case PowerUpType::SPEED:
                return t2;
            case PowerUpType::GHOST:
                return t3;
            case PowerUpType::NONE:
            default:
                return t0;
        }
    }

    char* PowerUpMassage (POWER_UP::PowerUpType type, bool gamepad, u8& textSize)
    {
        char* text;
        switch(type)
        {
            case PowerUpType::BOUNCE:
                if (gamepad)
                {
                    text = "Press TRIANGLE for\nbouncy platforms!";
                    textSize = 36;
                }
                else
                {
                    text = "Press Q for\nbouncy platforms!";
                    textSize = 29;
                }
                return text;
            case PowerUpType::SPEED:
                if (gamepad)
                {
                    text = "Press TRIANGLE for\nunstoppable speed!";
                    textSize = 38;
                }
                else
                {
                    text = "Press Q for\nunstoppable speed!";
                    textSize = 31;
                }
                return text;
            case PowerUpType::GHOST:
                if (gamepad)
                {
                    text = "Press TRIANGLE for\ntrap immunity!";
                    textSize = 34;
                }
                else
                {
                    text = "Press Q for\ntrap immunity!";
                    textSize = 27;
                }
                return text;
            default:
                text = "1";
                textSize = 1;
        }
        return text;
    }
}



#endif //EVILENGINE_POWERUP_HPP
