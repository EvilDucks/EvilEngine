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
		
		// Rotate ENTITY_4 so it's child will rotate too
		//  Find ENTITY_4 TRANSFORM then find it's children
		//  For each child and their child and cheir child recalculate their globalspace.
		// For now we hardcode it... so theres something always ratating
		//  Constant rotation should be a component and that logic should be component based
		//if (world.parenthoodsCount > 1) { 
		//	//assert(world.parenthoodsCount == 2);
		//	//

		//

		auto& transformsOffset = world.transformsOffset;
		auto& transformsCount = world.transformsCount;
		auto& transforms = world.lTransforms;

		auto& parenthood = world.parenthoods[1];	
		auto& parent = parenthood.id;					// Get node (child of root)
		auto& child = parenthood.base.children[0];		// Get node (child of child)

		{ // PARENT
			auto& transfrom = transforms[parent];
			transfrom.local.rotation.z += 1; 
			transfrom.flags = TRANSFORM::DIRTY;
		}

		{ // CHILD
			auto& transfrom = transforms[child];
			transfrom.local.rotation.y += 1; 
			//transfrom.flags = TRANSFORM::DIRTY;
		}

		TRANSFORM::ApplyDirtyFlag (
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