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

    void HandleAllCheckpoints(Manager chM, TRANSFORM::LTransform* lTransforms, TRANSFORM::GTransform* gTransforms, TRANSFORM::LTransform& lTransform1, TRANSFORM::LTransform& lTransform2, TRANSFORM::GTransform& gTransform1, TRANSFORM::GTransform& gTransform2, RIGIDBODY::Rigidbody* rigidbodies)
    {
        glm::vec3 posToCheck{};
//        for( int i = 0; i < 2; i++ )
//        {
////            posToCheck = glm::vec3(gTransforms[chM.players[i].local.transformIndex][3]);
////            if(posToCheck.y < chM.checkpoints[chM.players[i].local.currentCheckpointIndex].position.y )
////            {
////                lTransforms[chM.players[i].local.transformIndex].base.position
////                    = chM.checkpoints[chM.players[i].local.currentCheckpointIndex].position;
////                lTransforms[chM.players[i].local.transformIndex].base.position.y += 1.0f;
////
////                TRANSFORM::ApplyDirtyFlagSingle(lTransforms[chM.players[i].local.transformIndex], gTransforms[chM.players[i].local.transformIndex]);
////            }
//        }

        posToCheck = glm::vec3(gTransform1[3]);
        if(posToCheck.y < chM.checkpoints[chM.players[0].local.currentCheckpointIndex].position.y )
        {
            lTransform1.base.position
                    = chM.checkpoints[chM.players[0].local.currentCheckpointIndex].position;
            lTransform1.base.position.y += 1.0f;
            rigidbodies[chM.players[0].local.rigidbodyIndex].base.velocity.y = 0.f;

            TRANSFORM::ApplyDirtyFlagSingle(lTransform1, gTransform1);
        }

        posToCheck = glm::vec3(gTransform2[3]);
        if(posToCheck.y < chM.checkpoints[chM.players[1].local.currentCheckpointIndex].position.y )
        {
            lTransform2.base.position
                    = chM.checkpoints[chM.players[1].local.currentCheckpointIndex].position;
            lTransform2.base.position.y += 1.0f;
            rigidbodies[chM.players[1].local.rigidbodyIndex].base.velocity.y = 0.f;

            TRANSFORM::ApplyDirtyFlagSingle(lTransform2, gTransform2);
        }
    }

}