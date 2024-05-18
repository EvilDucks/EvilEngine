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
		glDepthFunc(GL_LESS);
		
		//glPolygonMode ( GL_FRONT_AND_BACK, GL_LINE );
		glActiveTexture (GL_TEXTURE0);
	}

	// GLOBALS
	glm::mat4 view, projection;
	r32 ratio;
	
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
			auto& framebufferX = GLOBAL::windowTransform[2];
			auto& framebufferY = GLOBAL::windowTransform[3];
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

		UPDATE::Collisions (
			GLOBAL::scene.world->colliders, GLOBAL::scene.world->collidersCount, 
			GLOBAL::players, GLOBAL::playerCount
		);

		UPDATE::UICollisions (
			GLOBAL::scene.canvas->colliders, GLOBAL::scene.canvas->collidersCount,
			GLOBAL::players, GLOBAL::playerCount
		);

		{ // UPDATES
			UPDATE::World (sharedWorld, world);
			UPDATE::Canvas (sharedCanvas, canvas);

			// SEGMENTS
			//for (u8 iSegment = 0; iSegment < GLOBAL::segmentsCount; ++iSegment) { 
			//	auto& cWorld = segmentWorlds[iSegment];
			//	UPDATE::World (sharedWorld, cWorld);
			//}
		}

		{ // RENDERS
			RENDER::Base (GLOBAL::backgroundColor, framebufferX, framebufferY);
			ratio = (r32)framebufferX / (r32)framebufferY;

			//Screen (sharedScreen, screen);

			// Perspective Camera + Skybox
			view = glm::mat4 ( glm::mat3( GetViewMatrix (world.camera) ) );

			projection = glm::perspective (
				glm::radians(world.camera.local.zoom),
				ratio, 0.1f, 100.0f
			);

			world.camFrustum = world.camFrustum.createFrustumFromCamera(
				world.camera, ratio, glm::radians(world.camera.local.zoom),
				0.1f, 100.0f
			);

			RENDER::Skybox (skybox, projection, view);
			
			// Perspective Camera - Skybox
			view = GetViewMatrix (world.camera);
			// SET up camera position
			SHADER::UNIFORM::BUFFORS::viewPosition = world.camera.local.position;
			RENDER::World (sharedWorld, world, projection, view);

			// SEGMENTS
			//for (u8 iSegment = 0; iSegment < GLOBAL::segmentsCount; ++iSegment) { 
			//	auto& cWorld = segmentWorlds[iSegment];
			//	RENDER::World (sharedWorld, cWorld, projection, view);
			//}

			DEBUG if (GLOBAL::mode == EDITOR::EDIT_MODE) {
				IMGUI::Render (
					*(ImVec4*)(&GLOBAL::backgroundColor), view, projection, 
					GLOBAL::world.lTransforms, GLOBAL::world.gTransforms, GLOBAL::world.transformsCount
				);
			}

			// Orthographic Camera
			projection = glm::ortho (0.0f, (float)framebufferX, 0.0f, (float)framebufferY);
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
