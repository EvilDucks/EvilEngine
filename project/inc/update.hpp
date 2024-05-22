#pragma once
#include "global.hpp"

namespace UPDATE {

	void Collisions (
		std::unordered_map<COLLIDER::ColliderGroup, COLLIDER::Collider*> colliders, 
		std::unordered_map<COLLIDER::ColliderGroup, u64> collidersCount, 
		PLAYER::Player *players, u64 playerCount
	) {
		PROFILER { ZoneScopedN("GLOBAL: Collisions"); }

		//CheckOBBCollisions(
		//    COLLIDER::ColliderGroup::PLAYER, COLLIDER::ColliderGroup::MAP, 
		//    GLOBAL::scene.world->colliders, GLOBAL::scene.world->collidersCount
		//);

		for (int i = 0; i < playerCount; i++)
		{
			//PLAYER::HandlePlayerCollisions(players[i], colliders, collidersCount);
		}
	}


	void UICollisions (
		std::unordered_map<COLLIDER::ColliderGroup, COLLIDER::Collider*> colliders, 
		std::unordered_map<COLLIDER::ColliderGroup, u64> collidersCount, 
		PLAYER::Player *players, u64 playerCount
	) {
		for (int i = 0; i < playerCount; i++)
		{
			CheckUICollisions(
				colliders[COLLIDER::ColliderGroup::UI], collidersCount[COLLIDER::ColliderGroup::UI], 
				players[i].local.selection.x, players[i].local.selection.y, 
				GLOBAL::canvas.buttons, GLOBAL::canvas.buttonsCount, GLOBAL::uiManager
			);
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