#pragma once

#include "global.hpp"

namespace UPDATE {

	void Collisions () {
        PROFILER { ZoneScopedN("GLOBAL: Collisions"); }

        CheckOBBCollisions(COLLIDER::ColliderGroup::PLAYER, COLLIDER::ColliderGroup::MAP, GLOBAL::scene.world->colliders, GLOBAL::scene.world->collidersCount);

        CheckOBBCollisions(COLLIDER::ColliderGroup::PLAYER, COLLIDER::ColliderGroup::TRIGGER, GLOBAL::scene.world->colliders, GLOBAL::scene.world->collidersCount);


        for (int i = 0; i < GLOBAL::playerCount; i++)
        {
            PLAYER::HandlePlayerCollisions(GLOBAL::players[i], GLOBAL::world.colliders, GLOBAL::world.collidersCount, GLOBAL::world.lTransforms, GLOBAL::world.gTransforms);
        }

        for (int i = 0; i < GLOBAL::scene.world->collidersCount[COLLIDER::ColliderGroup::TRIGGER]; i++)
        {
            COLLISION::MANAGER::HandleTriggerCollisions(GLOBAL::collisionManager, GLOBAL::scene.world->colliders[COLLIDER::ColliderGroup::TRIGGER][i], GLOBAL::scene.world->colliders, GLOBAL::scene.world->collidersCount);
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

    void UpdateColliders ()
    {
        for (int i = 0; i < GLOBAL::playerCount; i++)
        {
            COLLIDER::UpdateColliderTransform(GLOBAL::world.colliders[GLOBAL::players[i].local.colliderGroup][GLOBAL::players[i].local.colliderIndex], GLOBAL::world.gTransforms[GLOBAL::players[i].local.transformIndex]);
        }
    }

    void MovePlayers ()
    {
        for (int i = 0; i < GLOBAL::playerCount; i++)
        {
            PLAYER::MOVEMENT::Move(GLOBAL::players[i], GLOBAL::world.lTransforms, GLOBAL::world.gTransforms, float(GLOBAL::timeDelta));
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