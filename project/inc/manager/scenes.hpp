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
			gltfWorld.lTransforms[0].base.position.x = -5.0f;
			gltfWorld.lTransforms[0].base.position.y = 1.0f;
			gltfWorld.lTransforms[0].base.position.z = 1.0f;
			//
			gltfWorld.lTransforms[0].base.rotation.x = 15.0f;
			gltfWorld.lTransforms[0].base.rotation.y = 15.0f;
			gltfWorld.lTransforms[0].base.rotation.z = 15.0f;
		}

		{ // root's transform change.
			auto& gltfWorld = MANAGER::OBJECTS::GLTF::worlds[1];
			//
			gltfWorld.lTransforms[0].base.position.x = 5.0f;
			gltfWorld.lTransforms[0].base.position.y = 3.0f;
			gltfWorld.lTransforms[0].base.position.z = 1.0f;
			//
			gltfWorld.lTransforms[0].base.rotation.x = 0.0f;
			gltfWorld.lTransforms[0].base.rotation.y = 45.0f;
			gltfWorld.lTransforms[0].base.rotation.z = 0.0f;
		}

		MANAGER::OBJECTS::GLTF::Set ();
	}

}



namespace MANAGER::SCENES::MAIN {

	// This should be read from the json scene file instead.
	void Create (
		SCENE::Canvas& canvas,
		SCENE::Screen& screen,
		SCENE::World& world
	) {
		// SCREEN
		screen.parenthoodsCount = 0; 
		screen.transformsCount = 1;

		// CANVAS
		canvas.parenthoodsCount = 0; 
		canvas.rectanglesCount = 3;
		canvas.buttonsCount = 1;
		canvas.collidersCount[COLLIDER::ColliderGroup::UI] = 1;
		
		// WORLD
		world.collidersCount[COLLIDER::ColliderGroup::CAMERA]	= 2;
		world.collidersCount[COLLIDER::ColliderGroup::MAP]		= 0;
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
			/*rangePosition*/ 0.5f,
			/*rangeWidth*/    1.0f
		};

		MAP_GENERATOR::Modifiers modifiers {
			/*levelLength*/ 				5,
			/*stationaryTrapsAmount*/ 		0.5f,
			/*pushingTrapsAmount*/ 			5,
			/*checkpointsSpacing*/          2,
			/*parkourDifficulty*/ 			difficulty,
			/*windingModuleProbability*/	0.5f
		};

		mapGenerator = new MAP_GENERATOR::MapGenerator;
		mapGenerator->modifiers = modifiers;

		MAP_GENERATOR::LoadModules (mapGenerator, RESOURCES::MANAGER::SEGMENTS);
		MAP_GENERATOR::GenerateLevel (mapGenerator);
			
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
				cWorld.rotatingsCount, cWorld.rotatings
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

        u16 giCollider = 2; // HACK, wall is 1st, WindowTrap is 2nd.
		for (u16 iSegment = 0; iSegment < segmentsCount; ++iSegment) {
			
			auto& worldColliders = world.colliders[COLLIDER::ColliderGroup::MAP];

			auto& segment = segmentsWorld[iSegment];
			auto& collidersCount = segment.collidersCount[COLLIDER::ColliderGroup::MAP];

			for (u16 iCollider = 0; iCollider < collidersCount; ++iCollider) {
				
				auto& componentCollider = worldColliders[giCollider];
				auto& base = componentCollider.local;

				base.group = COLLIDER::ColliderGroup::MAP;
        		base.type = COLLIDER::ColliderType::OBB;

				const u16 hackOffset = 15; // HACK
				componentCollider.id = hackOffset + (iSegment * segmentsCount) + iCollider + 1; // It simply should refer to segments collisions

				COLLIDER::InitializeColliderSize (componentCollider, sharedWorld.meshes[0], segment.gTransforms[iCollider + 1]); // HACK +1 to skip root transform
				++giCollider;
			}
		}


		u16 springTrapsCount = 0;
        u16 checkpointsCount = 1;
        u16 giTriggerCollider = 3; // HACK, wall is 1st. power up is 2nd, windowTrap is 3rd
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
                    base.collisionEventName = "SpringTrap";
                    springTrapsCount ++;
                }
                else if(iCollider == 1)
                {
                    base.collisionEventName = "CheckPoint";
                    world.checkpoints[checkpointsCount].position = segment.gTransforms[segment.transformsCount-collidersCount+iCollider][3];
                    world.checkpoints[checkpointsCount].id = componentCollider.id;
                    checkpointsCount ++;
                }

                COLLIDER::InitializeColliderSize (componentCollider, sharedWorld.meshes[0], segment.gTransforms[segment.transformsCount-collidersCount+iCollider]); // HACK +1 to skip root transform
                ++giTriggerCollider;
            }
        }


		TrapGeneration (mapGenerator, springTrapsCount);

        ApplyTraps (
			mapGenerator, world.colliders[COLLIDER::ColliderGroup::TRIGGER], 
			world.collidersCount[COLLIDER::ColliderGroup::TRIGGER], segmentsWorld
		);

        CheckpointsGeneration (mapGenerator, checkpointsCount);

        ApplyCheckpoints (
			mapGenerator, world.colliders[COLLIDER::ColliderGroup::TRIGGER], 
			world.collidersCount[COLLIDER::ColliderGroup::TRIGGER], segmentsWorld
		);
	}

}