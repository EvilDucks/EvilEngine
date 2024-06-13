#pragma once

#include <iostream>
#include "glm/mat4x4.hpp"


#include "player/player.hpp"
#include "checkpoint.hpp"
namespace CHECKPOINT::MANAGER
{
    struct Manager{
        PLAYER::Player* players;
        CHECKPOINT::Checkpoint* checkpoints;
    };

    void HandleAllCheckpoints(Manager chM, TRANSFORM::LTransform* lTransforms, TRANSFORM::GTransform* gTransforms)
    {
        glm::vec3 posToCheck{};
        for( int i = 0; i < 2; i++ )
        {
            posToCheck = glm::vec3(gTransforms[chM.players[i].local.transformIndex][3]);
            if(posToCheck.y < chM.checkpoints[chM.players[i].local.currentCheckpointIndex].position.y )
            {
                lTransforms[chM.players[i].local.transformIndex].base.position
                    = chM.checkpoints[chM.players[i].local.currentCheckpointIndex].position;
                lTransforms[chM.players[i].local.transformIndex].base.position.y += 1.0f;

                TRANSFORM::ApplyDirtyFlagSingle(lTransforms[chM.players[i].local.transformIndex], gTransforms[chM.players[i].local.transformIndex]);
            }
        }
    }

}