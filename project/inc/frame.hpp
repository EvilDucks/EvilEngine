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

	// GLOBALS
	r32 ratio;
    std::vector<VIEWPORT::data> viewPort;
	
	void Frame () {
		PROFILER { ZoneScopedN("Render: Frame"); }

		#if PLATFORM == PLATFORM_WINDOWS
			wglMakeCurrent (WIN::LOADER::graphicalContext, WIN::LOADER::openGLRenderContext);
		#else
			glfwMakeContextCurrent(GLOBAL::mainWindow);
		#endif

		#if PLATFORM == PLATFORM_WINDOWS
			auto& framebufferX = GLOBAL::windowTransform.right;
			auto& framebufferY = GLOBAL::windowTransform.bottom;
		#else
			auto framebufferX = GLOBAL::windowTransform[2]/GLOBAL::viewPortCount;
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
		
		auto& segmentWorlds = GLOBAL::segmentsWorld;
		auto& screen = *GLOBAL::scene.screen;
		auto& canvas = *GLOBAL::scene.canvas;
		auto& skybox = *GLOBAL::scene.skybox;
		auto& world = *GLOBAL::scene.world;
        viewPort = world.viewPortDatas;

        UPDATE::MovePlayers();

        FORCE::ApplyForces(GLOBAL::forces, GLOBAL::world.lTransforms, GLOBAL::world.gTransforms, float(GLOBAL::timeDelta));

        UPDATE::UpdateColliders();

        // Collisions have to be handled after world global transforms are updated
        UPDATE::Collisions ();

        UPDATE::UICollisions ();

		{ // UPDATES
			UPDATE::World (sharedWorld, world);
			UPDATE::Canvas (sharedCanvas, canvas);

			// SEGMENTS
			//for (u8 iSegment = 0; iSegment < GLOBAL::segmentsCount; ++iSegment) { 
			//	auto& cWorld = segmentWorlds[iSegment];
			//	UPDATE::World (sharedWorld, cWorld);
			//}

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

        //UPDATE::SetCamPositions();
        //for(int i = 0; i < GLOBAL::playerCount; i++)
       // {
            //glm::vec3 difference = GLOBAL::world.viewPortDatas[i].camera.local.position - GLOBAL::camPos[i];
            //GLOBAL::world.viewPortDatas[i].camera.local.position -= difference * 0.25f;
            //glm::vec3 front = glm::vec3(1.f, 0.f, 0.f);
            //front = glm::rotate(front, glm::radians(GLOBAL::players[i].local.movement.yaw), glm::vec3(0.f, 1.f, 0.f));
            //GLOBAL::world.viewPortDatas[i].camera.local.front = -front;
            //updateCameraVectors(GLOBAL::world.viewPortDatas[i].camera);
       // }

		{ // RENDERS
            RENDER::Clear (GLOBAL::backgroundColor);
            for (int i = 0; i < GLOBAL::viewPortCount; i++) {

                auto target = glm::vec3(GLOBAL::world.gTransforms[GLOBAL::players[i].local.transformIndex][3]);

                s32 originX = framebufferX * i;
                s32 originY = 0;
                RENDER::Base ( originX, originY, framebufferX, framebufferY);

                ratio = (r32)framebufferX / (r32)framebufferY;

                //Screen (sharedScreen, screen);

                // Perspective Camera + Skybox

                // Update Camera Position in PLAY mode
                CAMERA::UpdateCamPos(viewPort[i].camera, target);

                std::cout << "  " << viewPort[i].colliderIndex << " " << std::endl;

                GLOBAL::world.gTransforms[viewPort[i].colliderIndex] = glm::translate(glm::mat4(1.0f), viewPort[i].camera.local.position);
                COLLIDER::UpdateColliderTransform(GLOBAL::world.colliders[COLLIDER::ColliderGroup::CAMERA][i], GLOBAL::world.gTransforms[viewPort[i].colliderIndex]);
                glm::vec3 overlapVec;
                CheckOBBCollisionsOne(GLOBAL::world.colliders[COLLIDER::ColliderGroup::CAMERA][i], COLLIDER::ColliderGroup::MAP, GLOBAL::scene.world->colliders, GLOBAL::scene.world->collidersCount, overlapVec);

                viewPort[i].view = glm::mat4 ( glm::mat3( GetViewMatrix (viewPort[i].camera, target) ) );

                viewPort[i].projection = glm::perspective (
                        glm::radians (viewPort[i].camera.local.zoom),
                        ratio, 0.1f, 100.0f
                );

                viewPort[i].camFrustum = viewPort[i].camFrustum.createFrustumFromCamera (
                    viewPort[i].camera, ratio,
					glm::radians (viewPort[i].camera.local.zoom),
                    0.1f, 100.0f
                );

                RENDER::Skybox (skybox, viewPort[i].projection, viewPort[i].view);

                // Perspective Camera - Skybox
                viewPort[i].view = GetViewMatrix (viewPort[i].camera, target);

                // SET up camera position
                SHADER::UNIFORM::BUFFORS::viewPosition = viewPort[i].camera.local.position;
                RENDER::World (sharedWorld, world, viewPort[i].projection, viewPort[i].view, viewPort[i].camFrustum);

                // SEGMENTS
                //for (u8 iSegment = 0; iSegment < GLOBAL::segmentsCount; ++iSegment) {
                //	auto& cWorld = segmentWorlds[iSegment];
                //	RENDER::World (sharedWorld, cWorld, viewPort[i].projection, viewPort[i].view);
                // }
            }

            DEBUG if (GLOBAL::mode == EDITOR::EDIT_MODE) {
                    IMGUI::Render (
                            *(ImVec4*)(&GLOBAL::backgroundColor), viewPort[0].view, viewPort[0].projection,
                            GLOBAL::world.lTransforms, GLOBAL::world.gTransforms, GLOBAL::world.transformsCount
                    );
            }

			// Orthographic Camera
            glViewport (GLOBAL::windowTransform[0], GLOBAL::windowTransform[1], GLOBAL::windowTransform[2], GLOBAL::windowTransform[3]);
			glm::mat4 projection = glm::ortho (0.0f, (float)GLOBAL::windowTransform[2], 0.0f, (float)GLOBAL::windowTransform[3]);
			RENDER::Canvas (sharedCanvas, canvas, projection);

			DEBUG if (GLOBAL::mode == EDITOR::EDIT_MODE) {
				IMGUI::PostRender ();
			}
		}

		#if PLATFORM == PLATFORM_WINDOWS
			SwapBuffers (WIN::LOADER::graphicalContext);
		#else
			glfwSwapBuffers (GLOBAL::mainWindow);
			//TracyGpuCollect;
		#endif
	}

}
