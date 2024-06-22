#pragma once
#include "global.hpp"
#include "update.hpp"
#include "render/render.hpp"


namespace FRAME {

	void Initialize () {
		PROFILER { ZoneScopedN ("Render: Initialize"); }

		glEnable (GL_BLEND);
		glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glEnable (GL_DEPTH_TEST);
		glDepthFunc (GL_LESS);
		
		//glPolygonMode ( GL_FRONT_AND_BACK, GL_LINE );
		glActiveTexture (GL_TEXTURE0);
	}
	
	void Frame () {
		PROFILER { ZoneScopedN("Render: Frame"); }
        //std::cout << "frames:" << 1.0f/GLOBAL::timeDelta << '\n';
		#if PLATFORM == PLATFORM_WINDOWS
			wglMakeCurrent (WIN::LOADER::graphicalContext, WIN::LOADER::openGLRenderContext);
		#else
			glfwMakeContextCurrent(GLOBAL::mainWindow);
		#endif

		#if PLATFORM == PLATFORM_WINDOWS
			auto& framebufferX = GLOBAL::windowTransform.right;
			auto& framebufferY = GLOBAL::windowTransform.bottom;
		#else
			auto framebufferX = GLOBAL::windowTransform[2] / GLOBAL::viewportsCount;
			auto framebufferY = GLOBAL::windowTransform[3];
		#endif

		DEBUG_RENDER assert (
			GLOBAL::scene.screen != nullptr && 
			GLOBAL::scene.canvas != nullptr && 
			GLOBAL::scene.skybox != nullptr && 
			GLOBAL::scene.world != nullptr
		);

		auto& sharedScreen = GLOBAL::sharedScreen;
		auto& sharedCanvas = GLOBAL::sharedCanvas;
		auto& sharedWorld = GLOBAL::sharedWorld;
		
		auto& segmentWorlds = MANAGER::SCENES::GENERATOR::segmentsWorld;
		auto& segmentsCount = MANAGER::SCENES::GENERATOR::segmentsCount;

		auto& screen = *GLOBAL::scene.screen;
		auto& canvas = *GLOBAL::scene.canvas;
		auto& skybox = *GLOBAL::scene.skybox;
		auto& world = *GLOBAL::scene.world;

		auto& playersCount = GLOBAL::scene.world->playersCount;
        auto& players = GLOBAL::scene.world->players;

        UPDATE::StateMachine();

		UPDATE::MovePlayers();

        UPDATE::Checkpoints();

        UPDATE::MoveRigidbodies();
        for (int i = 0; i < 4; i++)
        {
            UPDATE::World(MANAGER::OBJECTS::GLTF::sharedWorlds[i], MANAGER::OBJECTS::GLTF::worlds[i]);
        }

        UPDATE::MovePlatforms();

		UPDATE::UpdateColliders();

		// Collisions have to be handled after world global transforms are updated
		UPDATE::Collisions ();

		UPDATE::UICollisions ();

        UPDATE::UpdatePowerUp();

        UPDATE::AnimateColliderObjects();

        for (int i = 0; i < 4; i++)
        {
            UPDATE::World(MANAGER::OBJECTS::GLTF::sharedWorlds[i], MANAGER::OBJECTS::GLTF::worlds[i]);
        }

		{ // UPDATES
			UPDATE::World (sharedWorld, world);
			UPDATE::Canvas (sharedCanvas, canvas);

			// SEGMENTS
			for (u8 iSegment = 0; iSegment < segmentsCount; ++iSegment) {
				auto& cWorld = segmentWorlds[iSegment];
				UPDATE::World (sharedWorld, cWorld);
			}

			// Update UI colliders, in the future check if the window's size is changed
			for (int i = 0; i < GLOBAL::canvas.collidersCount[COLLIDER::ColliderGroup::UI]; i++)
			{
				u64 rectangleIndex = OBJECT::ID_DEFAULT;
				OBJECT::GetComponentFast<RECTANGLE::LRectangle> (
						rectangleIndex, canvas.rectanglesCount,
						canvas.lRectangles, OBJECT::_09_SQUARE_1
				);

				auto& rectangle = canvas.lRectangles[rectangleIndex];

				COLLIDER::UpdateUICollider(GLOBAL::canvas.colliders[COLLIDER::ColliderGroup::UI][0], rectangle, GLOBAL::windowTransform[2], GLOBAL::windowTransform[3]);
			}
		}

		{ // RENDERS
			RENDER::Clear (GLOBAL::backgroundColor);

			for (int iViewport = 0; iViewport < GLOBAL::viewportsCount; iViewport++) {
				auto& viewport = GLOBAL::viewports[iViewport];
                auto& camTransform = GLOBAL::camTransform;
                auto& overlapVec = GLOBAL::camCollisionOffset;

                //MANAGER::OBJECTS::GLTF::worlds[i*2].lTransforms[0],
                        //MANAGER::OBJECTS::GLTF::worlds[((i+1)*2)%4].lTransforms[0]
				auto target =  MANAGER::OBJECTS::GLTF::worlds[iViewport*2].lTransforms[0].base.position;
					//old player GLOBAL::world.gTransforms[players[iViewport].local.transformIndex][3]

                // How much above should camera be ( 3rd person cam )
                target.y += 1.2f;

				s32 originX = framebufferX * iViewport;
				s32 originY = 0;
				RENDER::Base ( originX, originY, framebufferX, framebufferY);

				r32 ratio = (r32)framebufferX / (r32)framebufferY;

				//Screen (sharedScreen, screen);

				// Perspective Camera + Skybox



                // Update Camera Position in PLAY mode
                CAMERA::UpdateCamPos(viewport.camera, target);

                if(viewport.camera.type==CAMERA::THIRD_PERSON)
                {
                    // CameraMoveToTarget factor is speed of this move
                    viewport.camera.local.position += (viewport.camera.local.targetPos - viewport.camera.local.position) * 0.75f;
                    camTransform = glm::translate(glm::mat4(1.0f), viewport.camera.local.position);
                    COLLIDER::UpdateColliderTransform(GLOBAL::world.colliders[COLLIDER::ColliderGroup::CAMERA][iViewport], camTransform);

                    overlapVec = {0, 0, 0};
                    CheckOBBCollisionsSingleCollider(GLOBAL::world.colliders[COLLIDER::ColliderGroup::CAMERA][iViewport], COLLIDER::ColliderGroup::MAP, GLOBAL::scene.world->colliders, GLOBAL::scene.world->collidersCount, overlapVec);
                    camTransform = glm::translate(glm::mat4(1.0f), viewport.camera.local.position - overlapVec );
                    viewport.camera.local.position = camTransform[3];
                    CAMERA::UpdateCameraVectors(viewport.camera);
                }


                // Camera view & projection
                viewport.view = glm::mat4 ( glm::mat3( GetViewMatrix (viewport.camera, target) ) );
				viewport.projection = glm::perspective (
					glm::radians (viewport.camera.local.zoom),
					ratio, 0.1f, 110.0f
				);

				viewport.cameraFrustum = viewport.cameraFrustum.createFrustumFromCamera (
					viewport.camera, ratio,
					glm::radians (viewport.camera.local.zoom),
					0.1f, 100.0f
				);

				RENDER::Skybox (skybox, viewport.projection, viewport.view);

				// Perspective Camera - Skybox
				viewport.view = GetViewMatrix (viewport.camera, target);

				// SET up camera position
				SHADER::UNIFORM::BUFFORS::viewPosition = viewport.camera.local.position;
				RENDER::World (sharedWorld, world, viewport.projection, viewport.view, viewport.cameraFrustum);

				// gltfs
				for (u16 i = 0; i < RESOURCES::MANAGER::GLTFS::HANDLERS_COUNT; ++i) {
					RENDER::World (
						MANAGER::OBJECTS::GLTF::sharedWorlds[i], 
						MANAGER::OBJECTS::GLTF::worlds[i], 
						viewport.projection, viewport.view, viewport.cameraFrustum
					);
				}

				// SEGMENTS
				for (u8 iSegment = 0; iSegment < segmentsCount; ++iSegment) {
					auto& cWorld = segmentWorlds[iSegment];
					RENDER::World (sharedWorld, cWorld, viewport.projection, viewport.view, viewport.cameraFrustum);
				}
			}

			// EDIT MODE ONLY
			DEBUG if (EDITOR::mode == EDITOR::EDIT_MODE) {
				IMGUI::RENDER::World (
					*(ImVec4*)(&GLOBAL::backgroundColor), GLOBAL::viewports[0].view, GLOBAL::viewports[0].projection,
					GLOBAL::world.lTransforms, GLOBAL::world.gTransforms, GLOBAL::world.transformsCount
				);

				IMGUI::RENDER::Post ();
			}

			EDITOR_PLAY_MODE_OR_RELEASE_ONLY ({ 
				// Orthographic Camera
				glViewport (GLOBAL::windowTransform[0], GLOBAL::windowTransform[1], GLOBAL::windowTransform[2], GLOBAL::windowTransform[3]);
				glm::mat4 projection = glm::ortho (0.0f, (float)GLOBAL::windowTransform[2], 0.0f, (float)GLOBAL::windowTransform[3]);

				//RENDER::Canvas (sharedCanvas, canvas, projection);
			})

		}

		#if PLATFORM == PLATFORM_WINDOWS
			SwapBuffers (WIN::LOADER::graphicalContext);
		#else
			glfwSwapBuffers (GLOBAL::mainWindow);
			//TracyGpuCollect;
		#endif
	}

}
