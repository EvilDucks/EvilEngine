#pragma once

#include "global.hpp"

namespace UPDATE {

	void Collisions () {
        PROFILER { ZoneScopedN("GLOBAL: Collisions"); }

        CheckOBBCollisions(COLLIDER::ColliderGroup::PLAYER, COLLIDER::ColliderGroup::MAP, GLOBAL::scene.world->colliders, GLOBAL::scene.world->collidersCount);



        for (int i = 0; i < GLOBAL::playerCount; i++)
        {
            PLAYER::HandlePlayerCollisions(GLOBAL::players[i], GLOBAL::world.colliders, GLOBAL::world.collidersCount, GLOBAL::world.lTransforms);
        }
    }


	void UICollisions ()
    {
//        for (int i = 0; i < GLOBAL::playerCount; i++)
//        {
//            CheckUICollisions(GLOBAL::canvas.colliders[COLLIDER::ColliderGroup::UI], GLOBAL::canvas.collidersCount[COLLIDER::ColliderGroup::UI], GLOBAL::players[i].local.selection.x, GLOBAL::players[i].local.selection.y, GLOBAL::canvas.buttons, GLOBAL::canvas.buttonsCount, GLOBAL::uiManager);
//        }

        //for (int i = 0; i < GLOBAL::playerCount; i++)
        {
            CheckUICollisions(GLOBAL::canvas.colliders[COLLIDER::ColliderGroup::UI], GLOBAL::canvas.collidersCount[COLLIDER::ColliderGroup::UI], GLOBAL::players[0].local.selection.x, GLOBAL::players[0].local.selection.y, GLOBAL::canvas.buttons, GLOBAL::canvas.buttonsCount, GLOBAL::uiManager);
        }
    }

    void MovePlayers ()
    {
        for (int i = 0; i < GLOBAL::playerCount; i++)
        {
            PLAYER::MOVEMENT::Move(GLOBAL::players[i], GLOBAL::world.lTransforms, GLOBAL::world.colliders);
        }
    }

    void SetCamPositions ()
    {
        for (int i = 0; i < GLOBAL::playerCount; i++) {
            auto& positions = GLOBAL::camPos[i];
            positions = GLOBAL::world.lTransforms[GLOBAL::players[i].local.transformIndex].base.position;
            //GLOBAL::camPos[i] = positions;
            glm::vec3 targetVec = glm::rotate(GLOBAL::thirdPerson, glm::radians(GLOBAL::players[i].local.movement.yaw), glm::vec3(0.f, 1.f, 0.f));

            positions += targetVec;
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