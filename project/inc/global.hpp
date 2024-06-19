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

#include "components/collisions/collisionsDetection.hpp"
#include "components/collisions/collisionManager.hpp"
#include "components/checkpoints/checkPointManager.hpp"
#include "components/traps/springTrap.hpp"
#include "components/ui/uiManager.hpp"
#include "player/playerMovement.hpp"

#include "generator/mapGenerator.hpp"

#include "manager/objects.hpp"
#include "manager/audio.hpp"

#include "util/colorCorrection.hpp"


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
    r32 timeToCreateCheckpoint{2.0f};

    /********Window Traps Parameters******/
    r32 windowTrapWindUpTime{2.5f};
    r32 windowTrapActiveTime{1.0f};
    r32 windowTrapRechargeTime{3.5f};
    /*************************************/


	//Prepare starting mouse positions
	float lastX = windowTransform[2] / 2.0f;
	float lastY = windowTransform[3] / 2.0f;
	
	INPUT_MANAGER::IM inputManager = nullptr;
	HID_INPUT::Input input = nullptr;
	WIN::Window mainWindow = nullptr;

	UI::MANAGER::UIM uiManager = nullptr;
    COLLISION::MANAGER::CM collisionManager = nullptr;
	

    glm::mat4 camTransform{};
    glm::vec3 camCollisionOffset{};
	ANIMATION::Animation sharedAnimation1 { 1.0f, 6, 0, 0.0f, 0 };

	// --------------------

	glm::vec3 lightPosition = glm::vec3(1.0f, 1.0f, 1.0f);

	POWER_UP::PowerUp activePowerUp;


	// ---------- SCENES ------------


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
	MAP_GENERATOR::MG mapGenerator = nullptr;

    unsigned int FBO, rectVAO, rectVBO, framebufferTexture;

	void BaseCreate () {
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

    void InitializeFrameBuffer (unsigned int& rectVAO, unsigned int& rectVBO, unsigned int& FBO, unsigned int& framebufferTexture, int width, int height)
    {
        float rectangleVertices[] =
                {
                        // Coords    // texCoords
                        1.0f, -1.0f,  1.0f, 0.0f,
                        -1.0f, -1.0f,  0.0f, 0.0f,
                        -1.0f,  1.0f,  0.0f, 1.0f,

                        1.0f,  1.0f,  1.0f, 1.0f,
                        1.0f, -1.0f,  1.0f, 0.0f,
                        -1.0f,  1.0f,  0.0f, 1.0f
                };

        glGenVertexArrays(1, &rectVAO);
        glGenBuffers(1, &rectVBO);
        glBindVertexArray(rectVAO);
        glBindBuffer(GL_ARRAY_BUFFER, rectVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(rectangleVertices), &rectangleVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

        glGenFramebuffers(1, &FBO);
        glBindFramebuffer(GL_FRAMEBUFFER, FBO);

        // Create Framebuffer Texture
        glGenTextures(1, &framebufferTexture);
        glBindTexture(GL_TEXTURE_2D, framebufferTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // Prevents edge bleeding
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); // Prevents edge bleeding
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, framebufferTexture, 0);

        // Create Render Buffer Object
        unsigned int RBO;
        glGenRenderbuffers(1, &RBO);
        glBindRenderbuffer(GL_RENDERBUFFER, RBO);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, RBO);


        // Error checking framebuffer
        auto fboStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
        if (fboStatus != GL_FRAMEBUFFER_COMPLETE)
            std::cout << "Framebuffer error: " << fboStatus << std::endl;
    }

	void Initialize () {
		PROFILER { ZoneScopedN("GLOBAL: Initialize"); }

		// It's all Data Layer, Memory allocations, Pointer assignments.
		RESOURCES::Json materialsJson;
		RESOURCES::Json meshesJson;
		RESOURCES::Json sceneJson;
		SCENE::SceneLoadContext sceneLoad { 0 };

		RESOURCES::Json* segmentsJson = nullptr;
		SCENE::SceneLoadContext* segmentLoad = nullptr;

        InitializeFrameBuffer(rectVAO, rectVBO, FBO, framebufferTexture, windowTransform[2], windowTransform[3]);

        { // GLTF'S
			DEBUG_ENGINE spdlog::info ("Creating GLTF scenes and objects.");

			MANAGER::OBJECTS::GLTF::Create ();

			DEBUG_ENGINE spdlog::info ("Loading GLTF scenes and objects.");

			MANAGER::OBJECTS::GLTF::Create ();
		}
		
		// This should be read from the json scene file.
		BaseCreate ();

		DEBUG_ENGINE { spdlog::info ("Creating map generator."); }

		{
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

		DEBUG_ENGINE { spdlog::info ("Allocating memory for components and collections."); }

        //world.modelsCount = 1;
        //world.models = new MODEL::Model[world.modelsCount]{ nullptr };
        //RESOURCES::MANAGER::LoadModels (world.modelsCount, world.models);
		//DEBUG { spdlog::info ("MODELRS LOADED!"); }

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

        //AI StateMachine
        {
            world.windowTrapCount = 1;
            world.windowTraps = new AGENT::WindowData[world.windowTrapCount];

            // TEMP window trap create - delete after import from json works
            world.windowTraps[0].isRechargable = true;
            world.windowTraps[0].isActive = true;
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

        DEBUG_ENGINE { spdlog::info ("Precalculating transfroms global position."); }

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
        u16 CG06 = 10; // WindowTrap Trigger
        u16 CG07 = 11; // WindowTrap Collider

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

        TrapGeneration(mapGenerator, springTrapsCount);
        ApplyTraps(mapGenerator, world.colliders[COLLIDER::ColliderGroup::TRIGGER], world.collidersCount[COLLIDER::ColliderGroup::TRIGGER], segmentsWorld);

        CheckpointsGeneration(mapGenerator, checkpointsCount);
        ApplyCheckpoints(mapGenerator, world.colliders[COLLIDER::ColliderGroup::TRIGGER], world.collidersCount[COLLIDER::ColliderGroup::TRIGGER], segmentsWorld);

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
            { // windowTrap hardcoded collider
                auto& componentCollider = world.colliders[COLLIDER::ColliderGroup::MAP][1];
                auto& local = componentCollider.local;
                local.group = COLLIDER::ColliderGroup::MAP;
                local.type = COLLIDER::ColliderType::OBB2;
                componentCollider.id = CG07;
                u64 transformIndex = OBJECT::ID_DEFAULT;
                OBJECT::GetComponentFast<TRANSFORM::LTransform>(transformIndex, GLOBAL::world.transformsCount, GLOBAL::world.lTransforms, componentCollider.id);

                world.windowTraps[0].colliderId = transformIndex;
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
            { // windowTrap hardcoded trigger
                auto& componentCollider = world.colliders[COLLIDER::ColliderGroup::TRIGGER][2];
                auto& local = componentCollider.local;
                local.group = COLLIDER::ColliderGroup::TRIGGER;
                local.type = COLLIDER::ColliderType::AABB;
                componentCollider.id = CG06;
                local.collisionEventName = "WindowTrap";
                world.windowTraps[0].id = componentCollider.id;
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
            {//HARDCODED WINDOW TRAP COLLIDER
                u64 meshIndex = OBJECT::ID_DEFAULT;
                //OBJECT::GetComponentSlow<MESH::Mesh>(meshIndex, sharedWorld.meshesCount, sharedWorld.meshes, CGO2);
                u64 colliderIndex = OBJECT::ID_DEFAULT;
                OBJECT::GetComponentSlow<COLLIDER::Collider>(colliderIndex, world.collidersCount[COLLIDER::ColliderGroup::MAP], world.colliders[COLLIDER::ColliderGroup::MAP], CG07);
                u64 transformIndex = 0;
                OBJECT::GetComponentFast<TRANSFORM::LTransform>(transformIndex, world.transformsCount,world.lTransforms, CG07);
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
            {//HARDCODED WINDOWTRAP TRIGGER
                u64 meshIndex = OBJECT::ID_DEFAULT;
                //OBJECT::GetComponentSlow<MESH::Mesh>(meshIndex, sharedWorld.meshesCount, sharedWorld.meshes, CG05);
                u64 colliderIndex = OBJECT::ID_DEFAULT;
                OBJECT::GetComponentSlow<COLLIDER::Collider>(colliderIndex, world.collidersCount[COLLIDER::ColliderGroup::TRIGGER], world.colliders[COLLIDER::ColliderGroup::TRIGGER], CG06);
                u64 transformIndex = 0;
                OBJECT::GetComponentFast<TRANSFORM::LTransform>(transformIndex, world.transformsCount,world.lTransforms, CG06);
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

        LoadCanvas (uiManager, canvas.buttons, canvas.buttonsCount);

		DEBUG_ENGINE spdlog::info ("Combining and Sorting the scenes.");

		//

		DEBUG_ENGINE spdlog::info ("Initialization Complete!");

		// Connect Scene to Screen & World structures.
		scene.skybox = &skybox;
		scene.screen = &screen;
		scene.canvas = &canvas;
		scene.world = &world;
        glfwSetInputMode(GLOBAL::mainWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	}










	void Destroy () {
		PROFILER { ZoneScopedN ("GLOBAL: Destroy"); }

		DEBUG_ENGINE { spdlog::info ("Destroying mesh components."); }

		RESOURCES::MESHES::DeleteMeshes (
			sharedScreen.meshesCount, sharedScreen.meshes,
			sharedCanvas.meshesCount, sharedCanvas.meshes,
			sharedWorld.meshesCount, sharedWorld.meshes
		);

		DEBUG_ENGINE { spdlog::info ("Destroying materials."); }

		RESOURCES::MATERIALS::DestoryMaterials (
			sharedScreen.tables.uniforms, screen.tables.meshes, sharedScreen.materials,
			sharedCanvas.tables.uniforms, canvas.tables.meshes, sharedCanvas.materials,
			sharedWorld.tables.uniforms, sharedWorld.materials
		);

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

		{ // SCENES
			SCENE::SCREEN::Destroy (screen);
			SCENE::CANVAS::Destroy (canvas);
			SCENE::WORLD::Destroy (world);

			DEBUG_ENGINE { spdlog::info ("Destroying segments words."); }

			for (u8 iSegment = 0; iSegment < segmentsCount; ++iSegment) { // Precalculate Global Trnasfroms
				auto& cWorld = segmentsWorld[iSegment];
				SCENE::WORLD::Destroy (cWorld);
			}

		}

		{ // OTHER
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
		}

		DEBUG_ENGINE { spdlog::info ("Successfully FREED the memory!"); }

	}

}
