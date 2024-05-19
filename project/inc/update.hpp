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

		const ROTATING::Rotating component1 { 0.0f, 0.0f, 1.0f };
		const ROTATING::Rotating component2 { 0.0f, 1.0f, 0.0f };

		auto& transformsOffset = world.transformsOffset;
		auto& transformsCount = world.transformsCount;
		auto& transforms = world.lTransforms;

		auto& parenthood = world.parenthoods[1];	
		auto& parent = parenthood.id;					// Get node (child of root)
		auto& child = parenthood.base.children[0];		// Get node (child of child)

		{ // PARENT
			auto& transform = transforms[parent];
			transform.local.rotation += component1; 
			// This will propagate through all children and child's children.
			transform.flags = TRANSFORM::DIRTY; 
		}

		{ // CHILD
			auto& transform = transforms[child];
			transform.local.rotation += component2; 
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