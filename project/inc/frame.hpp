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
		
		auto& segmentWorlds = GLOBAL::segmentsWorld;
		auto& screen = *GLOBAL::scene.screen;
		auto& canvas = *GLOBAL::scene.canvas;
		auto& skybox = *GLOBAL::scene.skybox;
		auto& world = *GLOBAL::scene.world;
		

		UPDATE::MovePlayers();

		FORCE::ApplyForces (GLOBAL::forces, GLOBAL::world.lTransforms, GLOBAL::world.gTransforms, float(GLOBAL::timeDelta));

		UPDATE::UpdateColliders();

		// Collisions have to be handled after world global transforms are updated
		UPDATE::Collisions ();

		UPDATE::UICollisions ();

		{ // UPDATES
			UPDATE::World (sharedWorld, world);
			UPDATE::Canvas (sharedCanvas, canvas);

			// SEGMENTS
			for (u8 iSegment = 0; iSegment < GLOBAL::segmentsCount; ++iSegment) {
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

				auto target = glm::vec3 (
					GLOBAL::world.gTransforms[GLOBAL::players[iViewport].local.transformIndex][3]
				);

				s32 originX = framebufferX * iViewport;
				s32 originY = 0;
				RENDER::Base ( originX, originY, framebufferX, framebufferY);

				r32 ratio = (r32)framebufferX / (r32)framebufferY;

				//Screen (sharedScreen, screen);

				// Perspective Camera + Skybox
				viewport.view = glm::mat4 ( glm::mat3( GetViewMatrix (viewport.camera, target) ) );
				viewport.projection = glm::perspective (
					glm::radians (viewport.camera.local.zoom),
					ratio, 0.1f, 100.0f
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

				// SEGMENTS
				for (u8 iSegment = 0; iSegment < GLOBAL::segmentsCount; ++iSegment) {
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

				RENDER::Canvas (sharedCanvas, canvas, projection);
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
