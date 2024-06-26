#pragma once

#include "global.hpp"

namespace UPDATE {

	void Collisions () {
        PROFILER { ZoneScopedN("GLOBAL: Collisions"); }

        CheckOBBCollisions(COLLIDER::ColliderGroup::PLAYER, COLLIDER::ColliderGroup::MAP, GLOBAL::scene.world->colliders, GLOBAL::scene.world->collidersCount);

        CheckOBBCollisions(COLLIDER::ColliderGroup::PLAYER, COLLIDER::ColliderGroup::TRIGGER, GLOBAL::scene.world->colliders, GLOBAL::scene.world->collidersCount);

        CheckOBBCollisionsSingleGroup(COLLIDER::ColliderGroup::PLAYER, GLOBAL::scene.world->colliders, GLOBAL::scene.world->collidersCount);

        auto& triggersCount = GLOBAL::scene.world->collidersCount[COLLIDER::ColliderGroup::TRIGGER]; 
        auto& triggers = GLOBAL::scene.world->colliders[COLLIDER::ColliderGroup::TRIGGER];
        auto& playersCount = GLOBAL::scene.world->playersCount;
        auto& players = GLOBAL::scene.world->players;

        for (int i = 0; i < playersCount; i++)
        {
            PLAYER::HandlePlayerCollisions(
                players[i], GLOBAL::world.colliders, GLOBAL::world.collidersCount, 
                GLOBAL::world.lTransforms, GLOBAL::world.gTransforms, GLOBAL::world.transformsCount, 
                GLOBAL::world.rigidbodies, players[(i + 1) % 2], GLOBAL::activePowerUp.type,
                MANAGER::OBJECTS::GLTF::worlds[i*2].lTransforms[0],
                MANAGER::OBJECTS::GLTF::worlds[((i+1)*2)%4].lTransforms[0],
                GLOBAL::bounces
            );
        }

        // Before triggers, we need to reset checkpointInRange value in players
        for(int i = 0; i < playersCount; i++)
        {
            // reset the value to make "trigger exit"
            players[i].local.checkPointInRange = -1;
        }
        //and also window trap triggers
        for(int i = 0; i < GLOBAL::world.windowTrapCount; i++)
        {
            GLOBAL::world.windowTraps[i].isTriggered = false;
        }


        for (int i = 0; i < triggersCount; i++)
        {
            COLLISION::MANAGER::HandleTriggerCollisions(
                    GLOBAL::collisionManager, triggers[i],
                    GLOBAL::scene.world->colliders, GLOBAL::scene.world->collidersCount
            );
        }
    }


	void UICollisions ()
    {
//        for (int i = 0; i < GLOBAL::playerCount; i++)
//        {
//            CheckUICollisions(GLOBAL::canvas.colliders[COLLIDER::ColliderGroup::UI], GLOBAL::canvas.collidersCount[COLLIDER::ColliderGroup::UI], GLOBAL::players[i].local.selection.x, GLOBAL::players[i].local.selection.y, GLOBAL::canvas.buttons, GLOBAL::canvas.buttonsCount, GLOBAL::uiManager);
//        }

        auto& players = GLOBAL::scene.world->players;

        //for (int i = 0; i < GLOBAL::playerCount; i++)
        {
            CheckUICollisions(GLOBAL::canvas.colliders[COLLIDER::ColliderGroup::UI], GLOBAL::canvas.collidersCount[COLLIDER::ColliderGroup::UI], players[0].local.selection.x, players[0].local.selection.y, GLOBAL::canvas.buttons, GLOBAL::canvas.buttonsCount, GLOBAL::uiManager);
        }
    }

    void UpdateColliders ()
    {
        auto& playersCount = GLOBAL::scene.world->playersCount;
        auto& players = GLOBAL::scene.world->players;

        for (int i = 0; i < playersCount; i++)
        {
            //COLLIDER::UpdateColliderTransform(GLOBAL::world.colliders[players[i].local.colliderGroup][players[i].local.colliderIndex], GLOBAL::world.gTransforms[players[i].local.transformIndex]);
            COLLIDER::UpdateColliderTransform(GLOBAL::world.colliders[players[i].local.colliderGroup][players[i].local.colliderIndex], MANAGER::OBJECTS::GLTF::worlds[i*2].gTransforms[0]);
        }

        auto& movingPlatformsCount = GLOBAL::scene.world->movingPlatformsCount;
        auto& movingPlatforms = GLOBAL::scene.world->movingPlatforms;

        for (int i = 0; i < movingPlatformsCount; i++)
        {
            COLLIDER::UpdateColliderTransform(GLOBAL::world.colliders[COLLIDER::ColliderGroup::MAP][movingPlatforms[i].base.mapColliderIndex], GLOBAL::world.gTransforms[movingPlatforms[i].base.transformIndex]);
            COLLIDER::UpdateColliderTransform(GLOBAL::world.colliders[COLLIDER::ColliderGroup::TRIGGER][movingPlatforms[i].base.triggerColliderIndex], GLOBAL::world.gTransforms[movingPlatforms[i].base.transformIndex]);
        }
    }

    void MovePlayers ()
    {
        auto& playersCount = GLOBAL::scene.world->playersCount;
        auto& players = GLOBAL::scene.world->players;

        for (int i = 0; i < playersCount; i++)
        {
            PLAYER::MOVEMENT::Move(players[i], GLOBAL::world.rigidbodies, GLOBAL::timeDelta, GLOBAL::activePowerUp.type, MANAGER::OBJECTS::GLTF::worlds[i*2].lTransforms[0]);
        }
    }

    void MoveRigidbodies ()
    {
        for (int i = 0; i < GLOBAL::world.rigidbodiesCount; i++)
        {
            RIGIDBODY::Move(GLOBAL::world.rigidbodies[i], GLOBAL::world.lTransforms, GLOBAL::world.gTransforms, float(GLOBAL::timeDelta),MANAGER::OBJECTS::GLTF::worlds[i*2].lTransforms[0], MANAGER::OBJECTS::GLTF::worlds[i*2].gTransforms[0]);
        }
    }

    void UpdatePowerUp ()
    {
        auto& powerUp = GLOBAL::activePowerUp;
        if (powerUp.type != POWER_UP::PowerUpType::NONE)
        {
            powerUp.timeLeft -= GLOBAL::timeDelta;
            if (powerUp.timeLeft <= 0)
            {
                if (powerUp.type == POWER_UP::PowerUpType::SPEED)
                {
                    for (int i = 0; i < GLOBAL::world.playersCount; i++)
                    {
                        GLOBAL::world.players[i].local.movement.playerSpeed /= POWER_UP::SPEED::speedMultiplier;
                        GLOBAL::world.rigidbodies[GLOBAL::world.players[i].local.rigidbodyIndex].base.movementSpeed = GLOBAL::world.players[i].local.movement.playerSpeed;
                        CalculateGravitation(GLOBAL::world.players[i], GLOBAL::world.rigidbodies);
                    }
                }
                else if (powerUp.type == POWER_UP::PowerUpType::GHOST)
                {
                    for (int i = 0; i < GLOBAL::world.playersCount; i++)
                    {
                        GLOBAL::world.players[i].local.movement.ghostForm = false;
                    }
                }
                powerUp.type = POWER_UP::PowerUpType::NONE;
                DEBUG spdlog::info("Power up end");

            }
        }
    }

    void AnimateColliderObjects ()
    {
        for (int i = 0; i < GLOBAL::world.collidersCount[COLLIDER::ColliderGroup::TRIGGER]; i++)
        {
            auto& collider = GLOBAL::world.colliders[COLLIDER::ColliderGroup::TRIGGER][i];
            if (collider.local.isEnabled)
            {
                // Spring trap standby animation
                if (collider.local.collisionEventName == "SpringTrap")
                {
                    float yOffset = 0.005f;
                    float yOffsetSpeed = 5.f;
                    MANAGER::SCENES::GENERATOR::segmentsWorld[collider.local.segmentIndex].lTransforms[collider.local.transformIndex].base.position.y
                    += yOffset * sin(GLOBAL::timeCurrent * yOffsetSpeed);
                    MANAGER::SCENES::GENERATOR::segmentsWorld[collider.local.segmentIndex].lTransforms[collider.local.transformIndex].flags = TRANSFORM::DIRTY;
                }
                // Power-up standby animation
                else if (collider.local.collisionEventName == "PowerUp")
                {
                    float yOffset = 0.01f;
                    float yOffsetSpeed = 2.5f;
                    float yRotationSpeed = 30.f;
                    MANAGER::SCENES::GENERATOR::segmentsWorld[collider.local.segmentIndex].lTransforms[collider.local.transformIndex].base.position.y += yOffset * sin(GLOBAL::timeCurrent * yOffsetSpeed);
                    MANAGER::SCENES::GENERATOR::segmentsWorld[collider.local.segmentIndex].lTransforms[collider.local.transformIndex].base.rotation.y += yRotationSpeed * GLOBAL::timeDelta;
                    MANAGER::SCENES::GENERATOR::segmentsWorld[collider.local.segmentIndex].lTransforms[collider.local.transformIndex].flags = TRANSFORM::DIRTY;
                }
            }
            else
            {
                // Activated spring trap animation
                if (collider.local.collisionEventName == "SpringTrap")
                {
                    if (collider.local.timer > 0.f)
                    {
                        float yOffset = 0.3f;
                        float bounceOffset = 0.05f;
                        float bounceSpeed = 30.f;
                        MANAGER::SCENES::GENERATOR::segmentsWorld[collider.local.segmentIndex].lTransforms[collider.local.transformIndex].base.position.y
                                += bounceOffset * sin(GLOBAL::timeCurrent * bounceSpeed);
                        MANAGER::SCENES::GENERATOR::segmentsWorld[collider.local.segmentIndex].lTransforms[collider.local.transformIndex].base.position.y
                                += yOffset * GLOBAL::timeDelta * collider.local.timer;
                        MANAGER::SCENES::GENERATOR::segmentsWorld[collider.local.segmentIndex].lTransforms[collider.local.transformIndex].flags = TRANSFORM::DIRTY;
                        collider.local.timer -= GLOBAL::timeDelta;
                    }
                    else
                    {
                        MANAGER::SCENES::GENERATOR::segmentsWorld[collider.local.segmentIndex].lTransforms[collider.local.transformIndex].base.position.y = -1000.f;
                        MANAGER::SCENES::GENERATOR::segmentsWorld[collider.local.segmentIndex].lTransforms[collider.local.transformIndex].flags = TRANSFORM::DIRTY;
                    }
                }
            }
        }
    }

    void AnimateBounces ()
    {
        for (int i = GLOBAL::bounces.size()-1; i >= 0; i--)
        {
            auto& bounce = GLOBAL::bounces[i];
            auto& transform = MANAGER::SCENES::GENERATOR::segmentsWorld[bounce.segmentIndex].lTransforms[bounce.transformIndex];
            if (bounce.totalTime != 0.f)
            {
                if (bounce.totalTime >= bounce.duration)
                {
                    transform.base.scale = bounce.savedSize;
                    GLOBAL::bounces.erase(GLOBAL::bounces.begin() + i);
                }
                else
                {
                    float step = bounce.strength * sin(bounce.totalTime*glm::pi<float>()/bounce.duration);
                    transform.base.scale = bounce.savedSize * (1-step);
                    transform.flags = TRANSFORM::DIRTY;
                    bounce.totalTime += GLOBAL::timeDelta;
                }
            }
        }

        for (int i = GLOBAL::bounces.size()-1; i >= 0; i--)
        {
            auto &bounce = GLOBAL::bounces[i];
            auto &transform = MANAGER::SCENES::GENERATOR::segmentsWorld[bounce.segmentIndex].lTransforms[bounce.transformIndex];
            if (bounce.totalTime == 0.f) {
                bounce.savedSize = transform.base.scale;
                bounce.totalTime += GLOBAL::timeDelta;
            }
        }
    }

    void UpdatePopups ()
    {
        if (GLOBAL::jumpPopupTimer > 0)
        {
            GLOBAL::jumpPopupTimer -= GLOBAL::timeDelta;
        }

        GLOBAL::checkpointPopup[0] = false;
        GLOBAL::checkpointPopup[1] = false;
    }

    void MovePlatforms ()
    {
        for (int i = 0; i < GLOBAL::world.movingPlatformsCount; i++)
        {
            u64 transformIndex = 0;
            OBJECT::GetComponentFast<TRANSFORM::LTransform>(transformIndex, GLOBAL::world.transformsCount,
                                                            GLOBAL::world.lTransforms, GLOBAL::world.movingPlatforms[i].id);
            MOVING_PLATFORM::MovePlatform(GLOBAL::world.movingPlatforms[i], GLOBAL::world.lTransforms[transformIndex], GLOBAL::timeDelta);

            TRANSFORM::ApplyDirtyFlagSingle(GLOBAL::world.lTransforms[transformIndex], GLOBAL::world.gTransforms[transformIndex]);

//            u64 colliderIndex = 0;
//            OBJECT::GetComponentFast<COLLIDER::Collider>(colliderIndex, GLOBAL::world.collidersCount[COLLIDER::ColliderGroup::MAP],
//                                                            GLOBAL::world.colliders[COLLIDER::ColliderGroup::MAP], GLOBAL::world.movingPlatforms[i].id);
//            COLLIDER::UpdateColliderTransform(GLOBAL::world.colliders[COLLIDER::ColliderGroup::MAP][colliderIndex], GLOBAL::world.gTransforms[transformIndex]);
//
//            OBJECT::GetComponentFast<COLLIDER::Collider>(colliderIndex, GLOBAL::world.collidersCount[COLLIDER::ColliderGroup::TRIGGER],
//                                                         GLOBAL::world.colliders[COLLIDER::ColliderGroup::TRIGGER], GLOBAL::world.movingPlatforms[i].id);
//            COLLIDER::UpdateColliderTransform(GLOBAL::world.colliders[COLLIDER::ColliderGroup::TRIGGER][colliderIndex], GLOBAL::world.gTransforms[transformIndex]);
        }
    }

    void UpdateDistances()
    {
//        auto& player1position = GLOBAL::world.lTransforms[GLOBAL::world.players[0].local.transformIndex].base.position;
//        auto& player2position = GLOBAL::world.lTransforms[GLOBAL::world.players[1].local.transformIndex].base.position;
        auto& player1position = MANAGER::OBJECTS::GLTF::worlds[0].lTransforms[0].base.position;
        auto& player2position = MANAGER::OBJECTS::GLTF::worlds[2].lTransforms[0].base.position;

        u64 transformIndex = 0;
        OBJECT::GetComponentFast<TRANSFORM::LTransform>(transformIndex, GLOBAL::world.transformsCount,
                                                        GLOBAL::world.lTransforms, 8);
        glm::vec3 goalPosition = GLOBAL::world.lTransforms[transformIndex].base.position;

        GLOBAL::goalDistances[0] = goalPosition.y - player1position.y + 1.f;
        GLOBAL::goalDistances[1] = goalPosition.y - player2position.y + 1.f;

        float towerHeight = MANAGER::SCENES::GENERATOR::mapGenerator->modifiers.levelLength * 24.f + 0.5f;
        float towerMinimapHeight =  GLOBAL::canvas.lRectangles[3].base.size.y * GLOBAL::canvas.lRectangles[3].base.scale.y;

//        GLOBAL::minimapDistances[0] = (towerHeight - GLOBAL::goalDistances[0]) / towerHeight * towerMinimapHeight;
//
//        GLOBAL::minimapDistances[1] = (towerHeight - GLOBAL::goalDistances[1]) / towerHeight * towerMinimapHeight;

        GLOBAL::canvas.lRectangles[4].base.position.y = (towerHeight - GLOBAL::goalDistances[0]) / towerHeight * towerMinimapHeight;

        GLOBAL::canvas.lRectangles[5].base.position.y = (towerHeight - GLOBAL::goalDistances[1]) / towerHeight * towerMinimapHeight;

        if (GLOBAL::canvas.lRectangles[4].base.position.y > GLOBAL::canvas.lRectangles[5].base.position.y)
        {
            GLOBAL::canvas.lRectangles[4].base.scale = glm::vec2(1.25f);
            GLOBAL::canvas.lRectangles[5].base.scale = glm::vec2(1.f);
        }
        else
        {
            GLOBAL::canvas.lRectangles[4].base.scale = glm::vec2(1.f);
            GLOBAL::canvas.lRectangles[5].base.scale = glm::vec2(1.25f);
        }

        GLOBAL::playersDistance = glm::distance(player1position, player2position);
    }

    void UpdateButtons ()
    {
        for (int i = 0; i < GLOBAL::canvas.buttonsCount; i++)
        {
            u64 rectangleIndex = OBJECT::ID_DEFAULT;
            OBJECT::GetComponentFast<RECTANGLE::LRectangle>(rectangleIndex, GLOBAL::canvas.rectanglesCount, GLOBAL::canvas.lRectangles, GLOBAL::canvas.buttons[i].id);
            GLOBAL::canvas.lRectangles[rectangleIndex].base.scale = glm::vec2(1.f);
            GLOBAL::canvas.lRectangles[rectangleIndex].base.position.x = -132.f;
            if (GLOBAL::canvas.buttons[i].local.state == UI::BUTTON::HOVERED_STATE)
            {
                GLOBAL::canvas.lRectangles[rectangleIndex].base.scale = glm::vec2(1.25f);
                GLOBAL::canvas.lRectangles[rectangleIndex].base.position.x = -165.f;
            }
        }
    }

	void World (
		const SCENE::SHARED::World& sharedWorld, 
		const SCENE::World& world
	) {
		PROFILER { ZoneScopedN("Update: World"); }

		auto& rotatingsCount = world.rotatingsCount;
		auto& transforms = world.lTransforms;

		// Rotating component LOGIC.
		for (u16 iRotating = 0; iRotating < rotatingsCount; ++iRotating) {
			auto& rotatingComponent = world.rotatings[iRotating];
			auto& rotating = rotatingComponent.base;
			auto& id = rotatingComponent.id;

			auto& transform = transforms[id];
			transform.base.rotation += rotating; 
			transform.flags = TRANSFORM::DIRTY;
		}

		TRANSFORM::ApplyDirtyFlagEx (
			world.parenthoodsCount, world.parenthoods,
			world.transformsCount, world.lTransforms, world.gTransforms
		);

	}

	void Canvas (
		const SCENE::SHARED::Canvas& sharedCanvas, 
		const SCENE::Canvas& canvas
	) {
		PROFILER { ZoneScopedN("Update: Canvas"); }

		const float shift = GLOBAL::timeCurrent * 0.25f;
		SHADER::UNIFORM::BUFFORS::shift = { shift, shift };
		
		{ // Recalculating Time Variables.
			GLOBAL::timeCurrent = glfwGetTime();
			GLOBAL::timeDelta = GLOBAL::timeCurrent - GLOBAL::timeSinceLastFrame;
			GLOBAL::timeSinceLastFrame = GLOBAL::timeCurrent;

			{ // For each animation loop?
				ANIMATION::Update (GLOBAL::sharedAnimation1, GLOBAL::timeDelta);
			}
		}
	}

    void Checkpoints()
    {
        CHECKPOINT::MANAGER::HandleAllCheckpoints(GLOBAL::checkpointManager, GLOBAL::world.lTransforms, GLOBAL::world.gTransforms, MANAGER::OBJECTS::GLTF::worlds[0].lTransforms[0], MANAGER::OBJECTS::GLTF::worlds[2].lTransforms[0], MANAGER::OBJECTS::GLTF::worlds[0].gTransforms[0], MANAGER::OBJECTS::GLTF::worlds[2].gTransforms[0], GLOBAL::world.rigidbodies);
    }

    void UpdateState(AGENT::WindowData& window)
    {
        switch ( window.type ) {
            case AGENT::StateType::Wait :
                if(window.isTriggered)
                {
                    window.timer = (float)GLOBAL::timeCurrent;
                    ChangeState(window, AGENT::StateType::WindUp);
                    if(window.isFrontAxis)
                    {
                        window.newRot = window.rotAxis - 0.1f * window.rotAxis; //hardcode
                    }
                    else
                    {
                        window.newRot = window.rotAxis + 0.1f * window.rotAxis; //hardcode
                    }

                }
                break;
            case AGENT::StateType::WindUp :
                if(GLOBAL::timeCurrent >= window.timer+GLOBAL::windowTrapWindUpTime)
                {
                    window.timer = (float)GLOBAL::timeCurrent;
                    ChangeState(window, AGENT::StateType::Active);
                    if(window.isFrontAxis)
                    {
                        window.newRot = {0, 0, 0}; //hardcode
                    }
                    else
                    {
                        window.newRot = window.rotAxis*2.0f; //hardcode
                    }

                    window.applyKnockback = true;
                }
                break;
            case AGENT::StateType::Active :
                if(GLOBAL::timeCurrent >= window.timer+GLOBAL::windowTrapActiveTime)
                {
                    window.timer = (float)GLOBAL::timeCurrent;
                    ChangeState(window, AGENT::StateType::Recharge);
                    window.newRot = window.rotAxis; //hardcode
                    window.applyKnockback = false;
                }
                break;
            case AGENT::StateType::Recharge:
                if(GLOBAL::timeCurrent >= window.timer+GLOBAL::windowTrapRechargeTime)
                {
                    window.timer = -1;
                    ChangeState(window, AGENT::StateType::Inactive);
                }
                break;
            case AGENT::StateType::Inactive :
                if(window.isActive && window.isRechargable)
                {
                    ChangeState(window, AGENT::StateType::Wait);
                }
                break;
            default :
                DEBUG spdlog::error ("Wrong state in window statemachine.\n");
                break;
        }

        if(GLOBAL::world.lTransforms[window.parentId].base.rotation != window.newRot)
        {
            GLOBAL::world.lTransforms[window.parentId].base.rotation += (window.newRot-GLOBAL::world.lTransforms[window.parentId].base.rotation)*0.05f;
            GLOBAL::world.lTransforms[window.parentId].flags = TRANSFORM::DIRTY;
            TRANSFORM::ApplyDirtyFlagEx(GLOBAL::world.parenthoodsCount, GLOBAL::world.parenthoods, GLOBAL::world.transformsCount, GLOBAL::world.lTransforms, GLOBAL::world.gTransforms);
            COLLIDER::UpdateColliderTransform(GLOBAL::world.colliders[COLLIDER::ColliderGroup::MAP][window.colliderId], GLOBAL::world.gTransforms[window.transformId]);
            COLLIDER::UpdateColliderTransform(GLOBAL::world.colliders[COLLIDER::ColliderGroup::TRIGGER][window.knockbackTriggerId], GLOBAL::world.gTransforms[window.transformId]);
        }

    }

    void StateMachine()
    {
        for(int i = 0; i < GLOBAL::world.windowTrapCount; i++)
        {
            UpdateState(GLOBAL::world.windowTraps[i]);
        }
    }
}