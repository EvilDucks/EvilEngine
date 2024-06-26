#pragma once
#include "scene.hpp"
#include "manager/objects.hpp"

#include "generator/mapGenerator.hpp"

namespace MANAGER::SCENES::OBJECTS {

	void CreateLoad () { // GLTF'S
		DEBUG_ENGINE spdlog::info ("Creating GLTF scenes and objects.");
			
		MANAGER::OBJECTS::GLTF::Create ();
			
		DEBUG_ENGINE spdlog::info ("Loading GLTF scenes and objects.");
			
		MANAGER::OBJECTS::GLTF::Load ();

		{ // root's transform change.
			auto& gltfWorld = MANAGER::OBJECTS::GLTF::worlds[2];
			// 
			gltfWorld.lTransforms[0].base.position.x = 2.0f;
			gltfWorld.lTransforms[0].base.position.y = 1.0f;
			gltfWorld.lTransforms[0].base.position.z = 2.0f;
			//
			gltfWorld.lTransforms[0].base.rotation.x = 0.0f;
			gltfWorld.lTransforms[0].base.rotation.y = 180.0f;
			gltfWorld.lTransforms[0].base.rotation.z = 0.0f;

            gltfWorld.lTransforms[0].base.scale /= 5;
		}

		{ // root's transform change.
			auto& gltfWorld = MANAGER::OBJECTS::GLTF::worlds[1];
			//
			gltfWorld.lTransforms[0].base.position.x = 0.0f;
			gltfWorld.lTransforms[0].base.position.y = 3.0f;
			gltfWorld.lTransforms[0].base.position.z = 1.0f;
			//
			gltfWorld.lTransforms[0].base.rotation.x = 0.0f;
			gltfWorld.lTransforms[0].base.rotation.y = 0.0f;
			gltfWorld.lTransforms[0].base.rotation.z = 0.0f;
		}

        { // root's transform change.
            auto& gltfWorld = MANAGER::OBJECTS::GLTF::worlds[0];
            //
            gltfWorld.lTransforms[0].base.position.x = -2.0f;
            gltfWorld.lTransforms[0].base.position.y = 1.0f;
            gltfWorld.lTransforms[0].base.position.z = 2.0f;
            //
            gltfWorld.lTransforms[0].base.rotation.x = 0.0f;
            gltfWorld.lTransforms[0].base.rotation.y = 180.0f;
            gltfWorld.lTransforms[0].base.rotation.z = 0.0f;

            gltfWorld.lTransforms[0].base.scale /= 5;
        }

		MANAGER::OBJECTS::GLTF::Set ();
	}

}




namespace MANAGER::SCENES::GENERATOR {

	MAP_GENERATOR::MG mapGenerator = nullptr;
	SCENE::World* segmentsWorld = nullptr;
	u8 segmentsCount = 0;

	SCENE::SceneLoadContext* segmentLoad = nullptr;
	RESOURCES::Json* segmentsJson = nullptr;

	void Create ()  {
		
		MAP_GENERATOR::ParkourDifficulty difficulty {
			/*rangePosition*/ 0.7f,
			/*rangeWidth*/    1.0f
		};

		MAP_GENERATOR::Modifiers modifiers {
			/*levelLength*/ 				5,
			/*stationaryTrapsAmount*/ 		0.7f,
			/*windowTrapsAmount*/ 			1.0f,
			/*checkpointsSpacing*/          2,
            /*powerUpsSpacing*/             3,
			/*parkourDifficulty*/ 			difficulty,
			/*windingModuleProbability*/	0.5f
		};

		mapGenerator = new MAP_GENERATOR::MapGenerator;
		mapGenerator->modifiers = modifiers;

		MAP_GENERATOR::LoadModules (mapGenerator, RESOURCES::MANAGER::SEGMENTS);
		MAP_GENERATOR::GenerateLevel (mapGenerator);
        //MAP_GENERATOR::GenerateSpecificLevel(mapGenerator, 2);
        
		segmentsCount = 
			mapGenerator->_generatedLevelMainBranch.size() + 
			mapGenerator->_generatedLevelSideBranch.size() + 
			mapGenerator->_generatedLevelCenter.size();

		// Memory allocations...
		segmentsJson	= new RESOURCES::Json[segmentsCount];
		segmentLoad		= new SCENE::SceneLoadContext[segmentsCount] { 0 };
		segmentsWorld	= new SCENE::World[segmentsCount] { 0 };
	}


	void CreateWorlds (
		SCENE::World& world,
		SCENE::SHARED::World& sharedWorld
	) {
		// Segments Loading...
		for (u8 iSegment = 0; iSegment < segmentsCount; ++iSegment) { 

			auto&& mainBranches = mapGenerator->_generatedLevelMainBranch;
			auto&& sideBranches = mapGenerator->_generatedLevelSideBranch;
			auto&& centerBranches = mapGenerator->_generatedLevelCenter;
			auto& mainBranch = mainBranches[0];

			{/* Main Branch */
				if (iSegment < mainBranches.size ()) {
					auto id = iSegment;
					mainBranch = mainBranches[id];
				} 

				else if (iSegment < mainBranches.size () + sideBranches.size ()) {
					auto id = iSegment - mainBranches.size ();
					mainBranch = sideBranches[id];
				} 

				else {
					auto id = iSegment - mainBranches.size () - sideBranches.size ();
					mainBranch = centerBranches	[id];
				}
			}

			auto& loadHelper = segmentLoad[iSegment];
			auto& fileJson = segmentsJson[iSegment];
			auto& cWorld = segmentsWorld[iSegment];
			
			DEBUG if (mainBranch.parkourDifficulty < 1.0f || mainBranch.parkourDifficulty > 10.0f)
				ErrorExit ("Segment difficulty ({0}) set to an invalid value!", mainBranch.parkourDifficulty);

			const u8 DIFFICULTY = (u8)mainBranch.parkourDifficulty;
			const u8 EXIT_TYPE = MAP_GENERATOR::ModuleTypeToInt (mainBranch.type); 						// 1;  // 0 - 2 (3)

			RESOURCES::Parse (
				fileJson, 
				RESOURCES::MANAGER::SCENES::SEGMENTS[MAP_GENERATOR::CalculateSegmentIndex(mapGenerator, DIFFICULTY, EXIT_TYPE)]
			);
			
			// map key is 64bit... we cast it to smaller type...
			auto& collidersMapCount 			= *(u16*)(void*)(&world.collidersCount[COLLIDER::ColliderGroup::MAP]);
			auto& collidersTriggerCount 		= *(u16*)(void*)(&world.collidersCount[COLLIDER::ColliderGroup::TRIGGER]);
			auto& collidersPlayerCount 			= *(u16*)(void*)(&world.collidersCount[COLLIDER::ColliderGroup::PLAYER]);

			// SEGMENT's properties.
			auto& collidersSegmentMapCount 		= *(u16*)(void*)(&cWorld.collidersCount[COLLIDER::ColliderGroup::MAP]);
			auto& collidersSegmentTriggerCount 	= *(u16*)(void*)(&cWorld.collidersCount[COLLIDER::ColliderGroup::TRIGGER]);
			auto& collidersSegmentPlayerCount 	= *(u16*)(void*)(&cWorld.collidersCount[COLLIDER::ColliderGroup::PLAYER]);

			DEBUG_ENGINE { spdlog::info ("JSON {0} Segment-Scene Initialization", iSegment); }

			RESOURCES::SCENE::Create (
				fileJson,
				sharedWorld.materialsCount, sharedWorld.meshesCount, 					// Already set
				cWorld.tables.meshes, cWorld.tables.parenthoodChildren, 				// Tables
				loadHelper.relationsLookUpTable, cWorld.transformsOffset,				// Helper Logic + what we get
				cWorld.parenthoodsCount, cWorld.transformsCount, world.rotatingsCount,	// What we actually get.
				collidersSegmentMapCount, collidersSegmentTriggerCount, collidersSegmentPlayerCount, 
				cWorld.rigidbodiesCount, cWorld.playersCount
			);

			collidersMapCount += collidersSegmentMapCount;
			collidersTriggerCount += collidersSegmentTriggerCount;

			//DEBUG spdlog::info ("{0}, mapColliders: {1}", iSegment, collidersSegmentMapCount);
			//DEBUG spdlog::info ("{0}, triggerColliders: {1}", iSegment, collidersSegmentTriggerCount);
		}

		//world.collidersCount[COLLIDER::ColliderGroup::MAP] += 20; // HACK
		//DEBUG spdlog::info ("mapColliders: {0}", world.collidersCount[COLLIDER::ColliderGroup::MAP]);
		//DEBUG spdlog::info ("triggerColliders: {0}", world.collidersCount[COLLIDER::ColliderGroup::TRIGGER]);
		//DEBUG spdlog::info ("triggerColliders: {0}", "huh");
	}



	void Allocate () {
		for (u8 iSegment = 0; iSegment < segmentsCount; ++iSegment) {
			auto& cWorld = segmentsWorld[iSegment];
			if (cWorld.parenthoodsCount) {
				cWorld.parenthoods = new PARENTHOOD::Parenthood[cWorld.parenthoodsCount] { 0 };
			}
		}

		for (u8 iSegment = 0; iSegment < segmentsCount; ++iSegment) {
			auto& cWorld = segmentsWorld[iSegment];
			if (cWorld.transformsCount) {
				cWorld.lTransforms = new TRANSFORM::LTransform[cWorld.transformsCount] { 0 };
				cWorld.gTransforms = new TRANSFORM::GTransform[cWorld.transformsCount];
			}
		}
	}



	void Destroy () {
		for (u8 iSegment = 0; iSegment < segmentsCount; ++iSegment) {
			auto& cWorld = segmentsWorld[iSegment];
			SCENE::WORLD::Destroy (cWorld);
		}
	}



	void Load (
		SCENE::World& world,
		SCENE::SHARED::World& sharedWorld
	) {
		for (u8 iSegment = 0; iSegment < segmentsCount; ++iSegment) { // SEGMENTS
			auto& fileJson = segmentsJson[iSegment];
			auto& loadHelper = segmentLoad[iSegment];
			auto& cWorld = segmentsWorld[iSegment];

			//DEBUG { spdlog::info ("load!"); }

			//DEBUG spdlog::info ("aaa: {0}, {1}", cWorld.transformsCount, cWorld.parenthoodsCount);
			RESOURCES::SCENE::Load (
				fileJson, 
				sharedWorld.materialsCount, sharedWorld.meshesCount, 
				cWorld.tables.meshes, cWorld.tables.parenthoodChildren, 
				loadHelper.relationsLookUpTable, cWorld.transformsOffset,
				cWorld.parenthoodsCount, cWorld.parenthoods, 
				cWorld.transformsCount, cWorld.lTransforms,
				cWorld.rotatingsCount, cWorld.rotatings,
				world.colliders[COLLIDER::ColliderGroup::MAP],
				world.colliders[COLLIDER::ColliderGroup::TRIGGER],
				world.colliders[COLLIDER::ColliderGroup::PLAYER]
			);
		}

		// free uneeded resources...
		delete[] segmentsJson;
		delete[] segmentLoad;
	}



	void Set () {
		// We start from the second segment as there is no need to move the first one
		for (u8 iSegment = 1; iSegment < segmentsCount; ++iSegment) {
			// After updating all the main branch segments we switch to side branch and later center
			auto& segment = mapGenerator->_generatedLevelMainBranch[0];
			if (iSegment < mapGenerator->_generatedLevelMainBranch.size())
			{
				segment = mapGenerator->_generatedLevelMainBranch[iSegment];
			}
			else if (iSegment < mapGenerator->_generatedLevelMainBranch.size() + mapGenerator->_generatedLevelSideBranch.size())
			{
				segment = mapGenerator->_generatedLevelSideBranch[iSegment - mapGenerator->_generatedLevelMainBranch.size()];
			}
			else
			{
				segment = mapGenerator->_generatedLevelCenter[iSegment - mapGenerator->_generatedLevelMainBranch.size() - mapGenerator->_generatedLevelSideBranch.size()];
			}
			auto& cWorld = segmentsWorld[iSegment];

			// Moving the segment higher based on its height
			cWorld.lTransforms[0].base.position.y += float(segment.moduleHeight)*24.f;

			// Rotating the segment
			cWorld.lTransforms[0].base.rotation.y += float(segment.rotation);

			// If segment is in clockwise direction we mirror it
			if (segment.direction == MODULE::ModuleDirection::CW)
			{
				for (int i = 1; i < cWorld.parenthoods[0].base.childrenCount+1; i++)
				{
					cWorld.lTransforms[i].base.position.x *= -1;
				}
			}
		}

		// Precalculate Global Trnasfroms
		for (u8 iSegment = 0; iSegment < segmentsCount; ++iSegment) { 
			auto& cWorld = segmentsWorld[iSegment];
			TRANSFORM::Precalculate (
				cWorld.parenthoodsCount, cWorld.parenthoods,
				cWorld.transformsCount, cWorld.lTransforms, cWorld.gTransforms
			);
		}
	}


	void GetMaxInstances (u8*& wInstancesCounts) {
		for (u8 iSegment = 0; iSegment < segmentsCount; ++iSegment) { // Precalculate Global Trnasfroms
			auto& cWorld = segmentsWorld[iSegment];
			MATERIAL::MESHTABLE::GetMaxInstances (cWorld.tables.meshes, wInstancesCounts);
		}
	}

	void InitializeColliders (
		SCENE::World& world,
		SCENE::SHARED::World& sharedWorld
	) {
		// Initialize segment colliders
		// HACK This has to happen inside location.hpp load phase !

		const u16 hackOffset = 25; // HACK Skip main colliders.

		u16 giCollider = 3; // HACK, wall is 1st, WindowTrap is 2nd., movingPlatform is 3rd
		for (u16 iSegment = 0; iSegment < segmentsCount; ++iSegment) {
			
			auto& worldColliders = world.colliders[COLLIDER::ColliderGroup::MAP];

			auto& segment = segmentsWorld[iSegment];
			auto& collidersCount = segment.collidersCount[COLLIDER::ColliderGroup::MAP];

			for (u16 iCollider = 0; iCollider < collidersCount; ++iCollider) {
				
				auto& componentCollider = worldColliders[giCollider];
				auto& base = componentCollider.local;

				base.group = COLLIDER::ColliderGroup::MAP;
				base.type = COLLIDER::ColliderType::OBB;
				
				u16 skipIndex = iCollider + 1;
				componentCollider.id = hackOffset + (iSegment * segmentsCount) + skipIndex; // It simply should refer to segments collisions

                base.segmentIndex = iSegment;
                base.transformIndex = skipIndex;

				COLLIDER::InitializeColliderSize (componentCollider, sharedWorld.meshes[0], segment.gTransforms[skipIndex]); // HACK +1 to skip root transform
				++giCollider;
			}
		}


		u16 springTrapsCount = 0;
        u16 powerUpsCount = 0;
        u16 windowTrapsCount = 0;
		u16 checkpointsCount = 1;
		u16 giTriggerCollider = 5; // HACK, wall is 1st. power up is 2nd, windowTrap is 3rd, movingPlatform is 4th, windowTrapTriggerKnockback is 5th...
		for (u16 iSegment = 0; iSegment < segmentsCount; ++iSegment) {

			auto& worldColliders = world.colliders[COLLIDER::ColliderGroup::TRIGGER];

			auto& segment = segmentsWorld[iSegment];
			auto& collidersCount = segment.collidersCount[COLLIDER::ColliderGroup::TRIGGER];

			for (u16 iCollider = 0; iCollider < collidersCount; ++iCollider) {

				auto& componentCollider = worldColliders[giTriggerCollider];
				auto& base = componentCollider.local;

				base.group = COLLIDER::ColliderGroup::TRIGGER;
				base.type = COLLIDER::ColliderType::AABB;

				const u16 hackOffset = 15; // HACK
				componentCollider.id = hackOffset + (iSegment * segmentsCount) + iCollider + 1; // It simply should refer to segments collisions
				componentCollider.local.segmentIndex = iSegment;
				componentCollider.local.transformIndex = segment.transformsCount-collidersCount+iCollider;


				if (iCollider == 0)
				{
					base.collisionEventName = "PowerUp";
                    powerUpsCount ++;
				}
				else if(iCollider == 1)
				{
					base.collisionEventName = "CheckPoint";
					world.checkpoints[checkpointsCount].position = segment.gTransforms[segment.transformsCount-collidersCount+iCollider][3];
					world.checkpoints[checkpointsCount].id = componentCollider.id;
					checkpointsCount ++;
				}
                else
                {
                    base.collisionEventName = "SpringTrap";
                    springTrapsCount ++;
                }

				COLLIDER::InitializeColliderSize (componentCollider, sharedWorld.meshes[0], segment.gTransforms[segment.transformsCount-collidersCount+iCollider]); // HACK +1 to skip root transform
				++giTriggerCollider;
			}
		}


        MAP_GENERATOR::TrapGeneration (mapGenerator, springTrapsCount);

        MAP_GENERATOR::ApplyTraps (
			mapGenerator, world.colliders[COLLIDER::ColliderGroup::TRIGGER], 
			world.collidersCount[COLLIDER::ColliderGroup::TRIGGER], segmentsWorld
		);

        MAP_GENERATOR::CheckpointsGeneration (mapGenerator, checkpointsCount);

        MAP_GENERATOR::ApplyCheckpoints (
			mapGenerator, world.colliders[COLLIDER::ColliderGroup::TRIGGER], 
			world.collidersCount[COLLIDER::ColliderGroup::TRIGGER], segmentsWorld
		);

        MAP_GENERATOR::PowerUpsGeneration(mapGenerator, powerUpsCount);

        MAP_GENERATOR::ApplyPowerUps (
                mapGenerator, world.colliders[COLLIDER::ColliderGroup::TRIGGER],
                world.collidersCount[COLLIDER::ColliderGroup::TRIGGER], segmentsWorld
        );
	}

}





namespace MANAGER::SCENES::MAIN {

	RESOURCES::Json sceneJson;
	SCENE::SceneLoadContext sceneLoad { 0 };

	void Create (
		SCENE::Canvas& canvas,
		SCENE::Screen& screen,
		SCENE::World& world,
		SCENE::SHARED::World& sharedWorld
	) {

		RESOURCES::Parse (sceneJson, RESOURCES::MANAGER::SCENES::ALPHA);

		// map key is 64bit... we cast it to smaller type...
		auto& collidersMapCount 	= *(u16*)(void*)(&world.collidersCount[COLLIDER::ColliderGroup::MAP]);
		auto& collidersTriggerCount = *(u16*)(void*)(&world.collidersCount[COLLIDER::ColliderGroup::TRIGGER]);
		auto& collidersPlayerCount	= *(u16*)(void*)(&world.collidersCount[COLLIDER::ColliderGroup::PLAYER]);

		DEBUG_ENGINE { spdlog::info ("JSON Main Scene Initialization"); }

		RESOURCES::SCENE::Create (
			sceneJson,
			sharedWorld.materialsCount, sharedWorld.meshesCount, 						// Already set
			world.tables.meshes, world.tables.parenthoodChildren, 						// Tables
			sceneLoad.relationsLookUpTable, world.transformsOffset,						// Helper Logic + what we get
			world.parenthoodsCount, world.transformsCount, world.rotatingsCount,		// What we actually get.
			collidersMapCount, collidersTriggerCount, collidersPlayerCount, 
			world.rigidbodiesCount, world.playersCount
		);

		{ 
			// SCREEN
			screen.parenthoodsCount = 0; 
			screen.transformsCount = 1;

			// CANVAS
			canvas.parenthoodsCount = 0; 
			canvas.rectanglesCount = 25;
			canvas.buttonsCount = 1;
			canvas.collidersCount[COLLIDER::ColliderGroup::UI] = 1;

			// WORLD
			world.collidersCount[COLLIDER::ColliderGroup::CAMERA]	= 2;
            world.movingPlatformsCount = 1;
		}
	}

	void Load (SCENE::World& world, SCENE::SHARED::World& sharedWorld) {
		RESOURCES::SCENE::Load (
			sceneJson, 
			sharedWorld.materialsCount, sharedWorld.meshesCount, 
			world.tables.meshes, world.tables.parenthoodChildren, 
			sceneLoad.relationsLookUpTable, world.transformsOffset,
			world.parenthoodsCount, world.parenthoods, 
			world.transformsCount, world.lTransforms,
			world.rotatingsCount, world.rotatings,
			world.colliders[COLLIDER::ColliderGroup::MAP],
			world.colliders[COLLIDER::ColliderGroup::TRIGGER],
			world.colliders[COLLIDER::ColliderGroup::PLAYER]
		);
	}

	void Allocate (
		SCENE::Canvas& canvas,
		SCENE::Screen& screen,
		SCENE::World& world
	) {
		{ // SCREEN
			if (screen.parenthoodsCount) screen.parenthoods = new PARENTHOOD::Parenthood[screen.parenthoodsCount] { 0 };
			if (screen.transformsCount) {
				screen.lTransforms = new TRANSFORM::LTransform[screen.transformsCount] { 0 };
				screen.gTransforms = new TRANSFORM::GTransform[screen.transformsCount];
			}
		}

		{ // CANVAS
			if (canvas.parenthoodsCount) canvas.parenthoods = new PARENTHOOD::Parenthood[canvas.parenthoodsCount] { 0 };
			if (canvas.rectanglesCount) {
				canvas.lRectangles = new RECTANGLE::LRectangle[canvas.rectanglesCount] { 0 };
				canvas.gRectangles = new RECTANGLE::GRectangle[canvas.rectanglesCount];
			}
			if (canvas.buttonsCount) canvas.buttons = new UI::BUTTON::Button[canvas.buttonsCount] { 0 };

			const auto& cu = COLLIDER::ColliderGroup::UI;

			if (canvas.collidersCount[cu]) 
				canvas.colliders[cu] = new COLLIDER::Collider[canvas.collidersCount[cu]] { 0 };
		}

		{ // WORLD
			if (world.parenthoodsCount) world.parenthoods = new PARENTHOOD::Parenthood[world.parenthoodsCount] { 0 };

			if (world.transformsCount) {
				world.lTransforms = new TRANSFORM::LTransform[world.transformsCount] { 0 };
				world.gTransforms = new TRANSFORM::GTransform[world.transformsCount];
			}

			if (world.rotatingsCount) {
				world.rotatings = new ROTATING::Rotating[world.rotatingsCount] { 0 };
			}

            if (world.movingPlatformsCount) {
                world.movingPlatforms = new MOVING_PLATFORM::MovingPlatform[world.movingPlatformsCount] { 0 };
            }

			MANAGER::SCENES::GENERATOR::Allocate ();

			const auto& cp = COLLIDER::ColliderGroup::PLAYER;
			const auto& cm = COLLIDER::ColliderGroup::MAP;
			const auto& ct = COLLIDER::ColliderGroup::TRIGGER;
			const auto& cc = COLLIDER::ColliderGroup::CAMERA;

			if (world.collidersCount[cp]) 
				world.colliders[cp] = new COLLIDER::Collider[world.collidersCount[cp]] { 0 };
			if (world.collidersCount[cm]) 
				world.colliders[cm] = new COLLIDER::Collider[world.collidersCount[cm]] { 0 };
			if (world.collidersCount[ct]) 
				world.colliders[ct] = new COLLIDER::Collider[world.collidersCount[ct]] { 0 };
			if (world.collidersCount[cc]) 
				world.colliders[cc] = new COLLIDER::Collider[world.collidersCount[cc]] { 0 };

			if (world.rigidbodiesCount) 
				world.rigidbodies = new RIGIDBODY::Rigidbody[world.rigidbodiesCount] { 0 };

			if (world.playersCount) 
				world.players = new PLAYER::Player[world.playersCount] { 0 };
		}
	}

	void Set (
		SCENE::Canvas& canvas,
		SCENE::Screen& screen,
		SCENE::World& world, 
		SCENE::SHARED::World& sharedWorld
	) {
		{ // World
			{ // Checkpoint
				world.checkpointsCount = MANAGER::SCENES::GENERATOR::segmentsCount + 1;
				world.checkpoints = new CHECKPOINT::Checkpoint[world.checkpointsCount] {0};
				world.checkpoints[0].position = glm::vec3(0.f, -2.f, 0.f);
			}

			{ // AI StateMachine
				world.windowTrapCount = 1;
				world.windowTraps = new AGENT::WindowData[world.windowTrapCount];	// DELETE?

				// TEMP window trap create - delete after import from json works
				world.windowTraps[0].isRechargable = true;
				world.windowTraps[0].isActive = true;

                world.windowTraps[0].parentId = 12; // HARDCODE
                world.windowTraps[0].newRot = {90, 0, 0}; // HARDCODE
                world.windowTraps[0].rotAxis = {90, 0, 0}; // HARDCODE
                world.windowTraps[0].isFrontAxis = true;

			}
    }

		{ // Screen

			{ // ROOT
				auto& componentTransform = screen.lTransforms[0];
				auto& base = componentTransform.base;
				componentTransform.id = OBJECT::_06;
				//
				base.position	= glm::vec3 (0.0f, 0.0f, 0.0f);
				base.rotation	= glm::vec3 (0.0f, 0.0f, 0.0f);
				base.scale		= glm::vec3 (1.0f, 1.0f, 1.0f);
			}

		}

		{ // CANVAS

			{ // PLAYER1 CHARGE TEXT
				auto& componentTransform = canvas.lRectangles[0];
				auto& base = componentTransform.base;

				componentTransform.id = 0;

                base.anchor		= RECTANGLE::Anchor		{ 0.f, 0.f };
                base.position	= RECTANGLE::Position	{ 0.0f, 175.0f };
                base.size		= RECTANGLE::Size		{ 50.0f, 50.0f };
                base.pivot		= RECTANGLE::Pivot		{ 0.0f, 0.0f };
                base.rotation	= RECTANGLE::Rotation	{ 0.0f };
                base.scale		= RECTANGLE::Scale		{ 0.5f, 0.5f };
			}

			{ // PLAYER2 CHARGE TEXT
				auto& componentTransform = canvas.lRectangles[1];
				auto& base = componentTransform.base;

				componentTransform.id = 1;

                base.anchor		= RECTANGLE::Anchor		{ 1.f, 0.f };
                base.position	= RECTANGLE::Position	{ -175.0f, 175.0f };
                base.size		= RECTANGLE::Size		{ 50.0f, 50.0f };
                base.pivot		= RECTANGLE::Pivot		{ 0.0f, 0.0f };
                base.rotation	= RECTANGLE::Rotation	{ 0.0f };
                base.scale		= RECTANGLE::Scale		{ 0.5f, 0.5f };
			}

			{ // BUTTON
				auto& componentTransform = canvas.lRectangles[2];
				auto& base = componentTransform.base;

				componentTransform.id =  OBJECT::_09_SQUARE_1;

				base.anchor		= RECTANGLE::Anchor		{ 0.5f, 0.5f };
				base.position	= RECTANGLE::Position	{ -100.0f, -50.0f }; // (-) half of size -> center it's position // { 700.0f, 50.0f };
				base.size		= RECTANGLE::Size		{ 200.0f, 100.0f };
				base.pivot		= RECTANGLE::Pivot		{ 100.0f, 50.0f }; // half of size -> center it's pivot
				base.rotation	= RECTANGLE::Rotation	{ 0.0f };
				base.scale		= RECTANGLE::Scale		{ 0.5f, 0.5f };
			}

            { // TOWER MINIMAP
                auto& componentTransform = canvas.lRectangles[3];
                auto& base = componentTransform.base;

                componentTransform.id =  16;

                base.anchor		= RECTANGLE::Anchor		{ 0.5f, 0.01f };
                base.position	= RECTANGLE::Position	{ -150.0f, 0.0f };
                base.size		= RECTANGLE::Size		{ 100.0f, 100.0f };
                base.pivot		= RECTANGLE::Pivot		{ 150.0f, 300.0f };
                base.rotation	= RECTANGLE::Rotation	{ 0.0f };
                base.scale		= RECTANGLE::Scale		{ 3.0f, 3.0f };
            }

            { // MINIMAP PLAYER1 ICON
                auto& componentTransform = canvas.lRectangles[4];
                auto& base = componentTransform.base;

                componentTransform.id =  17;

                base.anchor		= RECTANGLE::Anchor		{ 0.5f, 0.01f };
                base.position	= RECTANGLE::Position	{ -100.0f, 0.0f };
                base.size		= RECTANGLE::Size		{ 50.0f, 50.0f };
                base.pivot		= RECTANGLE::Pivot		{ 25.0f, 50.0f };
                base.rotation	= RECTANGLE::Rotation	{ 0.0f };
                base.scale		= RECTANGLE::Scale		{ 1.0f, 1.0f };
            }

            { // MINIMAP PLAYER2 ICON
                auto& componentTransform = canvas.lRectangles[5];
                auto& base = componentTransform.base;

                componentTransform.id =  18;

                base.anchor		= RECTANGLE::Anchor		{ 0.5f, 0.01f };
                base.position	= RECTANGLE::Position	{ 50.0f, 0.0f };
                base.size		= RECTANGLE::Size		{ 50.0f, 50.0f };
                base.pivot		= RECTANGLE::Pivot		{ 25.0f, 50.0f };
                base.rotation	= RECTANGLE::Rotation	{ 0.0f };
                base.scale		= RECTANGLE::Scale		{ 1.0f, 1.0f };
            }

            { // PLAYER1 POWER UP ICON
                auto& componentTransform = canvas.lRectangles[6];
                auto& base = componentTransform.base;

                componentTransform.id =  19;

                base.anchor		= RECTANGLE::Anchor		{ 0.f, 1.f };
                base.position	= RECTANGLE::Position	{ 0.0f, -218.0f };
                base.size		= RECTANGLE::Size		{ 127.0f, 218.0f };
                base.pivot		= RECTANGLE::Pivot		{ 63.5f, 109.0f };
                base.rotation	= RECTANGLE::Rotation	{ 0.0f };
                base.scale		= RECTANGLE::Scale		{ 1.0f, 1.0f };
            }

            { // PLAYER2 POWER UP ICON
                auto& componentTransform = canvas.lRectangles[7];
                auto& base = componentTransform.base;

                componentTransform.id =  20;

                base.anchor		= RECTANGLE::Anchor		{ 1.f, 1.f };
                base.position	= RECTANGLE::Position	{ -127.0f, -218.0f };
                base.size		= RECTANGLE::Size		{ 127.0f, 218.0f };
                base.pivot		= RECTANGLE::Pivot		{ 63.5f, 109.0f };
                base.rotation	= RECTANGLE::Rotation	{ 0.0f };
                base.scale		= RECTANGLE::Scale		{ 1.0f, 1.0f };
            }

            { // PLAYER1 CHARGE INDICATOR
                auto& componentTransform = canvas.lRectangles[8];
                auto& base = componentTransform.base;

                componentTransform.id =  21;

                base.anchor		= RECTANGLE::Anchor		{ 0.f, 0.f };
                base.position	= RECTANGLE::Position	{ 0.0f, 0.0f };
                base.size		= RECTANGLE::Size		{ 150.0f, 150.0f };
                base.pivot		= RECTANGLE::Pivot		{ 75.0f, 75.0f };
                base.rotation	= RECTANGLE::Rotation	{ 0.0f };
                base.scale		= RECTANGLE::Scale		{ 1.0f, 1.0f };
            }

            { // PLAYER2 CHARGE INDICATOR
                auto& componentTransform = canvas.lRectangles[9];
                auto& base = componentTransform.base;

                componentTransform.id =  22;

                base.anchor		= RECTANGLE::Anchor		{ 1.f, 0.f };
                base.position	= RECTANGLE::Position	{ -150.0f, 0.0f };
                base.size		= RECTANGLE::Size		{ 150.0f, 150.0f };
                base.pivot		= RECTANGLE::Pivot		{ 75.0f, 75.0f };
                base.rotation	= RECTANGLE::Rotation	{ 0.0f };
                base.scale		= RECTANGLE::Scale		{ 1.0f, 1.0f };
            }

            { // PLAYER1 POWER UP TEXT
                auto& componentTransform = canvas.lRectangles[10];
                auto& base = componentTransform.base;

                componentTransform.id = 23;

                base.anchor		= RECTANGLE::Anchor		{ 0.f, 1.f };
                base.position	= RECTANGLE::Position	{ 0.0f, -75.0f };
                base.size		= RECTANGLE::Size		{ 50.0f, 50.0f };
                base.pivot		= RECTANGLE::Pivot		{ 0.0f, 0.0f };
                base.rotation	= RECTANGLE::Rotation	{ 0.0f };
                base.scale		= RECTANGLE::Scale		{ 0.4f, 0.4f };
            }

            { // PLAYER2 POWER UP TEXT
                auto& componentTransform = canvas.lRectangles[11];
                auto& base = componentTransform.base;

                componentTransform.id = 24;

                base.anchor		= RECTANGLE::Anchor		{ 1.f, 1.f };
                base.position	= RECTANGLE::Position	{ -175.0f, -75.0f };
                base.size		= RECTANGLE::Size		{ 50.0f, 50.0f };
                base.pivot		= RECTANGLE::Pivot		{ 0.0f, 0.0f };
                base.rotation	= RECTANGLE::Rotation	{ 0.0f };
                base.scale		= RECTANGLE::Scale		{ 0.4f, 0.4f };
            }

            { // ACTIVE POWER UP
                auto& componentTransform = canvas.lRectangles[12];
                auto& base = componentTransform.base;

                componentTransform.id = 25;

                base.anchor		= RECTANGLE::Anchor		{ 0.5f, 1.f };
                base.position	= RECTANGLE::Position	{ -63.5f, -218.0f };
                base.size		= RECTANGLE::Size		{ 127.0f, 218.0f };
                base.pivot		= RECTANGLE::Pivot		{ 0.0f, 0.0f };
                base.rotation	= RECTANGLE::Rotation	{ 0.0f };
                base.scale		= RECTANGLE::Scale		{ 1.f, 1.f };
            }

            { // ACTIVE POWER UP TEXT
                auto& componentTransform = canvas.lRectangles[13];
                auto& base = componentTransform.base;

                componentTransform.id = 26;

                base.anchor		= RECTANGLE::Anchor		{ 0.5f, 1.f };
                base.position	= RECTANGLE::Position	{ -55.0f, -100.0f };
                base.size		= RECTANGLE::Size		{ 127.0f, 218.0f };
                base.pivot		= RECTANGLE::Pivot		{ 0.0f, 0.0f };
                base.rotation	= RECTANGLE::Rotation	{ 0.0f };
                base.scale		= RECTANGLE::Scale		{ 0.4f, 0.4f };
            }

            { // PLAYER1 POPUP TEXT
                auto& componentTransform = canvas.lRectangles[14];
                auto& base = componentTransform.base;

                componentTransform.id = 27;

                base.anchor		= RECTANGLE::Anchor		{ 0.25f, 0.5f };
                base.position	= RECTANGLE::Position	{ -125.0f, -40.0f };
                base.size		= RECTANGLE::Size		{ 100.0f, 100.0f };
                base.pivot		= RECTANGLE::Pivot		{ 0.0f, 0.0f };
                base.rotation	= RECTANGLE::Rotation	{ 0.0f };
                base.scale		= RECTANGLE::Scale		{ 0.5f, 0.5f };
            }

            { // PLAYER2 POPUP TEXT
                auto& componentTransform = canvas.lRectangles[15];
                auto& base = componentTransform.base;

                componentTransform.id = 28;

                base.anchor		= RECTANGLE::Anchor		{ 0.75f, 0.5f };
                base.position	= RECTANGLE::Position	{ -125.0f, -40.0f };
                base.size		= RECTANGLE::Size		{ 100.0f, 100.0f };
                base.pivot		= RECTANGLE::Pivot		{ 0.0f, 0.0f };
                base.rotation	= RECTANGLE::Rotation	{ 0.0f };
                base.scale		= RECTANGLE::Scale		{ 0.5f, 0.5f };
            }

            { // RETURN BUTTON
                auto& componentTransform = canvas.lRectangles[16];
                auto& base = componentTransform.base;

                componentTransform.id = 29;

                base.anchor		= RECTANGLE::Anchor		{ 0.5f, 0.3f };
                base.position	= RECTANGLE::Position	{ -132.0f, -31.0f };
                base.size		= RECTANGLE::Size		{ 264.0f, 62.0f };
                base.pivot		= RECTANGLE::Pivot		{ 0.0f, 0.0f };
                base.rotation	= RECTANGLE::Rotation	{ 0.0f };
                base.scale		= RECTANGLE::Scale		{ 1.f, 1.f };
            }

            { // RETURN BUTTON TEXT
                auto& componentTransform = canvas.lRectangles[17];
                auto& base = componentTransform.base;

                componentTransform.id = 30;

                base.anchor		= RECTANGLE::Anchor		{ 0.5f, 0.3f };
                base.position	= RECTANGLE::Position	{ -40.0f, -10.0f };
                base.size		= RECTANGLE::Size		{ 100.0f, 100.0f };
                base.pivot		= RECTANGLE::Pivot		{ 0.0f, 0.0f };
                base.rotation	= RECTANGLE::Rotation	{ 0.0f };
                base.scale		= RECTANGLE::Scale		{ 0.8f, 0.8f };
            }

            { // SUMMARY
                auto& componentTransform = canvas.lRectangles[18];
                auto& base = componentTransform.base;

                componentTransform.id = 31;

                base.anchor		= RECTANGLE::Anchor		{ 0.5f, 0.5f };
                base.position	= RECTANGLE::Position	{ -250.0f, -350.0f };
                base.size		= RECTANGLE::Size		{ 500.0f, 700.0f };
                base.pivot		= RECTANGLE::Pivot		{ 0.0f, 0.0f };
                base.rotation	= RECTANGLE::Rotation	{ 0.0f };
                base.scale		= RECTANGLE::Scale		{ 1.f, 1.f };
            }

            { // SUMMARY TEXT
                auto& componentTransform = canvas.lRectangles[19];
                auto& base = componentTransform.base;

                componentTransform.id = 32;

                base.anchor		= RECTANGLE::Anchor		{ 0.5f, 0.5f };
                base.position	= RECTANGLE::Position	{ -200.0f, 100.0f };
                base.size		= RECTANGLE::Size		{ 100.0f, 100.0f };
                base.pivot		= RECTANGLE::Pivot		{ 0.0f, 0.0f };
                base.rotation	= RECTANGLE::Rotation	{ 0.0f };
                base.scale		= RECTANGLE::Scale		{ 1.f, 1.f };
            }
		}
	}


	void CreateLoadTextures (
		SCENE::Skybox& skybox,
		SCENE::SHARED::Screen& sharedScreen, 
		SCENE::SHARED::Canvas& sharedCanvas, 
		SCENE::SHARED::World& sharedWorld
	) {
		{ // TEXTURE
			const TEXTURE::Atlas dustsAtlas	   { 6, 6, 1, 16, 16 }; // elements, cols, rows, tile_pixels_x, tile_pixels_y
			const TEXTURE::Atlas writtingAtlas { 6, 5, 2, 64, 64 };

			// SCREEN
			auto& textureS0 = sharedScreen.materials[0].texture;
			auto& textureS1 = sharedScreen.materials[1].texture;
			auto& textureS2 = sharedScreen.materials[2].texture;
			// CANVAS
			auto& textureC1_1 = sharedCanvas.materials[1].texture1;
            auto& textureC1_2 = sharedCanvas.materials[1].texture2;
            auto& textureC2_1 = sharedCanvas.materials[2].texture;
            auto& textureC2_2 = sharedCanvas.materials[2].texture2;
            auto& textureC3 = sharedCanvas.materials[3].texture;
            auto& textureC4_0 = sharedCanvas.materials[4].texture;
            auto& textureC4_1 = sharedCanvas.materials[4].texture1;
            auto& textureC4_2 = sharedCanvas.materials[4].texture2;
            auto& textureC4_3 = sharedCanvas.materials[4].texture3;
            auto& textureC5_1 = sharedCanvas.materials[5].texture;
            auto& textureC5_2 = sharedCanvas.materials[5].texture2;
            auto& textureC6_0 = sharedCanvas.materials[6].texture;
            auto& textureC6_1 = sharedCanvas.materials[6].texture1;
            auto& textureC6_2 = sharedCanvas.materials[6].texture2;
            auto& textureC6_3 = sharedCanvas.materials[6].texture3;

			// WORLD
			auto& textureW0 = sharedWorld.materials[3].texture;
			auto& textureW1 = sharedWorld.materials[6].texture;
			
			// Don't overuse memory allocations.
			TEXTURE::HolderCube textureCubeHolder;
			TEXTURE::Holder& textureHolder = textureCubeHolder[0];

			{ // SKYBOX
				for (u8 i = 0; i < TEXTURE::CUBE_FACES_COUNT; ++i) {
					TEXTURE::Load (textureCubeHolder[i], RESOURCES::MANAGER::SKYBOX_NIGHT[i]);
					//TEXTURE::Load (textureCubeHolder[i], RESOURCES::MANAGER::SKYBOX_DEFAULT[i]);
				}
				TEXTURE::CUBEMAP::Create (skybox.texture, textureCubeHolder, TEXTURE::PROPERTIES::defaultRGB);
			}

			stbi_set_flip_vertically_on_load (true);

			TEXTURE::Load (textureHolder, RESOURCES::MANAGER::TEXTURE_BRICK);
			TEXTURE::SINGLE::Create (textureS0, textureHolder, TEXTURE::PROPERTIES::defaultRGB);

			//TEXTURE::Load (textureHolder, RESOURCES::MANAGER::TEXTURE_EARTH);
			//TEXTURE::SINGLE::Create (texture0, textureHolder, TEXTURE::PROPERTIES::defaultRGB);

			TEXTURE::Load (textureHolder, RESOURCES::MANAGER::TEXTURE_TIN_SHEARS);
			TEXTURE::SINGLE::Create (textureS1, textureHolder, TEXTURE::PROPERTIES::defaultRGB);

			TEXTURE::Load (textureHolder, RESOURCES::MANAGER::ANIMATED_TEXTURE_2);
			TEXTURE::ARRAY::Create (textureS2, textureHolder, TEXTURE::PROPERTIES::alphaPixelNoMipmap, writtingAtlas);

			TEXTURE::Load (textureHolder, RESOURCES::MANAGER::TEXTURE_EARTH);
			TEXTURE::SINGLE::Create (textureW1, textureHolder, TEXTURE::PROPERTIES::defaultRGB);

            TEXTURE::Load (textureHolder, RESOURCES::MANAGER::BUTTON);
            TEXTURE::SINGLE::Create (textureC1_1, textureHolder, TEXTURE::PROPERTIES::defaultRGBA);

            TEXTURE::Load (textureHolder, RESOURCES::MANAGER::BUTTON2);
            TEXTURE::SINGLE::Create (textureC1_2, textureHolder, TEXTURE::PROPERTIES::defaultRGBA);

            TEXTURE::Load (textureHolder, RESOURCES::MANAGER::PLAYER1_ICON_TEXTURE);
            TEXTURE::SINGLE::Create (textureC2_1, textureHolder, TEXTURE::PROPERTIES::defaultRGBA);

            TEXTURE::Load (textureHolder, RESOURCES::MANAGER::PLAYER2_ICON_TEXTURE);
            TEXTURE::SINGLE::Create (textureC2_2, textureHolder, TEXTURE::PROPERTIES::defaultRGBA);

            TEXTURE::Load (textureHolder, RESOURCES::MANAGER::TOWER_MINIMAP_TEXTURE);
            TEXTURE::SINGLE::Create (textureC3, textureHolder, TEXTURE::PROPERTIES::defaultRGBA);

            TEXTURE::Load (textureHolder, RESOURCES::MANAGER::POWER_UP_ICON_NONE);
            TEXTURE::SINGLE::Create (textureC4_0, textureHolder, TEXTURE::PROPERTIES::defaultRGBA);

            TEXTURE::Load (textureHolder, RESOURCES::MANAGER::POWER_UP_ICON_BOUNCE);
            TEXTURE::SINGLE::Create (textureC4_1, textureHolder, TEXTURE::PROPERTIES::defaultRGBA);

            TEXTURE::Load (textureHolder, RESOURCES::MANAGER::POWER_UP_ICON_SPEED);
            TEXTURE::SINGLE::Create (textureC4_2, textureHolder, TEXTURE::PROPERTIES::defaultRGBA);

            TEXTURE::Load (textureHolder, RESOURCES::MANAGER::POWER_UP_ICON_GHOST);
            TEXTURE::SINGLE::Create (textureC4_3, textureHolder, TEXTURE::PROPERTIES::defaultRGBA);

			textureW0 = textureS0;
            textureC5_1 = textureC2_1;
            textureC5_2 = textureC2_2;
            textureC6_0 = textureC4_0;
            textureC6_1 = textureC4_1;
            textureC6_2 = textureC4_2;
            textureC6_3 = textureC4_3;
		}
	}

}
