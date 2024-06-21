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

#include "manager/scenes.hpp"
#include "manager/audio.hpp"


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
	r32 timeToCreateCheckpoint {2.0f};

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

	void Initialize () {
		PROFILER { ZoneScopedN("GLOBAL: Initialize"); }

		// It's all Data Layer, Memory allocations, Pointer assignments.
		RESOURCES::Json materialsJson, meshesJson;

		collisionManager = new COLLISION::MANAGER::CollisionManager;
		uiManager = new UI::MANAGER::UIManager;

		MANAGER::SCENES::OBJECTS::CreateLoad ();

		DEBUG_ENGINE { spdlog::info ("Creating map generator."); }

		MANAGER::SCENES::GENERATOR::Create ();

		DEBUG_ENGINE { spdlog::info ("Creating Viewports."); }

		VIEWPORT::CreateLoad (viewports, viewportsCount);

		DEBUG_ENGINE { spdlog::info ("Allocating memory for components and collections."); }

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

		MANAGER::SCENES::MAIN::Create (canvas, screen, world, sharedWorld);

		MANAGER::SCENES::GENERATOR::CreateWorlds (world, sharedWorld);

		MANAGER::SCENES::MAIN::Allocate (canvas, screen, world);

		DEBUG_ENGINE { spdlog::info ("Creating scene : parenthoods, transforms, meshTable."); }

		MANAGER::SCENES::MAIN::Load (world, sharedWorld);
		MANAGER::SCENES::MAIN::Set (canvas, screen, world, sharedWorld);

		{
			checkpointManager.players = world.players;
			checkpointManager.checkpoints = world.checkpoints;
			checkpointIndexes = new s32[world.playersCount] {-1, -1};			// DELETE?
			checkpointTimers = new r32[world.playersCount]  {-1.0f, -1.0f};		// DELETE?
		}
		

		MANAGER::SCENES::GENERATOR::Load (world, sharedWorld);
		MANAGER::SCENES::GENERATOR::Set ();

		{ // ?
			u64 transformIndex = 0;
			OBJECT::GetComponentFast<TRANSFORM::LTransform> (transformIndex, world.transformsCount, world.lTransforms, 8);
			world.lTransforms[transformIndex].base.position.y = 
				MANAGER::SCENES::GENERATOR::mapGenerator->modifiers.levelLength * 24.f + 0.5f;
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

		DEBUG_ENGINE { spdlog::info ("Creating textures."); }

		MANAGER::SCENES::MAIN::CreateLoadTextures (skybox, sharedScreen, sharedCanvas, sharedWorld);

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

			MANAGER::SCENES::GENERATOR::GetMaxInstances (wInstancesCounts);
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
		u16 CG07 = 12; // WindowTrap Collider
        u16 CG08 = 13; // Moving platform

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

		MANAGER::SCENES::GENERATOR::InitializeColliders (world, sharedWorld);

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
				world.windowTraps[0].transformId = transformIndex;

                u64 colliderIndex = OBJECT::ID_DEFAULT;
                OBJECT::GetComponentSlow<COLLIDER::Collider>(colliderIndex, world.collidersCount[COLLIDER::ColliderGroup::MAP], world.colliders[COLLIDER::ColliderGroup::MAP], CG07);
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
            { // moving platform map
                auto& componentCollider = world.colliders[COLLIDER::ColliderGroup::MAP][2];
                auto& local = componentCollider.local;
                local.group = COLLIDER::ColliderGroup::MAP;
                local.type = COLLIDER::ColliderType::OBB;
                componentCollider.id = CG08;
            }
            { // moving platform trigger
                auto& componentCollider = world.colliders[COLLIDER::ColliderGroup::TRIGGER][3];
                auto& local = componentCollider.local;
                local.group = COLLIDER::ColliderGroup::TRIGGER;
                local.type = COLLIDER::ColliderType::OBB;
                componentCollider.id = CG08;
                local.collisionEventName = "MovingPlatform";
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
				COLLIDER::InitializeColliderSize(world.colliders[COLLIDER::ColliderGroup::PLAYER][colliderIndex], glm::vec3(2.f, 3.5f, 2.f), MANAGER::OBJECTS::GLTF::worlds[0].gTransforms[0]);
			}
			{
				u64 meshIndex = OBJECT::ID_DEFAULT;
				//OBJECT::GetComponentSlow<MESH::Mesh>(meshIndex, sharedWorld.meshesCount, sharedWorld.meshes, CGO3);
				u64 colliderIndex = OBJECT::ID_DEFAULT;
				OBJECT::GetComponentSlow<COLLIDER::Collider>(colliderIndex, world.collidersCount[COLLIDER::ColliderGroup::PLAYER], world.colliders[COLLIDER::ColliderGroup::PLAYER], CGO3);
				u64 transformIndex = 0;
				OBJECT::GetComponentFast<TRANSFORM::LTransform>(transformIndex, world.transformsCount,world.lTransforms, CGO3);
				COLLIDER::InitializeColliderSize(world.colliders[COLLIDER::ColliderGroup::PLAYER][colliderIndex], glm::vec3(2.f, 3.5f, 2.f), MANAGER::OBJECTS::GLTF::worlds[2].gTransforms[0]);
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
				//TEMP
				world.windowTraps[0].newPos = glm::vec3(world.gTransforms[transformIndex][3]);
			}
            {
                u64 meshIndex = OBJECT::ID_DEFAULT;
                //OBJECT::GetComponentSlow<MESH::Mesh>(meshIndex, sharedWorld.meshesCount, sharedWorld.meshes, CGO2);
                u64 colliderIndex = OBJECT::ID_DEFAULT;
                OBJECT::GetComponentSlow<COLLIDER::Collider>(colliderIndex, world.collidersCount[COLLIDER::ColliderGroup::MAP], world.colliders[COLLIDER::ColliderGroup::MAP], CG08);
                u64 transformIndex = 0;
                OBJECT::GetComponentFast<TRANSFORM::LTransform>(transformIndex, world.transformsCount,world.lTransforms, CG08);
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
            {
                u64 meshIndex = OBJECT::ID_DEFAULT;
                //OBJECT::GetComponentSlow<MESH::Mesh>(meshIndex, sharedWorld.meshesCount, sharedWorld.meshes, CG05);
                u64 colliderIndex = OBJECT::ID_DEFAULT;
                OBJECT::GetComponentSlow<COLLIDER::Collider>(colliderIndex, world.collidersCount[COLLIDER::ColliderGroup::TRIGGER], world.colliders[COLLIDER::ColliderGroup::TRIGGER], CG08);
                u64 transformIndex = 0;
                OBJECT::GetComponentFast<TRANSFORM::LTransform>(transformIndex, world.transformsCount,world.lTransforms, CG08);
                COLLIDER::InitializeColliderSize(world.colliders[COLLIDER::ColliderGroup::TRIGGER][colliderIndex], sharedWorld.meshes[meshIndex], world.gTransforms[transformIndex]);
            }
			{ // CAMERA COLLIDERS
				u64 colliderIndex = OBJECT::ID_DEFAULT;
				OBJECT::GetComponentSlow<COLLIDER::Collider>(colliderIndex, world.collidersCount[COLLIDER::ColliderGroup::CAMERA], world.colliders[COLLIDER::ColliderGroup::CAMERA], 14);
				//viewports[0].colliderIndex = colliderIndex;
				world.colliders[COLLIDER::ColliderGroup::CAMERA][colliderIndex].local.size = glm::vec3(0.3f);
				glm::mat4 transform = glm::mat4(1.0);
				transform = glm::translate(viewports[0].camera.local.position);
				COLLIDER::UpdateColliderTransform(world.colliders[COLLIDER::ColliderGroup::CAMERA][colliderIndex], transform);
			}
			{
				u64 colliderIndex = OBJECT::ID_DEFAULT;
				OBJECT::GetComponentSlow<COLLIDER::Collider>(colliderIndex, world.collidersCount[COLLIDER::ColliderGroup::CAMERA], world.colliders[COLLIDER::ColliderGroup::CAMERA], 15);
				//viewports[1].colliderIndex = colliderIndex;
				world.colliders[COLLIDER::ColliderGroup::CAMERA][colliderIndex].local.size = glm::vec3(0.3f);
				glm::mat4 transform = glm::mat4(1.0);
				transform = glm::translate(viewports[1].camera.local.position);
				COLLIDER::UpdateColliderTransform(world.colliders[COLLIDER::ColliderGroup::CAMERA][colliderIndex], transform);
			}
		}

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

        // Moving platforms
        {
            auto& movingPlatform = world.movingPlatforms[0];
            movingPlatform.id = CG08;
            auto& base = movingPlatform.base;
            base.travelDistance = glm::vec3(0.f, 10.f, 0.f);
            base.travelTime = 0.2f;
            u64 transformIndex = 0;
            OBJECT::GetComponentFast<TRANSFORM::LTransform>(transformIndex, world.transformsCount,
                                                            world.lTransforms, movingPlatform.id);
            base.transformIndex = transformIndex;
            u64 colliderIndex = 0;
            OBJECT::GetComponentFast<COLLIDER::Collider>(colliderIndex, GLOBAL::world.collidersCount[COLLIDER::ColliderGroup::MAP],
                                                         GLOBAL::world.colliders[COLLIDER::ColliderGroup::MAP], movingPlatform.id);
            base.mapColliderIndex = colliderIndex;
            OBJECT::GetComponentFast<COLLIDER::Collider>(colliderIndex, GLOBAL::world.collidersCount[COLLIDER::ColliderGroup::TRIGGER],
                                                         GLOBAL::world.colliders[COLLIDER::ColliderGroup::TRIGGER], movingPlatform.id);
            base.triggerColliderIndex = colliderIndex;
        }

		LoadCanvas (uiManager, canvas.buttons, canvas.buttonsCount);

		DEBUG_ENGINE spdlog::info ("Combining and Sorting the scenes.");

		//

		DEBUG_ENGINE spdlog::info ("Initialization Complete!");

		//MANAGER::OBJECTS::GLTF::Log (world, sharedWorld);

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

			MANAGER::SCENES::GENERATOR::Destroy ();
			MANAGER::OBJECTS::GLTF::Destroy ();
		}

		VIEWPORT::Destroy (viewports);

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

			delete MANAGER::SCENES::GENERATOR::mapGenerator;
		}

		DEBUG_ENGINE { spdlog::info ("Successfully FREED the memory!"); }

	}

}
