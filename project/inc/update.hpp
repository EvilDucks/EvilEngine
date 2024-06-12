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
                GLOBAL::world.rigidbodies, players[(i + 1) % 2], GLOBAL::activePowerUp.type
            );
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
            COLLIDER::UpdateColliderTransform(GLOBAL::world.colliders[players[i].local.colliderGroup][players[i].local.colliderIndex], GLOBAL::world.gTransforms[players[i].local.transformIndex]);
        }
    }

    void MovePlayers ()
    {
        auto& playersCount = GLOBAL::scene.world->playersCount;
        auto& players = GLOBAL::scene.world->players;

        for (int i = 0; i < playersCount; i++)
        {
            PLAYER::MOVEMENT::Move(players[i], GLOBAL::world.rigidbodies, GLOBAL::timeDelta, GLOBAL::activePowerUp.type);
        }
    }

    void MoveRigidbodies ()
    {
        for (int i = 0; i < GLOBAL::world.rigidbodiesCount; i++)
        {
            RIGIDBODY::Move(GLOBAL::world.rigidbodies[i], GLOBAL::world.lTransforms, GLOBAL::world.gTransforms, float(GLOBAL::timeDelta));
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
                if (collider.local.collisionEventName == "SpringTrap")
                {

                }
                else if (collider.local.collisionEventName == "PowerUp")
                {
                    u64 transformIndex = OBJECT::ID_DEFAULT;
                    OBJECT::GetComponentFast<TRANSFORM::LTransform>(transformIndex, GLOBAL::world.transformsCount, GLOBAL::world.lTransforms, collider.id);
                    float yOffset = 0.01f;
                    float yOffsetSpeed = 2.5f;
                    float yRotationSpeed = 30.f;
                    GLOBAL::world.lTransforms[transformIndex].base.position.y += yOffset * sin(GLOBAL::timeCurrent * yOffsetSpeed);
                    GLOBAL::world.lTransforms[transformIndex].base.rotation.y += yRotationSpeed * GLOBAL::timeDelta;
                    GLOBAL::world.lTransforms[transformIndex].flags = TRANSFORM::DIRTY;

                }
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

}