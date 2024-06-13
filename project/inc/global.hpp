#pragma once
#include "types.hpp"

#include "tool/imgui.hpp"

#if PLATFORM == PLATFORM_WINDOWS
#include "platform/win/types.hpp"
#else
#include "platform/agn/types.hpp"
#endif

#include "resources/manager.hpp"
#include "resources/materials.hpp"
#include "resources/shaders.hpp"
#include "resources/meshes.hpp"
#include "resources/location.hpp"
#include "resources/gltf.hpp"

#include "scene.hpp"
#include "viewport.hpp"
#include "object.hpp"
#include "render/texture.hpp"

#include "components/ui/uiManager.hpp"
#include "components/collisions/collisionManager.hpp"
#include "player/playerMovement.hpp"
#include "components/collisions/collisionsDetection.hpp"
#include "generator/mapGenerator.hpp"
#include "components/traps/springTrap.hpp"
#include "components/checkpoints/checkPointManager.hpp"


#ifdef DEBUG_TOKEN
namespace GLOBAL::EDITOR {
	s32 editedObject = 6;			// ???
}
#endif

namespace GLOBAL {

	Color4 backgroundColor = Color4 ( 114.0f / 255.0f, 144.0f / 255.0f, 154.0f / 255.0f, 200.0f / 255.0f );

	// time of the previous frame
	// time of the the the the the the the the the the
	double timeSinceLastFrame = 0, timeCurrent = 0, timeDelta = 0;

	WIN::WindowTransform windowTransform { 0, 0, 1200, 640 }; // pos.x, pos.y, size.x, size.y


	VIEWPORT::Viewport* viewports;
	s32 viewportsCount = 2;
    CHECKPOINT::MANAGER::Manager checkpointManager{};
    r32* checkpointTimers;
    s32* checkpointIndexes;
    r32 timeToCreateCheckpoint{5.0f};

	//Prepare starting mouse positions
	float lastX = windowTransform[2] / 2.0f;
	float lastY = windowTransform[3] / 2.0f;
	
	INPUT_MANAGER::IM inputManager = nullptr;
	HID_INPUT::Input input = nullptr;
	WIN::Window mainWindow = nullptr;

	UI::MANAGER::UIM uiManager = nullptr;
    COLLISION::MANAGER::CM collisionManager = nullptr;
	MAP_GENERATOR::MG mapGenerator = nullptr;

    glm::mat4 camTransform{};
    glm::vec3 camCollisionOffset{};
	ANIMATION::Animation sharedAnimation1 { 1.0f, 6, 0, 0.0f, 0 };

	// --------------------

	glm::vec3 lightPosition = glm::vec3(1.0f, 1.0f, 1.0f);

	// SET DURING INITIALIZATION
	SCENE::SHARED::Screen sharedScreen;
	SCENE::SHARED::Canvas sharedCanvas;
	SCENE::SHARED::World sharedWorld;
	SCENE::Scene scene;

	SCENE::Screen screen { 0 };
	SCENE::Canvas canvas { 0 };
	SCENE::Skybox skybox { 0 };
	SCENE::World world   { 0 };

	u8 segmentsCount = 0;
	SCENE::World* segmentsWorld = nullptr;

	// GLTF .. for now ..
	SCENE::SHARED::World gltfSharedWorld[RESOURCES::MANAGER::GLTFS::HANDLERS_COUNT];
	SCENE::World gltfWorld[RESOURCES::MANAGER::GLTFS::HANDLERS_COUNT];

    POWER_UP::PowerUp activePowerUp;


	AUDIO::IO::WAV::Wav music {};
	AUDIO::IO::WAV::Wav springTrapActivate {};
	AUDIO::IO::WAV::Wav fallImpact {};
	AUDIO::IO::WAV::Wav jump {};
	AUDIO::IO::WAV::Wav checkpoint {};
	AUDIO::IO::WAV::Wav victory {};

	// mono!
	ALuint sounds[6];
	ALuint musicSource;

	void CreateSounds () {
		auto& musicSound = sounds[0];

		AUDIO::IO::WAV::Load 		(RESOURCES::MANAGER::AUDIO_WAV_TEST, GLOBAL::music);
		AUDIO::SOUND::CreateMono 	(musicSound, GLOBAL::music);

		AUDIO::IO::WAV::Load 		(RESOURCES::MANAGER::AUDIO_WAV_SPRINT_TRAP_ACTIVATE, GLOBAL::springTrapActivate);
		AUDIO::SOUND::CreateMono 	(sounds[1], GLOBAL::springTrapActivate);

		AUDIO::IO::WAV::Load 		(RESOURCES::MANAGER::AUDIO_WAV_FALL_IMPACT, GLOBAL::fallImpact);
		AUDIO::SOUND::CreateMono 	(sounds[2], GLOBAL::fallImpact);

		AUDIO::IO::WAV::Load 		(RESOURCES::MANAGER::AUDIO_WAV_JUMP, GLOBAL::jump);
		AUDIO::SOUND::CreateMono 	(sounds[3], GLOBAL::jump);

		AUDIO::IO::WAV::Load 		(RESOURCES::MANAGER::AUDIO_WAV_NEW_CHECKPOINT, GLOBAL::checkpoint);
		AUDIO::SOUND::CreateMono 	(sounds[4], GLOBAL::checkpoint);

		AUDIO::IO::WAV::Load 		(RESOURCES::MANAGER::AUDIO_WAV_VICTORY, GLOBAL::victory);
		AUDIO::SOUND::CreateMono 	(sounds[5], GLOBAL::victory);
	}

	void DestroySounds () {
		auto& musicSound = sounds[0];

		AUDIO::SOUND::Destroy (musicSound);
		AUDIO::IO::WAV::Destory (GLOBAL::music);

		AUDIO::SOUND::Destroy (sounds[1]);
		AUDIO::IO::WAV::Destory (GLOBAL::springTrapActivate);

		AUDIO::SOUND::Destroy (sounds[2]);
		AUDIO::IO::WAV::Destory (GLOBAL::fallImpact);

		AUDIO::SOUND::Destroy (sounds[3]);
		AUDIO::IO::WAV::Destory (GLOBAL::jump);

		AUDIO::SOUND::Destroy (sounds[4]);
		AUDIO::IO::WAV::Destory (GLOBAL::checkpoint);

		AUDIO::SOUND::Destroy (sounds[5]);
		AUDIO::IO::WAV::Destory (GLOBAL::victory);
	}

	void CreateGlobalSources () {
		// SOUND
		auto& musicSound = sounds[0];
		// SOURCE
		AUDIO::SOURCE::CreateGlobalMono (musicSource, musicSound, true, 1.0f, 1.0f);
		// STATE
        AUDIO::STATE::Play 				(musicSource);
	}

	void CreateSource (const ALuint& sound, const AUDIO::float3& position) {
		auto& source = sources[sourcesCounter];
		++sourcesCounter;

		AUDIO::SOURCE::CreateMono 		(source, sound, position, AUDIO::ZERO, false, 1.0f, 1.0f);
	}

	void DestroySources () {
		AUDIO::STATE::Stop (musicSource);
		AUDIO::SOURCE::Destroy (musicSource);

		for (u8 i = 0; i < sourcesCounter; ++i) {
			auto& source = sources[i];
			AUDIO::STATE::Stop (source);
			AUDIO::SOURCE::Destroy (source);
		}
	}

	// INITIALIZATION STAGES
	// 1. SET ( set how many specific components there will be )
	// 2. PARSE (change from file format to nlohman/json format )
	// 3. CREATE ( allocate memory for each component )
	// 4. LOAD ( load default data to each created component )


	void Initialize () {
		PROFILER { ZoneScopedN("GLOBAL: Initialize"); }
		// Make a debug_directive later...
		// DEBUG GL::GetSpecification ();

		// It's all Data Layer, Memory allocations, Pointer assignments.
		RESOURCES::Json materialsJson;
		RESOURCES::Json meshesJson;
		RESOURCES::Json sceneJson;
		SCENE::SceneLoadContext sceneLoad { 0 };

		RESOURCES::Json* segmentsJson = nullptr;
		SCENE::SceneLoadContext* segmentLoad = nullptr;
		
		{ // SCREEN
			screen.parenthoodsCount = 0; 
			screen.transformsCount = 1; // must be 1! (for root)
		}

		{ // CANVAS
			canvas.parenthoodsCount = 0; 
			canvas.rectanglesCount = 3;
			canvas.buttonsCount = 1;
			canvas.collidersCount[COLLIDER::ColliderGroup::UI] = 1;
		}
		
		{ // WORLD
			// Remove them for now. -> Scene Loading 12.05.2024.
			//world.collidersCount[COLLIDER::ColliderGroup::PLAYER]	= 2;
            world.collidersCount[COLLIDER::ColliderGroup::CAMERA]	= 2;
            world.collidersCount[COLLIDER::ColliderGroup::MAP]		= 0;
            //world.collidersCount[COLLIDER::ColliderGroup::TRIGGER]	= 1;
			//world.collidersCount[COLLIDER::ColliderGroup::PLAYER]	= 0;
			//world.collidersCount[COLLIDER::ColliderGroup::MAP]		= 0;
			//world.rotatingsCount									= 2;
		}

		DEBUG_ENGINE { spdlog::info ("Creating map generator."); }

		{
			MAP_GENERATOR::ParkourDifficulty difficulty {
				/*rangePosition*/ 0.5f,
				/*rangeWidth*/    1.0f
			};

			MAP_GENERATOR::Modifiers modifiers {
				/*levelLength*/ 				5,
				/*stationaryTrapsAmount*/ 		0.75f,
				/*pushingTrapsAmount*/ 			5,
				/*parkourDifficulty*/ 			difficulty,
				/*windingModuleProbability*/	0.5f
			};

			uiManager = new UI::MANAGER::UIManager;
            collisionManager = new COLLISION::MANAGER::CollisionManager;

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

			// HACK!
            //world.collidersCount[COLLIDER::ColliderGroup::MAP]	= segmentsCount * 6;
			DEBUG spdlog::info ("mapColliders: {0}", segmentsCount * 6);
		}

		DEBUG_ENGINE { spdlog::info ("Creating Viewports."); }

		// allocation
		if (viewportsCount) viewports = new VIEWPORT::Viewport[viewportsCount];

			{ // Viewport 1
				auto& viewport = viewports[0];
				auto& camera = viewport.camera;

				camera.local.position 			= glm::vec3 (2.0f, 0.0f, 8.0f);
				camera.local.worldUp			= glm::vec3 (0.0f, 1.0f, 0.0f);
				camera.local.front				= glm::vec3 (0.0f, 0.0f, -1.0f);
                camera.type						= CAMERA::CameraType::THIRD_PERSON;
				camera.local.yaw				= CAMERA::YAW;
				camera.local.pitch				= CAMERA::PITCH;
				camera.local.zoom				= CAMERA::ZOOM;
				camera.local.distance			= CAMERA::DIST_FROM_TARGET;
				camera.local.mouseSensitivity	= CAMERA::SENSITIVITY;
				camera.local.moveSpeed			= CAMERA::SPEED;

				CAMERA::UpdateCameraVectors (camera);

			}

			{ // Viewport 2
				auto& viewport = viewports[1];
				auto& camera = viewport.camera;

				camera.local.position			= glm::vec3 (2.0f, 0.0f, 8.0f);
				camera.local.worldUp			= glm::vec3 (0.0f, 1.0f, 0.0f);
				camera.local.front				= glm::vec3 (0.0f, 0.0f, -1.0f);
                camera.type						= CAMERA::CameraType::THIRD_PERSON;
				camera.local.yaw				= CAMERA::YAW;
				camera.local.pitch				= CAMERA::PITCH;
				camera.local.zoom				= CAMERA::ZOOM;
				camera.local.mouseSensitivity	= CAMERA::SENSITIVITY;
				camera.local.moveSpeed			= CAMERA::SPEED;

				CAMERA::UpdateCameraVectors (camera);
			}

		DEBUG { spdlog::info ("Allocating memory for components and collections."); }

        world.modelsCount = 1;
        world.models = new MODEL::Model[world.modelsCount]{ nullptr };

        RESOURCES::MANAGER::LoadModels (world.modelsCount, world.models);

		DEBUG { spdlog::info ("MODELRS LOADED!"); }

		RESOURCES::Parse (materialsJson, RESOURCES::MANAGER::MATERIALS);

		RESOURCES::MATERIALS::CreateMaterials (
			materialsJson,
			//
			sharedScreen.loadTables.shaders, sharedScreen.tables.uniforms, screen.tables.meshes, 
			sharedScreen.materialsCount, sharedScreen.materials,
			//
			sharedCanvas.loadTables.shaders, sharedCanvas.tables.uniforms, canvas.tables.meshes, 
			sharedCanvas.materialsCount, sharedCanvas.materials,
			//
			sharedWorld.loadTables.shaders, sharedWorld.tables.uniforms,
			sharedWorld.materialsCount, sharedWorld.materials
		);

		// [TODO] Meshes file, read using json...

		RESOURCES::MESHES::CreateMeshes (
			meshesJson,
			sharedScreen.meshesCount, sharedScreen.meshes,
			sharedCanvas.meshesCount, sharedCanvas.meshes,
			sharedWorld.meshesCount, sharedWorld.meshes
		);

		{ // Loading main.
			
			const u8 DIFFICULTY = 4; // 0 - 4 (5)
			const u8 EXIT_TYPE = 2;  // 0 - 2 (3)
			// NOW ALWAYS CONSTANT "height": 48
			// NOW ALWAYS CONSTANT "platform_count": 0
			// NOW ALWAYS CONSTANT "trap_count": 0
			
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
		}

		for (u8 iSegment = 0; iSegment < segmentsCount; ++iSegment) { // Loading additional.
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
			auto& fileJson = segmentsJson[iSegment];
			auto& loadHelper = segmentLoad[iSegment];
			auto& cWorld = segmentsWorld[iSegment];
			
			DEBUG if (segment.parkourDifficulty < 1.0f || segment.parkourDifficulty > 10.0f) {
				spdlog::error ("Segment difficulty ({0}) set to an invalid value!", segment.parkourDifficulty);
				exit (1);
			}

			const u8 DIFFICULTY = (u8)segment.parkourDifficulty;
			const u8 EXIT_TYPE = MAP_GENERATOR::ModuleTypeToInt(segment.type); 						// 1;  // 0 - 2 (3)

			RESOURCES::Parse (fileJson, RESOURCES::MANAGER::SCENES::SEGMENTS[MAP_GENERATOR::CalculateSegmentIndex(mapGenerator, DIFFICULTY, EXIT_TYPE)]);
			
			// map key is 64bit... we cast it to smaller type...
			auto& collidersMapCount 			= *(u16*)(void*)(&world.collidersCount[COLLIDER::ColliderGroup::MAP]);
			auto& collidersTriggerCount 		= *(u16*)(void*)(&world.collidersCount[COLLIDER::ColliderGroup::TRIGGER]);
			auto& collidersPlayerCount 			= *(u16*)(void*)(&world.collidersCount[COLLIDER::ColliderGroup::PLAYER]);

			// SEGMENT's properties.
			auto& collidersSegmentMapCount 		= *(u16*)(void*)(&cWorld.collidersCount[COLLIDER::ColliderGroup::MAP]);
			auto& collidersSegmentTriggerCount 	= *(u16*)(void*)(&cWorld.collidersCount[COLLIDER::ColliderGroup::TRIGGER]);
			auto& collidersSegmentPlayerCount 	= *(u16*)(void*)(&cWorld.collidersCount[COLLIDER::ColliderGroup::PLAYER]);

			u16 var4 = 0;
			u16 var5 = 0;

			DEBUG_ENGINE { spdlog::info ("JSON {0} Segment-Scene Initialization", iSegment); }

			RESOURCES::SCENE::Create (
				fileJson,
				sharedWorld.materialsCount, sharedWorld.meshesCount, 					// Already set
				cWorld.tables.meshes, cWorld.tables.parenthoodChildren, 				// Tables
				loadHelper.relationsLookUpTable, cWorld.transformsOffset,				// Helper Logic + what we get
				cWorld.parenthoodsCount, cWorld.transformsCount, world.rotatingsCount,	// What we actually get.
				collidersSegmentMapCount, collidersSegmentTriggerCount, collidersSegmentPlayerCount, 
				var4, var5
			);

			collidersMapCount += collidersSegmentMapCount;
            collidersTriggerCount += collidersSegmentTriggerCount;
		}

		//world.collidersCount[COLLIDER::ColliderGroup::MAP] += 20; // HACK
		DEBUG spdlog::info ("mapColliders: {0}", world.collidersCount[COLLIDER::ColliderGroup::MAP]);
        DEBUG spdlog::info ("triggerColliders: {0}", world.collidersCount[COLLIDER::ColliderGroup::TRIGGER]);

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

			if (world.collidersCount[COLLIDER::ColliderGroup::PLAYER]) 
				world.colliders[COLLIDER::ColliderGroup::PLAYER] = new COLLIDER::Collider[world.collidersCount[COLLIDER::ColliderGroup::PLAYER]] { 0 };
			if (world.collidersCount[COLLIDER::ColliderGroup::MAP]) 
				world.colliders[COLLIDER::ColliderGroup::MAP] = new COLLIDER::Collider[world.collidersCount[COLLIDER::ColliderGroup::MAP]] { 0 };
            if (world.collidersCount[COLLIDER::ColliderGroup::TRIGGER]) 
				world.colliders[COLLIDER::ColliderGroup::TRIGGER] = new COLLIDER::Collider[world.collidersCount[COLLIDER::ColliderGroup::TRIGGER]] { 0 };
            if (canvas.collidersCount[COLLIDER::ColliderGroup::UI]) 
				canvas.colliders[COLLIDER::ColliderGroup::UI] = new COLLIDER::Collider[canvas.collidersCount[COLLIDER::ColliderGroup::UI]] { 0 };
            if (world.collidersCount[COLLIDER::ColliderGroup::CAMERA]) 
				world.colliders[COLLIDER::ColliderGroup::CAMERA] = new COLLIDER::Collider[world.collidersCount[COLLIDER::ColliderGroup::CAMERA]] { 0 };

			if (world.rigidbodiesCount) world.rigidbodies = new RIGIDBODY::Rigidbody[world.rigidbodiesCount] { 0 };
        }

		{ // PLAYER
			if (world.playersCount) world.players = new PLAYER::Player[world.playersCount] { 0 };
		}

		DEBUG_ENGINE { spdlog::info ("Creating scene : parenthoods, transforms, meshTable."); }

        //Checkpoint
        {
            world.checkpointsCount = segmentsCount+1;
            checkpointManager.players = world.players;
            world.checkpoints = new CHECKPOINT::Checkpoint[world.checkpointsCount] {0};
            world.checkpoints[0].position = glm::vec3(0.f, -2.f, 0.f);
            checkpointManager.checkpoints = world.checkpoints;
            world.players[0].local.currentCheckpointIndex = 0;
            world.players[1].local.currentCheckpointIndex = 0;

            checkpointIndexes = new s32[world.playersCount]{-1, -1};
            checkpointTimers = new r32[world.playersCount]{-1.0f, -1.0f};
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

			{ // TEXT1
				auto& componentTransform = canvas.lRectangles[0];
				auto& base = componentTransform.base;

				componentTransform.id = 0;

				base.anchor		= RECTANGLE::Anchor		{ 0.0f, 0.0f };
				base.position	= RECTANGLE::Position	{ 25.0f, 25.0f };
				base.size		= RECTANGLE::Size		{ 100.0f, 100.0f };
				base.rotation	= RECTANGLE::Rotation	{ 0.0f };
				base.scale		= RECTANGLE::Scale		{ 1.0f, 1.0f };
			}

			{ // TEXT2
				auto& componentTransform = canvas.lRectangles[1];
				auto& base = componentTransform.base;

				componentTransform.id = 1;

				base.anchor		= RECTANGLE::Anchor		{ 1.0f, 1.0f };
				base.position	= RECTANGLE::Position	{ -300.0f, -100.0f };
				base.size		= RECTANGLE::Size		{ 100.0f, 100.0f };
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
				base.scale		= RECTANGLE::Scale		{ 1.0f, 1.0f };
			}

		}

		{ // MAIN
			RESOURCES::SCENE::Load (
				sceneJson, 
				sharedWorld.materialsCount, sharedWorld.meshesCount, 
				world.tables.meshes, world.tables.parenthoodChildren, 
				sceneLoad.relationsLookUpTable, world.transformsOffset,
				world.parenthoodsCount, world.parenthoods, 
				world.transformsCount, world.lTransforms,
				world.rotatingsCount, world.rotatings
			);
		}

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

        {
            u64 transformIndex = 0;
            OBJECT::GetComponentFast<TRANSFORM::LTransform>(transformIndex, world.transformsCount, world.lTransforms,
                                                            8);
            world.lTransforms[transformIndex].base.position.y = mapGenerator->modifiers.levelLength * 24.f + 0.5f;
        }

        DEBUG { spdlog::info ("Precalculating transfroms global position."); }

		{ // Precalculate Global Trnasfroms
			TRANSFORM::Precalculate (
					world.parenthoodsCount, world.parenthoods,
					world.transformsCount, world.lTransforms, world.gTransforms
			);
			TRANSFORM::Precalculate (
					screen.parenthoodsCount, screen.parenthoods,
					screen.transformsCount, screen.lTransforms, screen.gTransforms
			);
		}

		for (u8 iSegment = 0; iSegment < segmentsCount; ++iSegment) { // Precalculate Global Trnasfroms
			auto& cWorld = segmentsWorld[iSegment];
			TRANSFORM::Precalculate (
					cWorld.parenthoodsCount, cWorld.parenthoods,
					cWorld.transformsCount, cWorld.lTransforms, cWorld.gTransforms
			);
		}




        DEBUG_ENGINE { spdlog::info ("Creating textures."); }

		{ // TEXTURE
			const TEXTURE::Atlas dustsAtlas	   { 6, 6, 1, 16, 16 }; // elements, cols, rows, tile_pixels_x, tile_pixels_y
			const TEXTURE::Atlas writtingAtlas { 6, 5, 2, 64, 64 };

			// SCREEN
			auto& textureS0 = sharedScreen.materials[0].texture;
			auto& textureS1 = sharedScreen.materials[1].texture;
			auto& textureS2 = sharedScreen.materials[2].texture;
			// CANVAS
			auto& textureC1 = sharedCanvas.materials[1].texture;
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

			textureW0 = textureS0;
			textureC1 = textureW1;
		}

		DEBUG_ENGINE { spdlog::info ("Creating materials."); }

		RESOURCES::MATERIALS::LoadMaterials (
			materialsJson,
			sharedScreen.loadTables.shaders, sharedScreen.tables.uniforms, screen.tables.meshes, 
			sharedScreen.materialsCount, sharedScreen.materials,
			//
			sharedCanvas.loadTables.shaders, sharedCanvas.tables.uniforms, canvas.tables.meshes, 
			sharedCanvas.materialsCount, sharedCanvas.materials,
			//
			sharedWorld.loadTables.shaders, sharedWorld.tables.uniforms,
			sharedWorld.materialsCount, sharedWorld.materials
		);

		DEBUG_ENGINE { spdlog::info ("Creating shader programs."); }

		RESOURCES::SHADERS::Load ( 
			RESOURCES::MANAGER::SHADERS_SCREEN_SIZE, RESOURCES::MANAGER::SHADERS_SCREEN, 
			sharedScreen.loadTables.shaders, sharedScreen.tables.uniforms, sharedScreen.materials 
		);

		RESOURCES::SHADERS::Load ( 
			RESOURCES::MANAGER::SHADERS_CANVAS_SIZE, RESOURCES::MANAGER::SHADERS_CANVAS, 
			sharedCanvas.loadTables.shaders, sharedCanvas.tables.uniforms, sharedCanvas.materials
		);

		RESOURCES::SHADERS::Load ( 
			RESOURCES::MANAGER::SHADERS_WORLD_SIZE, RESOURCES::MANAGER::SHADERS_WORLD, 
			sharedWorld.loadTables.shaders, sharedWorld.tables.uniforms, sharedWorld.materials 
		);
		RESOURCES::SHADERS::LoadSkybox (skybox.shader);

		DEBUG_ENGINE { spdlog::info ("Creating meshes."); }

		u8* sInstancesCounts = (u8*) calloc (sharedScreen.meshesCount, sizeof (u8) );
		u8* cInstancesCounts = (u8*) calloc (sharedCanvas.meshesCount, sizeof (u8) );
		u8* wInstancesCounts = (u8*) calloc (sharedWorld.meshesCount, sizeof (u8) );

		{
			MATERIAL::MESHTABLE::GetMaxInstances (screen.tables.meshes, sInstancesCounts);
			MATERIAL::MESHTABLE::GetMaxInstances (canvas.tables.meshes, cInstancesCounts);
			MATERIAL::MESHTABLE::GetMaxInstances (world.tables.meshes, wInstancesCounts);

			for (u8 iSegment = 0; iSegment < segmentsCount; ++iSegment) { // Precalculate Global Trnasfroms
				auto& cWorld = segmentsWorld[iSegment];
				MATERIAL::MESHTABLE::GetMaxInstances (cWorld.tables.meshes, wInstancesCounts);
			}
		}

		RESOURCES::MESHES::LoadMeshes (
			meshesJson,
			sharedScreen.meshesCount, sharedScreen.meshes, sInstancesCounts,
			sharedCanvas.meshesCount, sharedCanvas.meshes, cInstancesCounts,
			sharedWorld.meshesCount, sharedWorld.meshes, wInstancesCounts,
			skybox.mesh
		);

		free (sInstancesCounts);
		free (cInstancesCounts);
		free (wInstancesCounts);

		DEBUG_ENGINE { spdlog::info ("Creating button components."); }

		// BUTTONS
		{ // screen button
			{
				auto &componentButton = canvas.buttons[0];
				auto &local = componentButton.local;

				local.name = "testButton";
				local.elementType = UI::ElementType::BUTTON;

				componentButton.id = OBJECT::_09_SQUARE_1;
			}
		}

		DEBUG_ENGINE { spdlog::info ("Creating collider components."); }

		// HARDCODDED Collision Game Object

		u16 CGO1 = 3; // OBJECT::_07_player;
		u16 CGO2 = 5; // OBJECT::_08_testWall;
        u16 CGO3 = 7; // OBJECT::_07_player;
        u16 CG04 = 8; // goal
        u16 CG05 = 9; // power up

		// COLLIDERS
		{ // Canvas
			{
				//SCENE::Canvas can = canvas;
				auto& componentCollider = canvas.colliders[COLLIDER::ColliderGroup::UI][0];
				auto& local = componentCollider.local;

				local.group = COLLIDER::ColliderGroup::UI;
				local.type = COLLIDER::ColliderType::PLANE;

				u64 rectangleIndex = OBJECT::ID_DEFAULT;
				OBJECT::GetComponentFast<RECTANGLE::LRectangle> (
					rectangleIndex, canvas.rectanglesCount, 
					canvas.lRectangles, OBJECT::_09_SQUARE_1
				);

				auto& rectangle = canvas.lRectangles[rectangleIndex];
				const glm::vec2 extra = glm::vec2 (1.0f, 1.0f); // Weirdly it seems to be off by 1 pixel.

				// Kiedy odwróci się żeby czytało od lewego-dolnego a nie od lewego-górnego
				//  To można usunąć `windowTransform[3]` i trzeba zamienić wartości `yMin` z `yMax`!
                COLLIDER::UpdateUICollider(componentCollider, rectangle, GLOBAL::windowTransform[2], GLOBAL::windowTransform[3]);

				componentCollider.id = OBJECT::_09_SQUARE_1;
			}
		}

        // Initialize segment colliders
		// HACK This has to happen inside location.hpp load phase !
        u16 giCollider = 1; // HACK, wall is 1st.
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
        u16 giTriggerCollider = 2; // HACK, wall is 1st. power up is 2nd
        for (u16 iSegment = 0; iSegment < segmentsCount; ++iSegment) {

            auto& worldColliders = world.colliders[COLLIDER::ColliderGroup::TRIGGER];

            auto& segment = segmentsWorld[iSegment];
            auto& collidersCount = segment.collidersCount[COLLIDER::ColliderGroup::TRIGGER];

            springTrapsCount += collidersCount;

            for (u16 iCollider = 0; iCollider < collidersCount; ++iCollider) {

                auto& componentCollider = worldColliders[giTriggerCollider];
                auto& base = componentCollider.local;

                base.group = COLLIDER::ColliderGroup::TRIGGER;
                base.type = COLLIDER::ColliderType::AABB;

                const u16 hackOffset = 15; // HACK
                componentCollider.id = hackOffset + (iSegment * segmentsCount) + iCollider + 1; // It simply should refer to segments collisions

                if (iCollider == 0)
                {
                    base.collisionEventName = "SpringTrap";
                }
                else
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

        TrapGeneration(mapGenerator, springTrapsCount);
        ApplyTraps(mapGenerator, world.colliders[COLLIDER::ColliderGroup::TRIGGER], world.collidersCount[COLLIDER::ColliderGroup::TRIGGER]);

        //for (int i = 0; i < segmentsCount; i++)
        //{
        //    for (int j = 0; j < segmentsWorld[i].transformsCount; j++)
        //    {
        //        auto& componentCollider = world.colliders[COLLIDER::ColliderGroup::MAP][colliderIndex];
        //        auto& local = componentCollider.local;
        //        local.group = COLLIDER::ColliderGroup::MAP;
        //        local.type = COLLIDER::ColliderType::OBB;
        //        componentCollider.id = 15 + i*segmentsCount+j;
        //        COLLIDER::InitializeColliderSize(world.colliders[COLLIDER::ColliderGroup::MAP][colliderIndex], sharedWorld.meshes[0], segmentsWorld[i].gTransforms[j]);
        //        colliderIndex++;
        //	  }
        //}


		// COLLIDERS
		{ // world colliders
			{ // player1
				auto& componentCollider = world.colliders[COLLIDER::ColliderGroup::PLAYER][0];
				auto& local = componentCollider.local;
				local.group = COLLIDER::ColliderGroup::PLAYER;
				local.type = COLLIDER::ColliderType::AABB;
				componentCollider.id = CGO1;
			}
            { // player2
                auto& componentCollider = world.colliders[COLLIDER::ColliderGroup::PLAYER][1];
                auto& local = componentCollider.local;
                local.group = COLLIDER::ColliderGroup::PLAYER;
                local.type = COLLIDER::ColliderType::AABB;
                componentCollider.id = CGO3;
            }
			{ // platform/wall
				auto& componentCollider = world.colliders[COLLIDER::ColliderGroup::MAP][0];
				auto& local = componentCollider.local;
				local.group = COLLIDER::ColliderGroup::MAP;
				local.type = COLLIDER::ColliderType::OBB2;
				componentCollider.id = CGO2;
			}
            { // test trigger
                auto& componentCollider = world.colliders[COLLIDER::ColliderGroup::TRIGGER][0];
                auto& local = componentCollider.local;
                local.group = COLLIDER::ColliderGroup::TRIGGER;
                local.type = COLLIDER::ColliderType::AABB;
                componentCollider.id = CG04;
                local.collisionEventName = "Goal";
            }
            { // power up trigger
                auto& componentCollider = world.colliders[COLLIDER::ColliderGroup::TRIGGER][1];
                auto& local = componentCollider.local;
                local.group = COLLIDER::ColliderGroup::TRIGGER;
                local.type = COLLIDER::ColliderType::AABB;
                componentCollider.id = CG05;
                local.collisionEventName = "PowerUp";
            }
            { // camera1
                auto& componentCollider = world.colliders[COLLIDER::ColliderGroup::CAMERA][0];
                auto& local = componentCollider.local;
                local.group = COLLIDER::ColliderGroup::CAMERA;
                local.type = COLLIDER::ColliderType::OBB;
                componentCollider.id = 14;
            }
            { // camera2
                auto& componentCollider = world.colliders[COLLIDER::ColliderGroup::CAMERA][1];
                auto& local = componentCollider.local;
                local.group = COLLIDER::ColliderGroup::CAMERA;
                local.type = COLLIDER::ColliderType::OBB;
                componentCollider.id = 15;
            }
		}

		{ // colliders initialization
			{ // HACK! mesh no longer holds gameObject ID! Matthew here I think it's best to move it inside json->location.hpp->Load method.
				u64 meshIndex = OBJECT::ID_DEFAULT;
				//OBJECT::GetComponentSlow<MESH::Mesh>(meshIndex, sharedWorld.meshesCount, sharedWorld.meshes, CGO1);
				u64 colliderIndex = OBJECT::ID_DEFAULT;
				OBJECT::GetComponentSlow<COLLIDER::Collider>(colliderIndex, world.collidersCount[COLLIDER::ColliderGroup::PLAYER], world.colliders[COLLIDER::ColliderGroup::PLAYER], CGO1);
                u64 transformIndex = 0;
                OBJECT::GetComponentFast<TRANSFORM::LTransform>(transformIndex, world.transformsCount,world.lTransforms, CGO1);
				COLLIDER::InitializeColliderSize(world.colliders[COLLIDER::ColliderGroup::PLAYER][colliderIndex], sharedWorld.meshes[meshIndex], world.gTransforms[transformIndex]);
			}
            {
                u64 meshIndex = OBJECT::ID_DEFAULT;
                //OBJECT::GetComponentSlow<MESH::Mesh>(meshIndex, sharedWorld.meshesCount, sharedWorld.meshes, CGO3);
                u64 colliderIndex = OBJECT::ID_DEFAULT;
                OBJECT::GetComponentSlow<COLLIDER::Collider>(colliderIndex, world.collidersCount[COLLIDER::ColliderGroup::PLAYER], world.colliders[COLLIDER::ColliderGroup::PLAYER], CGO3);
                u64 transformIndex = 0;
                OBJECT::GetComponentFast<TRANSFORM::LTransform>(transformIndex, world.transformsCount,world.lTransforms, CGO3);
                COLLIDER::InitializeColliderSize(world.colliders[COLLIDER::ColliderGroup::PLAYER][colliderIndex], sharedWorld.meshes[meshIndex], world.gTransforms[transformIndex]);
            }
			{
				u64 meshIndex = OBJECT::ID_DEFAULT;
				//OBJECT::GetComponentSlow<MESH::Mesh>(meshIndex, sharedWorld.meshesCount, sharedWorld.meshes, CGO2);
				u64 colliderIndex = OBJECT::ID_DEFAULT;
				OBJECT::GetComponentSlow<COLLIDER::Collider>(colliderIndex, world.collidersCount[COLLIDER::ColliderGroup::MAP], world.colliders[COLLIDER::ColliderGroup::MAP], CGO2);
                u64 transformIndex = 0;
                OBJECT::GetComponentFast<TRANSFORM::LTransform>(transformIndex, world.transformsCount,world.lTransforms, CGO2);
                COLLIDER::InitializeColliderSize(world.colliders[COLLIDER::ColliderGroup::MAP][colliderIndex], sharedWorld.meshes[meshIndex], world.gTransforms[transformIndex]);
			}
            {
                u64 meshIndex = OBJECT::ID_DEFAULT;
                //OBJECT::GetComponentSlow<MESH::Mesh>(meshIndex, sharedWorld.meshesCount, sharedWorld.meshes, CG04);
                u64 colliderIndex = OBJECT::ID_DEFAULT;
                OBJECT::GetComponentSlow<COLLIDER::Collider>(colliderIndex, world.collidersCount[COLLIDER::ColliderGroup::TRIGGER], world.colliders[COLLIDER::ColliderGroup::TRIGGER], CG04);
                u64 transformIndex = 0;
                OBJECT::GetComponentFast<TRANSFORM::LTransform>(transformIndex, world.transformsCount,world.lTransforms, CG04);
                COLLIDER::InitializeColliderSize(world.colliders[COLLIDER::ColliderGroup::TRIGGER][colliderIndex], sharedWorld.meshes[meshIndex], world.gTransforms[transformIndex]);
            }
            {
                u64 meshIndex = OBJECT::ID_DEFAULT;
                //OBJECT::GetComponentSlow<MESH::Mesh>(meshIndex, sharedWorld.meshesCount, sharedWorld.meshes, CG05);
                u64 colliderIndex = OBJECT::ID_DEFAULT;
                OBJECT::GetComponentSlow<COLLIDER::Collider>(colliderIndex, world.collidersCount[COLLIDER::ColliderGroup::TRIGGER], world.colliders[COLLIDER::ColliderGroup::TRIGGER], CG05);
                u64 transformIndex = 0;
                OBJECT::GetComponentFast<TRANSFORM::LTransform>(transformIndex, world.transformsCount,world.lTransforms, CG05);
                COLLIDER::InitializeColliderSize(world.colliders[COLLIDER::ColliderGroup::TRIGGER][colliderIndex], sharedWorld.meshes[meshIndex], world.gTransforms[transformIndex]);
            }
            { // CAMERA COLLIDERS
                u64 colliderIndex = OBJECT::ID_DEFAULT;
                OBJECT::GetComponentSlow<COLLIDER::Collider>(colliderIndex, world.collidersCount[COLLIDER::ColliderGroup::CAMERA], world.colliders[COLLIDER::ColliderGroup::CAMERA], 14);
                //viewports[0].colliderIndex = colliderIndex;
                world.colliders[COLLIDER::ColliderGroup::CAMERA][colliderIndex].local.size = glm::vec3(0.0f);
                glm::mat4 transform = glm::mat4(1.0);
                transform = glm::translate(viewports[0].camera.local.position);
                COLLIDER::UpdateColliderTransform(world.colliders[COLLIDER::ColliderGroup::CAMERA][colliderIndex], transform);
            }
            {
                u64 colliderIndex = OBJECT::ID_DEFAULT;
                OBJECT::GetComponentSlow<COLLIDER::Collider>(colliderIndex, world.collidersCount[COLLIDER::ColliderGroup::CAMERA], world.colliders[COLLIDER::ColliderGroup::CAMERA], 15);
                //viewports[1].colliderIndex = colliderIndex;
                world.colliders[COLLIDER::ColliderGroup::CAMERA][colliderIndex].local.size = glm::vec3(0.0f);
                glm::mat4 transform = glm::mat4(1.0);
                transform = glm::translate(viewports[1].camera.local.position);
                COLLIDER::UpdateColliderTransform(world.colliders[COLLIDER::ColliderGroup::CAMERA][colliderIndex], transform);
            }
		}

		//{ // colliders initialization
		//	{
		//		u64 meshIndex = OBJECT::ID_DEFAULT;
		//		OBJECT::GetComponentSlow<MESH::Mesh>(meshIndex, world.meshesCount, world.meshes, CGO1);
		//		u64 colliderIndex = OBJECT::ID_DEFAULT;
		//		OBJECT::GetComponentSlow<COLLIDER::Collider>(colliderIndex, world.collidersCount[COLLIDER::ColliderGroup::PLAYER], world.colliders[COLLIDER::ColliderGroup::PLAYER], CGO1);
		//		COLLIDER::InitializeColliderSize(world.colliders[COLLIDER::ColliderGroup::PLAYER][colliderIndex], world.meshes[meshIndex], world.transformsCount, world.lTransforms);
		//	}

        DEBUG_ENGINE { spdlog::info ("Creating rigidbody components."); }

        {
            { // player 1 rigidbody
                auto &rigidbodyComponent = world.rigidbodies[0];
                rigidbodyComponent.id = CGO1;
            }
            { // player 2 rigidbody
                auto &rigidbodyComponent = world.rigidbodies[1];
                rigidbodyComponent.id = CGO3;
            }
        }

		DEBUG_ENGINE { spdlog::info ("Creating player components."); }

        {// players
            { // player1
                auto &player = world.players[0];
                auto &local = player.local;
                player.id = CGO1;
                //
                local.name = "TEST PLAYER1";
                std::vector<InputDevice> controlScheme;
                int deviceIndex = -1;
                INPUT_MANAGER::FindDevice(inputManager, InputSource::KEYBOARD, 0, deviceIndex);
                if (deviceIndex > -1)
                {
                    controlScheme.push_back(inputManager->_devices[deviceIndex]);
                    inputManager->_devices[deviceIndex].PlayerIndex = 0;
                }
                deviceIndex = -1;
                INPUT_MANAGER::FindDevice(inputManager, InputSource::MOUSE, 0, deviceIndex);
                if (deviceIndex > -1)
                {
                    controlScheme.push_back(inputManager->_devices[deviceIndex]);
                    inputManager->_devices[deviceIndex].PlayerIndex = 0;
                }
                local.controlScheme = controlScheme;
                u64 transformIndex = 0;
                OBJECT::GetComponentFast<TRANSFORM::LTransform>(transformIndex, world.transformsCount,
                                                                world.lTransforms, player.id);
                local.transformIndex = transformIndex;
                u64 colliderIndex = 0;
                OBJECT::GetComponentFast<COLLIDER::Collider>(colliderIndex,
                                                             world.collidersCount[COLLIDER::ColliderGroup::PLAYER],
                                                             world.colliders[COLLIDER::ColliderGroup::PLAYER],
                                                             player.id);
                local.colliderIndex = colliderIndex;
                u64 rigidbodyIndex = 0;
                OBJECT::GetComponentFast<RIGIDBODY::Rigidbody>(rigidbodyIndex,
                                                             world.rigidbodiesCount,
                                                             world.rigidbodies,
                                                             player.id);
                local.rigidbodyIndex = rigidbodyIndex;
                world.rigidbodies[rigidbodyIndex].base.transformIndex = transformIndex;
                world.rigidbodies[rigidbodyIndex].base.movementSpeed = local.movement.playerSpeed;
                PLAYER::MOVEMENT::CalculateGravitation(player, world.rigidbodies);
                local.playerIndex = 0;
            }
            { // player2
                auto &player = world.players[1];
                auto &local = player.local;
                player.id = CGO3;
                //
                local.name = "TEST PLAYER2";
                std::vector<InputDevice> controlScheme;
                int deviceIndex = -1;
                INPUT_MANAGER::FindDevice(inputManager, InputSource::GAMEPAD, 0, deviceIndex);
                if (deviceIndex > -1)
                {
                    controlScheme.push_back(inputManager->_devices[deviceIndex]);
                    inputManager->_devices[deviceIndex].PlayerIndex = 1;
                }
                local.controlScheme = controlScheme;
                u64 transformIndex = 0;
                OBJECT::GetComponentFast<TRANSFORM::LTransform>(transformIndex, world.transformsCount,
                                                                world.lTransforms, player.id);
                local.transformIndex = transformIndex;
                u64 colliderIndex = 0;
                OBJECT::GetComponentFast<COLLIDER::Collider>(colliderIndex,
                                                             world.collidersCount[COLLIDER::ColliderGroup::PLAYER],
                                                             world.colliders[COLLIDER::ColliderGroup::PLAYER],
                                                             player.id);
                local.colliderIndex = colliderIndex;
                u64 rigidbodyIndex = 0;
                OBJECT::GetComponentFast<RIGIDBODY::Rigidbody>(rigidbodyIndex,
                                                               world.rigidbodiesCount,
                                                               world.rigidbodies,
                                                               player.id);
                local.rigidbodyIndex = rigidbodyIndex;
                world.rigidbodies[rigidbodyIndex].base.transformIndex = transformIndex;
                world.rigidbodies[rigidbodyIndex].base.movementSpeed = local.movement.playerSpeed;
                PLAYER::MOVEMENT::CalculateGravitation(player, world.rigidbodies);
                local.playerIndex = 1;
            }

        }

		DEBUG_ENGINE { spdlog::info ("Creating Rotating components."); }

		//{
		//	assert (world.rotatingsCount == 2);
		//	world.rotatings[0] = ROTATING::Rotating { 1, ROTATING::Base { 0.0f, 0.0f, 1.0f } };
		//	world.rotatings[1] = ROTATING::Rotating { 4, ROTATING::Base { 0.0f, 1.0f, 0.0f } };
		//}

		//DEBUG {
		//	auto&& meshes = world.tables.meshes;
		//	spdlog::info (
		//		"materials: {0}",
		//		meshes[0]
		//	);
		//	spdlog::info (
		//		"meshes: {0}, id: {1}, instances: {2}, id: {3},", 
		//		meshes[1], meshes[2], meshes[3], meshes[4]
		//	);
		//	spdlog::info (
		//		"instances: {0}, meshes: {1}, id: {2}, instances: {3}, ", 
		//		meshes[5], meshes[6], meshes[7], meshes[8]
		//	);
		//	spdlog::info (
		//		"meshes: {0}, id: {1}, instances: {2}, meshes: {3}", 
		//		meshes[9], meshes[10], meshes[11], meshes[12]
		//	);	
		//	spdlog::info (
		//		"id: {0}, instances: {1}, huh: {2}",
		//		meshes[13], meshes[14], meshes[15]
		//	);	
		//}

        LoadCanvas (uiManager, canvas.buttons, canvas.buttonsCount);

		DEBUG_ENGINE spdlog::info ("Creating GLTF scenes and objects.");

		// TODO
		// 1. transformsOffset -> Calculate transfroms without meshes.
		// 2. childrenTable -> Calculate how many childen there are and allocate an array. For Parenthood components to use.

		auto& handlersCount = RESOURCES::MANAGER::GLTFS::HANDLERS_COUNT;

		//  We'll allocate it with one call but point different pointers later.
		u16* parenthoodsChildrenTable[handlersCount];
		u16 childrenCount[handlersCount];

		RESOURCES::Json gltfsHandlers[handlersCount]	{ 0 };			// Create an a array of nlohmann/json data handlers.
		SCENE::SceneLoadContext gltfLoad[handlersCount] { 0 };			// Create a load structure. aka. relationsLookUpTable.

		// Pre alloc moved to surface.
		for (u8 i = 0; i < handlersCount; ++i) {
			auto& mmrlut = gltfLoad[i].relationsLookUpTable;
			mmrlut = (u16*) malloc (RESOURCES::MMRELATION::MAX_NODES * sizeof (u16));
		}

		u8* duplicateObjects[handlersCount] 					{ nullptr };	// HELPER
		u8 nodeMeshTable[handlersCount][MESH::MAX_MESHES / 2]	{}; 			// HELPER // !!! use alloc so i can free it myself at time i want.

		for (u16 i = 0; i < RESOURCES::MANAGER::GLTFS::HANDLERS_COUNT; ++i) {	// Go thouth all gltf difined files.
			auto& filepath 			= RESOURCES::MANAGER::GLTFS::FILEPATHS[i];						
			auto& json 				= gltfsHandlers[i];

			// WORLD
			auto& parenthoodsCount 	= gltfWorld[i].parenthoodsCount;
			auto& parenthoods 		= gltfWorld[i].parenthoods;
			auto& transformsCount 	= gltfWorld[i].transformsCount;	
			auto& lTransforms 		= gltfWorld[i].lTransforms;
			auto& gTransforms 		= gltfWorld[i].gTransforms;
			auto& meshTable 		= gltfWorld[i].tables.meshes;
			auto& transformsOffset 	= gltfWorld[i].transformsOffset;

			// SHARED 
			auto& materialsCount 	= gltfSharedWorld[i].materialsCount;
			auto& materials 		= gltfSharedWorld[i].materials;
			auto& meshesCount 		= gltfSharedWorld[i].meshesCount;
			auto& meshes 			= gltfSharedWorld[i].meshes;

			DEBUG_ENGINE spdlog::info ("Creating gltf: {0}.", filepath);
			RESOURCES::Parse (json, filepath);												// Parse file into json format.
			RESOURCES::GLTF::Create (														// Parse json in engine format. (Allocation and helper structs inforamtion only)
				json, gltfLoad[i],
				//
				parenthoodsCount,
				childrenCount[i],
				transformsCount,
				//
				materialsCount,
				meshesCount,
				//
				meshTable,
				transformsOffset,
				//
				duplicateObjects[i],
				nodeMeshTable[i]
			);	

			RESOURCES::GLTF::Allocate (
				childrenCount[i], parenthoodsChildrenTable[i], 
				parenthoodsCount,  parenthoods,
				transformsCount, lTransforms, gTransforms,
				materialsCount, materials,
				meshesCount, meshes
			);
			
		}

		DEBUG spdlog::info ("Loading GLTF scenes and objects.");

		for (u16 i = 0; i < RESOURCES::MANAGER::GLTFS::HANDLERS_COUNT; ++i) {				// Go thouth all gltf difined files.
			auto& json 				= gltfsHandlers[i];

			// WORLD
			auto& parenthoodsCount 	= gltfWorld[i].parenthoodsCount;
			auto& parenthoods 		= gltfWorld[i].parenthoods;
			auto& transformsCount 	= gltfWorld[i].transformsCount;	
			auto& lTransforms 		= gltfWorld[i].lTransforms;
			auto& gTransforms 		= gltfWorld[i].gTransforms;
			auto& meshTable 		= gltfWorld[i].tables.meshes;

			// SHARED 
			auto& materialsCount 	= gltfSharedWorld[i].materialsCount;
			auto& materials 		= gltfSharedWorld[i].materials;
			auto& meshesCount 		= gltfSharedWorld[i].meshesCount;
			auto& meshes 			= gltfSharedWorld[i].meshes;

			DEBUG spdlog::info ("Loading gltf: {0}.", i);
			RESOURCES::GLTF::Load (															// Parse json in engine format. 
				json, gltfLoad[i],
				//
				parenthoodsCount,
				parenthoodsChildrenTable[i],
				parenthoods,
				//
				transformsCount,
				lTransforms,
				gTransforms,
				//
				materialsCount,
				materials,
				//
				meshesCount,
				meshes,
				//
				meshTable,
				//
				duplicateObjects[i],
				nodeMeshTable[i]
			);													
		}

		// spdlog::info ("HERE!");


		//DEBUG { // TODO: Make it into functions...
		//	auto& world = gltfWorld[0];
		//
		//	spdlog::info ("PARENTHOODS: {0}", world.parenthoodsCount);
		//	for (u16 iParenthood = 0; iParenthood < world.parenthoodsCount; ++iParenthood) {
		//		auto& parenthood = world.parenthoods[iParenthood];
		//
		//		spdlog::info ("p-{0}: id: {1}, cc: {2}", iParenthood, parenthood.id, parenthood.base.childrenCount);
		//		for (u16 iChild = 0; iChild < parenthood.base.childrenCount; ++iChild)
		//			spdlog::info ("id: {0}", parenthood.base.children[iChild]);
		//	}
		//
		//	spdlog::info ("TRANSFORMS: {0}", world.transformsCount);
		//	for (u16 iTransform = 0; iTransform < world.transformsCount; ++iTransform) {
		//		auto& transform = world.lTransforms[iTransform];
		//		auto& base = transform.base;
		//
		//		spdlog::info ("t-{0}: id: {1}", iTransform, transform.id);
		//		spdlog::info ("p: {0}, {1}, {2}", base.position.x, base.position.y, base.position.z);
		//		spdlog::info ("r: {0}, {1}, {2}", base.rotation.x, base.rotation.y, base.rotation.z);
		//		spdlog::info ("s: {0}, {1}, {2}", base.scale.x, base.scale.y, base.scale.z);
		//	}
		//
		//	auto& sWorld = gltfSharedWorld[0];
		//
		//	spdlog::info ("MESHES: {0}", sWorld.meshesCount);
		//	for (u16 iMesh = 0; iMesh < sWorld.meshesCount; ++iMesh) {
		//		auto& mesh = sWorld.meshes[iMesh];
		//
		//		//spdlog::info ("t-{0}: id: {1}", iMesh, transform.id);
		//		//spdlog::info ("p: {0}, {1}, {2}", base.position.x, base.position.y, base.position.z);
		//		//spdlog::info ("r: {0}, {1}, {2}", base.rotation.x, base.rotation.y, base.rotation.z);
		//		//spdlog::info ("s: {0}, {1}, {2}", base.scale.x, base.scale.y, base.scale.z);
		//	}
		//
		//}

		DEBUG spdlog::info ("Combining and Sorting the scenes.");

		//

		DEBUG spdlog::info ("Initialization Complete!");

		// Connect Scene to Screen & World structures.
		scene.skybox = &skybox;
		scene.screen = &screen;
		scene.canvas = &canvas;
		scene.world = &world;
        glfwSetInputMode(GLOBAL::mainWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	}








	void DestroyWorld (SCENE::World& world) {
		DEBUG_ENGINE { spdlog::info ("Destroying parenthood components."); }
		delete[] world.parenthoods;
		delete[] world.tables.parenthoodChildren;
		DEBUG_ENGINE { spdlog::info ("Destroying transfrom components."); }
		delete[] world.lTransforms;
		delete[] world.gTransforms;
		DEBUG_ENGINE { spdlog::info ("Destroying rotating components."); }
		delete[] world.rotatings;
		DEBUG_ENGINE { spdlog::info ("Destroying collider components."); }
		delete[] world.colliders[COLLIDER::ColliderGroup::MAP];
		delete[] world.colliders[COLLIDER::ColliderGroup::PLAYER];
        delete[] world.colliders[COLLIDER::ColliderGroup::TRIGGER];
        delete[] world.colliders[COLLIDER::ColliderGroup::CAMERA];
		DEBUG_ENGINE { spdlog::info ("Destroying render objects."); }
		delete[] world.tables.meshes;
	}


	void Destroy () {
		PROFILER { ZoneScopedN("GLOBAL: Destroy"); }

		// !!!! segmentsWorld = new SCENE::World[segmentsCount] { 0 };

		DEBUG_ENGINE { spdlog::info ("Destroying parenthood components."); }
		
		delete[] screen.parenthoods;
		//delete[] screen.tables.parenthoodChildren;

		delete[] canvas.parenthoods;
		//delete[] canvas.tables.parenthoodChildren;

		delete[] world.parenthoods;
		delete[] world.tables.parenthoodChildren;

        DEBUG_ENGINE { spdlog::info ("Destroying models."); }

        delete[] world.models;

		DEBUG_ENGINE { spdlog::info ("Destroying mesh components."); }

		RESOURCES::MESHES::DeleteMeshes (
			sharedScreen.meshesCount, sharedScreen.meshes,
			sharedCanvas.meshesCount, sharedCanvas.meshes,
			sharedWorld.meshesCount, sharedWorld.meshes
		);

		DEBUG_ENGINE { spdlog::info ("Destroying transfrom components."); }

		delete[] screen.lTransforms;
		delete[] screen.gTransforms;
		
		delete[] canvas.lRectangles;
		delete[] canvas.gRectangles;

		delete[] world.lTransforms;
		delete[] world.gTransforms;

		DEBUG_ENGINE { spdlog::info ("Destroying collider components."); }

		delete[] world.colliders[COLLIDER::ColliderGroup::MAP];
		delete[] world.colliders[COLLIDER::ColliderGroup::PLAYER];
        delete[] world.colliders[COLLIDER::ColliderGroup::TRIGGER];
        delete[] world.colliders[COLLIDER::ColliderGroup::CAMERA];
		delete[] canvas.colliders[COLLIDER::ColliderGroup::UI];

		DEBUG_ENGINE { spdlog::info ("Destroying button components."); }

		delete[] canvas.buttons;

        DEBUG_ENGINE { spdlog::info ("Destroying rigidbodies."); }

        delete[] world.rigidbodies;

		DEBUG_ENGINE { spdlog::info ("Destroying players."); }

		delete[] world.players;

		DEBUG_ENGINE { spdlog::info ("Destroying materials."); }

		RESOURCES::MATERIALS::DestoryMaterials (
			sharedScreen.tables.uniforms, screen.tables.meshes, sharedScreen.materials,
			sharedCanvas.tables.uniforms, canvas.tables.meshes, sharedCanvas.materials,
			sharedWorld.tables.uniforms, sharedWorld.materials
		);

		delete[] world.tables.meshes;

		RESOURCES::MATERIALS::DestroyLoadShaders (
			sharedScreen.loadTables.shaders, sharedCanvas.loadTables.shaders, sharedWorld.loadTables.shaders
		);

		DEBUG_ENGINE { spdlog::info ("Destroying shader programs."); }

		for (u64 i = 0; i < sharedScreen.materialsCount; ++i) {
			auto& material = sharedScreen.materials[i];
			SHADER::Destroy (material.program);
		}

		for (u64 i = 0; i < sharedCanvas.materialsCount; ++i) {
			auto& material = sharedCanvas.materials[i];
			SHADER::Destroy (material.program);
		}

		for (u64 i = 0; i < sharedWorld.materialsCount; ++i) {
			auto& material = sharedWorld.materials[i];
			SHADER::Destroy (material.program);
		}

		DEBUG_ENGINE { spdlog::info ("Destroying other words!"); }

		for (u8 iSegment = 0; iSegment < segmentsCount; ++iSegment) { // Precalculate Global Trnasfroms
			auto& cWorld = segmentsWorld[iSegment];
			DestroyWorld (cWorld);
		}

		// gltf worlds and gltf shared worlds. 
		for (u8 igltf = 0; igltf < RESOURCES::MANAGER::GLTFS::HANDLERS_COUNT; ++igltf) { 
			for (u64 iMaterial = 0; iMaterial < gltfSharedWorld[igltf].materialsCount; ++iMaterial) {
				auto& material = gltfSharedWorld[igltf].materials[iMaterial];
				SHADER::Destroy (material.program);
			}

			DestroyWorld (gltfWorld[igltf]);
		}

		DEBUG_ENGINE { spdlog::info ("Destroying input manager."); }

		delete inputManager;

		DEBUG_ENGINE { spdlog::info ("Destroying input."); }

		delete input;

		DEBUG_ENGINE { spdlog::info ("Destroying ui manager."); }

		delete uiManager;

        DEBUG_ENGINE { spdlog::info ("Destroying collision manager."); }

        delete collisionManager;

		DEBUG_ENGINE { spdlog::info ("Destroying map generator."); }

		delete mapGenerator;

		DEBUG_ENGINE { spdlog::info ("Successfully FREED all allocated memory!"); }

	}

}
