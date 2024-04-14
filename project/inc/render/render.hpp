#pragma once
#include "systems.hpp"
#include "global.hpp"

namespace RENDER {

	ANIMATION::Animation sharedAnimation1 { 1.0f, 6, 0, 0.0f, 0 };


	void InitializeRender();
	void Render ();
	void UpdateFrame ( SCENE::Scene& scene );
	void RenderFrame ( Color4& backgroundColor, SCENE::Scene& scene );


	void InitializeRender () {
		glEnable (GL_BLEND);
		glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);  
		//glPolygonMode ( GL_FRONT_AND_BACK, GL_LINE );
		glActiveTexture (GL_TEXTURE0);
	}
		
	
	void Render () {
		DEBUG { IMGUI::Render (*(ImVec4*)(&GLOBAL::backgroundColor)); }

		#if PLATFORM == PLATFORM_WINDOWS
			wglMakeCurrent (WIN::LOADER::graphicalContext, WIN::LOADER::openGLRenderContext);
		#else
			glfwMakeContextCurrent(GLOBAL::mainWindow);
		#endif

		UpdateFrame (GLOBAL::scene);
		RenderFrame (GLOBAL::backgroundColor, GLOBAL::scene);
		DEBUG { IMGUI::PostRender (); }

		#if PLATFORM == PLATFORM_WINDOWS
			SwapBuffers (WIN::LOADER::graphicalContext);
		#else
			glfwSwapBuffers(GLOBAL::mainWindow);
		#endif
	}
	

	void RenderFrame (
		Color4& backgroundColor,
		SCENE::Scene& scene
	) {
		const u64 TRANSFORMS_ROOT_OFFSET = 1;


		//DEBUG spdlog::info ("here2");

		glm::mat4 view = glm::mat4(1.0f);
		glm::mat4 projection = glm::mat4(1.0f);

		#if PLATFORM == PLATFORM_WINDOWS
			auto& framebufferX = GLOBAL::windowTransform.right;
			auto& framebufferY = GLOBAL::windowTransform.bottom;
		#else
			auto& framebufferX = GLOBAL::windowTransform[2];
			auto& framebufferY = GLOBAL::windowTransform[3];
		#endif

		glViewport (0, 0, framebufferX, framebufferY);

		glClearColor (
			backgroundColor.r * backgroundColor.a, 
			backgroundColor.g * backgroundColor.a, 
			backgroundColor.b * backgroundColor.a, 
			backgroundColor.a
		);

		glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		{ // Render Screen Object

			auto& materialMeshTable = (*scene.screen).materialMeshTable;
			auto& materialsCount = (*scene.screen).materialsCount;
			auto& materials = (*scene.screen).materials;
			auto& meshes = (*scene.screen).meshes;

			for (u64 materialIndex = 0; materialIndex < materialsCount; ++materialIndex) {

				DEBUG_RENDER if (materials == nullptr) {
					spdlog::error ("Screen has no materials assigned!");
					exit (1);
				}

				const auto& materialMeshesCount = *MATERIAL::MESHTABLE::GetMeshCount (materialMeshTable, materialIndex);
				auto& material = materials[materialIndex];

				u64 meshIndex = 0;

				// { Example of Changing Uniform Buffor
				float timeValue = materialIndex + GLOBAL::timeCurrent;
				float greenValue = (sin (timeValue) / 2.0f) + 0.5f;
				SHADER::UNIFORM::BUFFORS::color = { 0.0f, greenValue, 0.0f, 1.0f };
				// }

				DEBUG_RENDER if (material.program.id == 0) {
					spdlog::error ("Screen material {0} not properly created!", materialIndex);
					exit (1);
				}

				SHADER::Use (material.program);
				SHADER::UNIFORM::SetsMaterial (material.program);
				SHADER::UNIFORM::BUFFORS::sampler1.texture = material.texture;
				SHADER::UNIFORM::BUFFORS::tile = sharedAnimation1.frameCurrent;
				const float shift = GLOBAL::timeCurrent * 0.25f;
				SHADER::UNIFORM::BUFFORS::shift = { shift, shift };

				for (; meshIndex < materialMeshesCount; ++meshIndex) {
					const auto& meshId = *MATERIAL::MESHTABLE::GetMesh (materialMeshTable, materialIndex, meshIndex);
					auto& mesh = meshes[meshId].base;
					
					DEBUG_RENDER if (mesh.vao == 0) {
						spdlog::error ("Screen mesh {0} not properly created!", meshIndex);
						exit (1);
					}

					SHADER::UNIFORM::SetsMesh (material.program);

					glBindVertexArray (mesh.vao); // BOUND VAO
					DEBUG_RENDER  GL::GetError (GL::ET::PRE_DRAW_BIND_VAO);
					mesh.drawFunc (GL_TRIANGLES, mesh.verticiesCount);
					glBindVertexArray (0); // UNBOUND VAO

				}
				MATERIAL::MESHTABLE::AddRead (meshIndex);
			}
			MATERIAL::MESHTABLE::SetRead (0);
		}


		{ // Render Camera Object

			auto& materialMeshTable = (*scene.world).materialMeshTable;
			auto& materialsCount = (*scene.world).materialsCount;
			auto& transforms = (*scene.world).transforms;
			auto& materials = (*scene.world).materials;
			auto& meshes = (*scene.world).meshes;

			u64 transformsCounter = TRANSFORMS_ROOT_OFFSET;

			view = GetViewMatrix (scene.world->camera);

			//DEBUG spdlog::info ("here1");

            projection = glm::perspective (
                    /*glm::radians(45.0f),*/ glm::radians((scene.world->camera.local.zoom)),
                                             (float)framebufferX / (float)framebufferY,
                                             0.1f,
                                             100.0f
            );

			for (u64 materialIndex = 0; materialIndex < materialsCount; ++materialIndex) {
				
				DEBUG_RENDER if (materials == nullptr) {
					spdlog::error ("World has no materials assigned!");
					exit (1);
				}

				const auto& materialMeshesCount = *MATERIAL::MESHTABLE::GetMeshCount (materialMeshTable, materialIndex);
				auto& material = materials[materialIndex];

				u64 meshIndex = 0;

				DEBUG if (material.program.id == 0) {
					spdlog::error ("World material {0} not properly created!", materialIndex);
					exit (1);
				}

				SHADER::Use (material.program);
				SHADER::UNIFORM::SetsMaterial (material.program);
				SHADER::UNIFORM::BUFFORS::projection = projection;
				SHADER::UNIFORM::BUFFORS::view = view;
				SHADER::UNIFORM::BUFFORS::sampler1.texture = material.texture; 

				for (; meshIndex < materialMeshesCount; ++meshIndex) {
					const auto& meshId = *MATERIAL::MESHTABLE::GetMesh (materialMeshTable, materialIndex, meshIndex);
					auto& mesh = meshes[meshId].base;

					DEBUG_RENDER if (mesh.vao == 0) {
						spdlog::error ("World mesh {0} not properly created!", meshIndex);
						exit (1);
					}

					SHADER::UNIFORM::BUFFORS::globalSpace = transforms[transformsCounter].global;
					SHADER::UNIFORM::SetsMesh (material.program);

					glBindVertexArray (mesh.vao); // BOUND VAO
					DEBUG_RENDER  GL::GetError (GL::ET::PRE_DRAW_BIND_VAO);
					mesh.drawFunc (GL_TRIANGLES, mesh.verticiesCount);
					glBindVertexArray (0); // UNBOUND VAO
					++transformsCounter;
				} 
				MATERIAL::MESHTABLE::AddRead (meshIndex);
			} 
			MATERIAL::MESHTABLE::SetRead (0);
		}


		
		{ // CANVAS
			auto& program = FONT::faceShader;
			SHADER::UNIFORM::BUFFORS::projection = glm::ortho (0.0f, (float)framebufferX, 0.0f, (float)framebufferY);
			SHADER::Use (program);
			SHADER::UNIFORM::SetsMaterial (program);
			{
				SHADER::UNIFORM::BUFFORS::color = { 0.5, 0.8f, 0.2f, 1.0f };
				SHADER::UNIFORM::SetsMesh (program);
				FONT::RenderText (19 - (u16)sharedAnimation1.frameCurrent, "This is sample text", 25.0f, 25.0f, 1.0f);
			}
			{
				SHADER::UNIFORM::BUFFORS::color = { 0.3, 0.7f, 0.9f, 1.0f };
				SHADER::UNIFORM::SetsMesh (program);
				FONT::RenderText (19 - (u16)sharedAnimation1.frameCurrent, "(C) LearnOpenGL.com", 540.0f, 570.0f, 0.5f);
			}
			
		}
		
		
	}

	void UpdateFrame ( SCENE::Scene& scene ) {

		const u64 WORLD_ROOT_ID = 0;
		auto& world = *scene.world;

		
		{ // Recalculating Time Variables.
			GLOBAL::timeCurrent = glfwGetTime();
			GLOBAL::timeDelta = GLOBAL::timeCurrent - GLOBAL::timeSinceLastFrame;
			GLOBAL::timeSinceLastFrame = GLOBAL::timeCurrent;

			{ // For each animation loop?
				ANIMATION::Update (sharedAnimation1, GLOBAL::timeDelta);
			}
		}
		
		
		// Rotate ENTITY_4 so it's child will rotate too
		//  Find ENTITY_4 TRANSFORM then find it's children
		//  For each child and their child and cheir child recalculate their globalspace.

		{
			assert(world.parenthoodsCount == 2);
			//
			auto& transformsCount = world.transformsCount;
			auto& transforms = world.transforms;
			auto& thisParenthood = world.parenthoods[1];	// Get node (child of root)
			auto& parent = thisParenthood.id;
			auto& child = thisParenthood.base.children[0];	// Get node (child of child)
			auto& transformIndex = SYSTEMS::tempIndex;
			//
			{ // THIS
				transformIndex = OBJECT::ID_DEFAULT;
				//
				OBJECT::GetComponentFast<TRANSFORM::Transform> (
					transformIndex, transformsCount, transforms, parent
				);
				//
				auto& thisTransfrom = transforms[transformIndex];
				thisTransfrom.local.rotation.z += 1; 
				thisTransfrom.flags = TRANSFORM::DIRTY;
			}
			{ // CHILD
				transformIndex = OBJECT::ID_DEFAULT;
				//
				OBJECT::GetComponentFast<TRANSFORM::Transform> (
					transformIndex, transformsCount, transforms, child
				);
				//
				auto& thisTransfrom = transforms[transformIndex];
				thisTransfrom.local.rotation.y += 1; 
				thisTransfrom.flags = TRANSFORM::DIRTY;
			}
		}

		SYSTEMS::ApplyDirtyFlag (
			world.parenthoodsCount, world.parenthoods,
			world.transformsCount, world.transforms
		);

	}

}