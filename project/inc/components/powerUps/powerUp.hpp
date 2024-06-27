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
    float speedMultiplier = 1.25f;
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

    const char* PowerUpMessage (POWER_UP::PowerUpType type, bool gamepad, u8& textSize)
    {
        switch(type)
        {
            case PowerUpType::BOUNCE:
                if (gamepad)
                {
                    textSize = 36;
                    const char* msg = "Press TRIANGLE for\nbouncy platforms!";
                    return msg;
                }
                else
                {
                    textSize = 29;
                    const char* msg = "Press Q for\nbouncy platforms!";
                    return msg;
                }
            case PowerUpType::SPEED:
                if (gamepad)
                {
                    textSize = 38;
                    const char* msg = "Press TRIANGLE for\nunstoppable speed!";
                    return msg;
                }
                else
                {
                    textSize = 31;
                    const char* msg = "Press Q for\nunstoppable speed!";
                    return msg;
                }
            case PowerUpType::GHOST:
                if (gamepad)
                {
                    textSize = 34;
                    const char* msg = "Press TRIANGLE for\ntrap immunity!";
                    return msg;
                }
                else
                {
                    textSize = 27;
                    const char* msg = "Press Q for\ntrap immunity!";
                    return msg;
                }
            default: 
                textSize = 1;
                const char* msg = "1";
                return msg;
        }
    }

    float PowerUpDuration(POWER_UP::PowerUpType type)
    {
        switch(type)
        {
            case PowerUpType::BOUNCE:
                return POWER_UP::BOUNCE::duration;
            case PowerUpType::SPEED:
                return POWER_UP::SPEED::duration;
            case PowerUpType::GHOST:
                return POWER_UP::GHOST::duration;
            default:
                return 1.f;
        }
    }
}



#endif //EVILENGINE_POWERUP_HPP
