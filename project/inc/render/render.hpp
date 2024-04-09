#pragma once
#include "systems.hpp"
#include "global.hpp"

namespace RENDER {

	void Render ();
	void UpdateFrame ( SCENE::Scene& scene );
	void RenderFrame ( Color4& backgroundColor, SCENE::Scene& scene );
	
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
			wglMakeCurrent (WIN::LOADER::graphicalContext, WIN::LOADER::openGLRenderContext);
			SwapBuffers (WIN::LOADER::graphicalContext);
		#else
			glfwMakeContextCurrent(GLOBAL::mainWindow);
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
		u8 prevMaterialMeshes = 0;

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

				DEBUG if (materials == nullptr) {
					spdlog::error ("Screen has no materials assigned!");
					exit (1);
				}

				auto& materialMeshesCount = materialMeshTable[1 + prevMaterialMeshes + materialIndex];
				auto& material = materials[materialIndex];

				u64 meshIndex = 0;

				// { Example of Changing Uniform Buffor
				float timeValue = materialIndex; // + glfwGetTime ();
				float greenValue = (sin (timeValue) / 2.0f) + 0.5f;
				GLOBAL::ubColor = { 0.0f, greenValue, 0.0f, 1.0f };
				// }

				DEBUG if (material.program.id == 0) {
					spdlog::error ("Screen material {0} not properly created!", materialIndex);
					exit (1);
				}

				SHADER::Use (material.program);

				// Some draw optimalizations might actually make it harder here.
				//SHADER::UNIFORM::SetsMaterial (material.program);

				for (; meshIndex < materialMeshesCount; ++meshIndex) {

					auto& meshId = materialMeshTable[2 + prevMaterialMeshes + meshIndex];
					auto& mesh = meshes[meshId].base;

					// DEBUG! Check if MESHID is valid !
					
					DEBUG if (mesh.vao == 0) {
						spdlog::error ("Screen mesh {0} not properly created!", meshIndex);
						exit (1);
					}
					
					SHADER::UNIFORM::SetsMesh (material.program);
					
					glBindVertexArray (mesh.vao); // BOUND VAO
					mesh.drawFunc (GL_TRIANGLES, mesh.verticiesCount);
					glBindVertexArray (0); // UNBOUND VAO

				}

				prevMaterialMeshes += meshIndex;
			}

			prevMaterialMeshes = 0;
		}


		{ // Render Camera Object

			auto& materialMeshTable = (*scene.world).materialMeshTable;
			auto& materialsCount = (*scene.world).materialsCount;
			auto& transforms = (*scene.world).transforms;
			auto& materials = (*scene.world).materials;
			auto& meshes = (*scene.world).meshes;

			u64 transformsCounter = TRANSFORMS_ROOT_OFFSET;

            view = GetViewMatrix(scene.world->camera);

			//DEBUG spdlog::info ("here1");

            projection = glm::perspective (
                    /*glm::radians(45.0f),*/ glm::radians((scene.world->camera.local.zoom)),
                                             (float)framebufferX / (float)framebufferY,
                                             0.1f,
                                             100.0f
            );

			for (u64 materialIndex = 0; materialIndex < materialsCount; ++materialIndex) {
				
				DEBUG if (materials == nullptr) {
					spdlog::error ("World has no materials assigned!");
					exit (1);
				}

				auto& materialMeshesCount = materialMeshTable[1 + prevMaterialMeshes + materialIndex];
				auto& material = materials[materialIndex];

				u64 meshIndex = 0;

				DEBUG if (material.program.id == 0) {
					spdlog::error ("World material {0} not properly created!", materialIndex);
					exit (1);
				}

				SHADER::Use (material.program);
				GLOBAL::ubProjection = projection;
				GLOBAL::ubView = view;

				for (; meshIndex < materialMeshesCount; ++meshIndex) {
					
					auto& meshId = materialMeshTable[2 + prevMaterialMeshes + meshIndex];
					auto& mesh = meshes[meshId].base;

					// DEBUG! Check if MESHID is valid !

					DEBUG if (mesh.vao == 0) {
						spdlog::error ("World mesh {0} not properly created!", meshIndex);
						exit (1);
					}

					//DEBUG {
					//	auto& a = transforms[transformsCounter].global;
					//	spdlog::info ("mi: {0}, x: {1}", meshIndex, a[0][0]);
					//}

					GLOBAL::ubGlobalSpace = transforms[transformsCounter].global;
					SHADER::UNIFORM::SetsMesh (material.program);
					//
					glBindVertexArray (mesh.vao); // BOUND VAO
					mesh.drawFunc (GL_TRIANGLES, mesh.verticiesCount);
					glBindVertexArray (0); // UNBOUND VAO
					//
					++transformsCounter;
				}
				prevMaterialMeshes += meshIndex;
			}
			prevMaterialMeshes = 0;
		}
		
	}

	void UpdateFrame ( SCENE::Scene& scene ) {

		const u64 WORLD_ROOT_ID = 0;
		auto& world = *scene.world;
		
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