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
#include "resources/viewPortData.hpp"

#include "scene.hpp"
#include "object.hpp"
#include "render/texture.hpp"

#include "components/ui/uiManager.hpp"
#include "components/collisions/collisionManager.hpp"
#include "player/playerMovement.hpp"
#include "components/collisions/collisionsDetection.hpp"
#include "generator/mapGenerator.hpp"
#include "components/force.hpp"

#ifdef DEBUG_TOKEN
namespace GLOBAL::EDITOR {
	s32 editedObject = 6;			// ???
}
#endif

namespace GLOBAL {

	Color4 backgroundColor = Color4 ( 114.0f / 255.0f, 144.0f / 255.0f, 154.0f / 255.0f, 200.0f / 255.0f );

	// time of the previous frame
	// time of the 
	double timeSinceLastFrame = 0, timeCurrent = 0, timeDelta = 0;

	WIN::WindowTransform windowTransform { 0, 0, 1200, 640 }; // pos.x, pos.y, size.x, size.y
	s32 viewportsCount = 2;

	//Prepare starting mouse positions
	float lastX = windowTransform[2] / 2.0f;
	float lastY = windowTransform[3] / 2.0f;
	
	INPUT_MANAGER::IM inputManager = nullptr;
	HID_INPUT::Input input = nullptr;
	WIN::Window mainWindow = nullptr;

	UI::MANAGER::UIM uiManager = nullptr;
    COLLISION::MANAGER::CM collisionManager = nullptr;
	MAP_GENERATOR::MG mapGenerator = nullptr;

    std::vector<FORCE::Force> forces;

    glm::vec3 *camPos = nullptr;
	PLAYER::Player *players = nullptr;
	u64 playerCount = 0;
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

	// INITIALIZATION STAGES
	// 1. SET ( set how many specific components there will be )
	// 2. CREATE ( allocate memory for each component )
	// 3. LOAD ( load default data to each created component )


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
			world.collidersCount[COLLIDER::ColliderGroup::PLAYER]	= 2;
			world.collidersCount[COLLIDER::ColliderGroup::MAP]	= 1;
            world.collidersCount[COLLIDER::ColliderGroup::TRIGGER]	= 1;
			//world.collidersCount[COLLIDER::ColliderGroup::PLAYER]	= 0;
			//world.collidersCount[COLLIDER::ColliderGroup::MAP]		= 0;
			//world.rotatingsCount									= 2;
		}

		{ // PLAYERS
			playerCount = 2;
		}

		DEBUG { spdlog::info ("Creating map generator."); }

		{
			MAP_GENERATOR::ParkourDifficulty difficulty {
				/*rangePosition*/ 0.5f,
				/*rangeWidth*/    1.0f
			};

			MAP_GENERATOR::Modifiers modifiers {
				/*levelLength*/ 				5,
				/*stationaryTrapsAmount*/ 		2,
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
			
			segmentsCount = mapGenerator->_generatedLevel.size();

			// Memory allocations...
			segmentsJson	= new RESOURCES::Json[segmentsCount];
			segmentLoad		= new SCENE::SceneLoadContext[segmentsCount] { 0 };
			segmentsWorld	= new SCENE::World[segmentsCount] { 0 };
		}

		DEBUG { spdlog::info ("Allocating memory for components and collections."); }

        world.modelsCount = 1;
        world.models = new MODEL::Model[world.modelsCount]{ nullptr };

        RESOURCES::MANAGER::LoadModels(world.modelsCount, world.models);

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

		RESOURCES::MESHES::CreateMeshes (
			meshesJson,
			sharedScreen.meshesCount, sharedScreen.meshes,
			sharedCanvas.meshesCount, sharedCanvas.meshes,
			sharedWorld.meshesCount, sharedWorld.meshes
		);

		{ // Loading main.
			//
			const u8 DIFFICULTY = 4; // 0 - 4 (5)
			const u8 EXIT_TYPE = 2;  // 0 - 2 (3)
			// NOW ALWAYS CONSTANT "height": 48
			// NOW ALWAYS CONSTANT "platform_count": 0
			// NOW ALWAYS CONSTANT "trap_count": 0
			//
			RESOURCES::SCENE::Create (
				//sceneJson, RESOURCES::MANAGER::SCENES::SEGMENTS[DIFFICULTY + (5 * EXIT_TYPE)],
				//sceneJson, RESOURCES::MANAGER::SCENES::TOWER,
				sceneJson, RESOURCES::MANAGER::SCENES::ALPHA,
				sharedWorld.materialsCount, sharedWorld.meshesCount, 						// Already set
				world.tables.meshes, world.tables.parenthoodChildren, 						// Tables
				sceneLoad.relationsLookUpTable, world.transformsOffset,						// Helper Logic + what we get
				world.parenthoodsCount, world.transformsCount,								// What we actually get.
				world.rotatingsCount
			);
		}

		for (u8 iSegment = 0; iSegment < segmentsCount; ++iSegment) { // Loading additional.
			auto& segment = mapGenerator->_generatedLevel[iSegment];
			auto& fileJson = segmentsJson[iSegment];
			auto& loadHelper = segmentLoad[iSegment];
			auto& cWorld = segmentsWorld[iSegment];
			
			DEBUG if (segment.parkourDifficulty < 1.0f || segment.parkourDifficulty > 5.0f) {
				spdlog::error ("Segment difficulty ({0}) set to an invalid value!", segment.parkourDifficulty);
				exit (1);
			}

			const u8 DIFFICULTY = (u8)segment.parkourDifficulty - 1; 	// 3; // 0 - 4 (5)
			const u8 EXIT_TYPE = segment.exitSide; 						// 1;  // 0 - 2 (3)

			//DEBUG spdlog::info ("aaa: {0}, {1}", DIFFICULTY, EXIT_TYPE);
			
			RESOURCES::SCENE::Create (
				fileJson, RESOURCES::MANAGER::SCENES::SEGMENTS[DIFFICULTY + (5 * EXIT_TYPE)],
				sharedWorld.materialsCount, sharedWorld.meshesCount, 					// Already set
				cWorld.tables.meshes, cWorld.tables.parenthoodChildren, 				// Tables
				loadHelper.relationsLookUpTable, cWorld.transformsOffset,				// Helper Logic + what we get
				cWorld.parenthoodsCount, cWorld.transformsCount,						// What we actually get.
				world.rotatingsCount
			);
		}

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

			if (world.collidersCount[COLLIDER::ColliderGroup::PLAYER]) world.colliders[COLLIDER::ColliderGroup::PLAYER] = new COLLIDER::Collider[world.collidersCount[COLLIDER::ColliderGroup::PLAYER]] { 0 };
			if (world.collidersCount[COLLIDER::ColliderGroup::MAP]) world.colliders[COLLIDER::ColliderGroup::MAP] = new COLLIDER::Collider[world.collidersCount[COLLIDER::ColliderGroup::MAP]] { 0 };
            if (world.collidersCount[COLLIDER::ColliderGroup::TRIGGER]) world.colliders[COLLIDER::ColliderGroup::TRIGGER] = new COLLIDER::Collider[world.collidersCount[COLLIDER::ColliderGroup::TRIGGER]] { 0 };
            if (canvas.collidersCount[COLLIDER::ColliderGroup::UI]) canvas.colliders[COLLIDER::ColliderGroup::UI] = new COLLIDER::Collider[canvas.collidersCount[COLLIDER::ColliderGroup::UI]] { 0 };
		}

		{ // PLAYER
			if (playerCount) players = new PLAYER::Player[playerCount] { 0 };
		}

		DEBUG { spdlog::info ("Creating scene : parenthoods, transforms, meshTable."); }

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

		DEBUG { spdlog::info ("Precalculating transfroms global position."); }
		
		// To make every segment higher and rotated.
		auto& fSegment = mapGenerator->_generatedLevel[0];
		u8 side = fSegment.exitSide;
		//
		for (u8 iSegment = 1; iSegment < segmentsCount; ++iSegment) { 
			auto& segment = mapGenerator->_generatedLevel[iSegment];
			auto& cWorld = segmentsWorld[iSegment];
			cWorld.lTransforms[0].base.position.y += (24.0f * iSegment);
			cWorld.lTransforms[0].base.rotation.y += (90.0f * side);
			side = (side + segment.exitSide) % 4;
		}

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

		DEBUG { spdlog::info ("Creating textures."); }

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

		DEBUG { spdlog::info ("Creating materials."); }

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

		DEBUG { spdlog::info ("Creating shader programs."); }

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

		DEBUG { spdlog::info ("Creating meshes."); }

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

		DEBUG { spdlog::info ("Creating camera components."); }

		{ // World
			{

				world.viewPortDatas = std::vector<VIEWPORT::data>({});
				VIEWPORT::data newData{};

				// CAM 1 SET UP
				glm::vec3 position = glm::vec3(2.0f, 0.0f, -8.0f);
				// set z to its negative value, if we don't do it camera position on z is its negative value
				position.z = -position.z;
				newData.camera.local.position = position;
				glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
				newData.camera.local.worldUp = up;
				newData.camera.local.front = glm::vec3(0.0f, 0.0f, -1.0f);
                newData.camera.type = CAMERA::CameraType::THIRD_PERSON;
				newData.camera.local.yaw = CAMERA::YAW;
				newData.camera.local.pitch = CAMERA::PITCH;
				newData.camera.local.zoom = CAMERA::ZOOM;
				newData.camera.local.distance = CAMERA::DIST_FROM_TARGET;
				newData.camera.local.mouseSensitivity = CAMERA::SENSITIVITY;
				newData.camera.local.moveSpeed = CAMERA::SPEED;
				updateCameraVectors(newData.camera);
				// ------------
				world.viewPortDatas.push_back(newData);

				// CAM 2 SET UP
				position = glm::vec3(-2.0f, 0.0f, -8.0f);
				// set z to its negative value, if we don't do it camera position on z is its negative value
				position.z = - position.z;
				up = glm::vec3(0.0f, 1.0f, 0.0f);
				newData.camera.local.position = position;
				newData.camera.local.worldUp = up;
				newData.camera.local.front = glm::vec3(0.0f, 0.0f, -1.0f);
				newData.camera.local.yaw = CAMERA::YAW;
				newData.camera.local.pitch = CAMERA::PITCH;
				newData.camera.local.zoom = CAMERA::ZOOM;
				newData.camera.local.mouseSensitivity = CAMERA::SENSITIVITY;
				newData.camera.local.moveSpeed = CAMERA::SPEED;
				updateCameraVectors(newData.camera);
				// ------------
				world.viewPortDatas.push_back(newData);
			}
		}

		DEBUG { spdlog::info ("Creating button components."); }

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

		DEBUG { spdlog::info ("Creating collider components."); }

		// HARDCODDED Collision Game Object
		u16 CGO1 = 3; // OBJECT::_07_player;
		u16 CGO2 = 5; // OBJECT::_08_testWall;
        u16 CGO3 = 7; // OBJECT::_07_player;
		//
		//DEBUG {
		//	CGO1 = 3;
		//	CGO2 = 5;
		//}


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
				local.type = COLLIDER::ColliderType::AABB;
				componentCollider.id = CGO2;
			}
            { // test trigger
                auto& componentCollider = world.colliders[COLLIDER::ColliderGroup::TRIGGER][0];
                auto& local = componentCollider.local;
                local.group = COLLIDER::ColliderGroup::TRIGGER;
                local.type = COLLIDER::ColliderType::AABB;
                componentCollider.id = 4;
                local.collisionEventName = "testTrigger";
            }
		}

		{ // colliders initialization
			{
				u64 meshIndex = OBJECT::ID_DEFAULT;
				OBJECT::GetComponentSlow<MESH::Mesh>(meshIndex, sharedWorld.meshesCount, sharedWorld.meshes, CGO1);
				u64 colliderIndex = OBJECT::ID_DEFAULT;
				OBJECT::GetComponentSlow<COLLIDER::Collider>(colliderIndex, world.collidersCount[COLLIDER::ColliderGroup::PLAYER], world.colliders[COLLIDER::ColliderGroup::PLAYER], CGO1);
				COLLIDER::InitializeColliderSize(world.colliders[COLLIDER::ColliderGroup::PLAYER][colliderIndex], sharedWorld.meshes[meshIndex], world.transformsCount, world.gTransforms, world.lTransforms);
			}
            {
                u64 meshIndex = OBJECT::ID_DEFAULT;
                OBJECT::GetComponentSlow<MESH::Mesh>(meshIndex, sharedWorld.meshesCount, sharedWorld.meshes, CGO3);
                u64 colliderIndex = OBJECT::ID_DEFAULT;
                OBJECT::GetComponentSlow<COLLIDER::Collider>(colliderIndex, world.collidersCount[COLLIDER::ColliderGroup::PLAYER], world.colliders[COLLIDER::ColliderGroup::PLAYER], CGO3);
                COLLIDER::InitializeColliderSize(world.colliders[COLLIDER::ColliderGroup::PLAYER][colliderIndex], sharedWorld.meshes[meshIndex], world.transformsCount, world.gTransforms, world.lTransforms);
            }
			{
				u64 meshIndex = OBJECT::ID_DEFAULT;
				OBJECT::GetComponentSlow<MESH::Mesh>(meshIndex, sharedWorld.meshesCount, sharedWorld.meshes, CGO2);
				u64 colliderIndex = OBJECT::ID_DEFAULT;
				OBJECT::GetComponentSlow<COLLIDER::Collider>(colliderIndex, world.collidersCount[COLLIDER::ColliderGroup::MAP], world.colliders[COLLIDER::ColliderGroup::MAP], CGO2);
				COLLIDER::InitializeColliderSize(world.colliders[COLLIDER::ColliderGroup::MAP][colliderIndex], sharedWorld.meshes[meshIndex], world.transformsCount, world.gTransforms, world.lTransforms);
			}
            {
                u64 meshIndex = OBJECT::ID_DEFAULT;
                OBJECT::GetComponentSlow<MESH::Mesh>(meshIndex, sharedWorld.meshesCount, sharedWorld.meshes, 4);
                u64 colliderIndex = OBJECT::ID_DEFAULT;
                OBJECT::GetComponentSlow<COLLIDER::Collider>(colliderIndex, world.collidersCount[COLLIDER::ColliderGroup::TRIGGER], world.colliders[COLLIDER::ColliderGroup::TRIGGER], 4);
                COLLIDER::InitializeColliderSize(world.colliders[COLLIDER::ColliderGroup::TRIGGER][colliderIndex], sharedWorld.meshes[meshIndex], world.transformsCount, world.gTransforms, world.lTransforms);
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

		DEBUG { spdlog::info ("Creating player components."); }

        {// players
            { // player1
                auto &player = players[0];
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
                PLAYER::MOVEMENT::CalculateGravitation(players[0]);
            }
            { // player2
                auto &player = players[1];
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
                PLAYER::MOVEMENT::CalculateGravitation(players[1]);
            }

        }

		DEBUG { spdlog::info ("Creating Rotating components."); }

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

        LoadCanvas(uiManager, canvas.buttons, canvas.buttonsCount);

		DEBUG spdlog::info ("Initialization Complete!");

		// Connect Scene to Screen & World structures.
		scene.skybox = &skybox;
		scene.screen = &screen;
		scene.canvas = &canvas;
		scene.world = &world;
        glfwSetInputMode(GLOBAL::mainWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	}


	void DestroyWorld (SCENE::World& world) {
		DEBUG { spdlog::info ("Destroying parenthood components."); }
		delete[] world.parenthoods;
		delete[] world.tables.parenthoodChildren;
		DEBUG { spdlog::info ("Destroying transfrom components."); }
		delete[] world.lTransforms;
		delete[] world.gTransforms;
		DEBUG { spdlog::info ("Destroying rotating components."); }
		delete[] world.rotatings;
		DEBUG { spdlog::info ("Destroying collider components."); }
		delete[] world.colliders[COLLIDER::ColliderGroup::MAP];
		delete[] world.colliders[COLLIDER::ColliderGroup::PLAYER];
        delete[] world.colliders[COLLIDER::ColliderGroup::TRIGGER];
		DEBUG { spdlog::info ("Destroying render objects."); }
		delete[] world.tables.meshes;
	}


	void Destroy () {
		PROFILER { ZoneScopedN("GLOBAL: Destroy"); }

		// !!!! segmentsWorld = new SCENE::World[segmentsCount] { 0 };

		DEBUG { spdlog::info ("Destroying parenthood components."); }
		
		delete[] screen.parenthoods;
		//delete[] screen.tables.parenthoodChildren;

		delete[] canvas.parenthoods;
		//delete[] canvas.tables.parenthoodChildren;

		delete[] world.parenthoods;
		delete[] world.tables.parenthoodChildren;

        DEBUG { spdlog::info ("Destroying models."); }

        delete[] world.models;

		DEBUG { spdlog::info ("Destroying mesh components."); }

		RESOURCES::MESHES::DeleteMeshes (
			sharedScreen.meshesCount, sharedScreen.meshes,
			sharedCanvas.meshesCount, sharedCanvas.meshes,
			sharedWorld.meshesCount, sharedWorld.meshes
		);

		DEBUG { spdlog::info ("Destroying transfrom components."); }

		delete[] screen.lTransforms;
		delete[] screen.gTransforms;
		
		delete[] canvas.lRectangles;
		delete[] canvas.gRectangles;

		delete[] world.lTransforms;
		delete[] world.gTransforms;

		DEBUG { spdlog::info ("Destroying collider components."); }

		delete[] world.colliders[COLLIDER::ColliderGroup::MAP];
		delete[] world.colliders[COLLIDER::ColliderGroup::PLAYER];
        delete[] world.colliders[COLLIDER::ColliderGroup::TRIGGER];

		delete[] canvas.colliders[COLLIDER::ColliderGroup::UI];

		DEBUG { spdlog::info ("Destroying button components."); }

		delete[] canvas.buttons;

		DEBUG { spdlog::info ("Destroying players."); }

		delete[] players;

		DEBUG { spdlog::info ("Destroying materials."); }

		RESOURCES::MATERIALS::DestoryMaterials (
			sharedScreen.tables.uniforms, screen.tables.meshes, sharedScreen.materials,
			sharedCanvas.tables.uniforms, canvas.tables.meshes, sharedCanvas.materials,
			sharedWorld.tables.uniforms, sharedWorld.materials
		);

		delete[] world.tables.meshes;

		RESOURCES::MATERIALS::DestroyLoadShaders (
			sharedScreen.loadTables.shaders, sharedCanvas.loadTables.shaders, sharedWorld.loadTables.shaders
		);

		DEBUG { spdlog::info ("Destroying shader programs."); }

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

		DEBUG { spdlog::info ("Destroying other words!"); }

		for (u8 iSegment = 0; iSegment < segmentsCount; ++iSegment) { // Precalculate Global Trnasfroms
			auto& cWorld = segmentsWorld[iSegment];
			DestroyWorld (cWorld);
		}

		DEBUG { spdlog::info ("Destroying input manager."); }

		delete inputManager;

		DEBUG { spdlog::info ("Destroying input."); }

		delete input;

		DEBUG { spdlog::info ("Destroying ui manager."); }

		delete uiManager;

        DEBUG { spdlog::info ("Destroying collision manager."); }

        delete collisionManager;

		DEBUG { spdlog::info ("Destroying map generator."); }

		delete mapGenerator;

		DEBUG { spdlog::info ("Successfully FREED all allocated memory!"); }

	}

}
