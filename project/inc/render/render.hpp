#pragma once
#include "systems.hpp"
#include "global.hpp"

namespace RENDER {

	void Render ();
	void UpdateFrame ( SCENE::Scene& scene );
	void RenderFrame ( Color4& backgroundColor, SCENE::Scene& scene );
	
	void Render () {
		IMGUI::Render (*(ImVec4*)(&GLOBAL::backgroundColor));

		#if PLATFORM == PLATFORM_WINDOWS
			wglMakeCurrent (WIN::LOADER::graphicalContext, WIN::LOADER::openGLRenderContext);
		#else
			glfwMakeContextCurrent(GLOBAL::mainWindow);
		#endif

		UpdateFrame (GLOBAL::scene);
		RenderFrame (GLOBAL::backgroundColor, GLOBAL::scene);
		IMGUI::PostRender ();

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
		const u64 ROOT_OFFSET = 1;

		// For 3D world representation.
		glm::mat4 view = glm::mat4(1.0f);
		glm::mat4 projection = glm::mat4(1.0f);
		//glm::mat4 localSpace = glm::mat4(1.0f);

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

			auto& screen = *scene.screen;

			// We dont render for each mesh. We render for each material !
			for (u64 i = 0; i < screen.materialsCount; ++i) {

				DEBUG if (screen.materials == nullptr) {
					spdlog::error ("Screen has no materials assigned!");
					exit (1);
				}

				auto& material = screen.materials[i];

				// { Example of Changing Uniform Buffor
				float timeValue = i; // + glfwGetTime ();
				float greenValue = (sin (timeValue) / 2.0f) + 0.5f;
				GLOBAL::ubColor = { 0.0f, greenValue, 0.0f, 1.0f };
				// }

				DEBUG if (material.program.id == 0) {
					spdlog::error ("Screen material {0} not properly created!", i);
					exit (1);
				}

				SHADER::Use (material.program);

				// Some draw optimalizations might actually make it harder here.
				//SHADER::UNIFORM::SetsMaterial (material.program);

				for (u64 j = 0; j < material.meshes.length; ++j) {

					DEBUG if (material.meshes.data == nullptr) {
						spdlog::error ("Screen material has no meshes assigned!");
						exit (1);
					}

					auto &mesh = ((MESH::Mesh*)(material.meshes.data))[j].base;

					DEBUG if (mesh.vao == 0) {
						spdlog::error ("Screen mesh {0} not properly created!", j);
						exit (1);
					}

					SHADER::UNIFORM::SetsMesh (material.program);

					glBindVertexArray (mesh.vao); // BOUND VAO
					mesh.drawFunc (GL_TRIANGLES, mesh.verticiesCount);
					glBindVertexArray (0); // UNBOUND VAO

				}
			}
		}


		{ // Render Camera Object

			view = glm::translate(view, glm::vec3(0.0, 0.0, -8.0));

			projection = glm::perspective (
				/*glm::radians(45.0f),*/ glm::radians(60.0f),
				(float)framebufferX / (float)framebufferY,
				0.1f,
				100.0f
			);

			// globalspace
			//localSpace = glm::translate(localSpace, glm::vec3(1.0, 1.0, 1.0));

			u64 globalIndex = 0;
			auto& world = *scene.world;

			for (u64 i = 0; i < world.materialsCount; ++i) {
				
				DEBUG if (world.materials == nullptr) {
					spdlog::error ("World has no materials assigned!");
					exit (1);
				}

				auto& material = world.materials[i];

				DEBUG if (material.program.id == 0) {
					spdlog::error ("World material {0} not properly created!", i);
					exit (1);
				}

				SHADER::Use (material.program);

				GLOBAL::ubProjection = projection;
				GLOBAL::ubView = view;

				for (u64 j = 0; j < material.meshes.length; ++j) { // 2

					DEBUG if (material.meshes.data == nullptr) {
						spdlog::error ("World material has no meshes assigned!");
						exit (1);
					}

					auto &mesh = ((MESH::Mesh*)(material.meshes.data))[j].base;

					DEBUG if (mesh.vao == 0) {
						spdlog::error ("World mesh {0} not properly created!", j);
						exit (1);
					}

					GLOBAL::ubGlobalSpace = world.transforms[globalIndex + ROOT_OFFSET].global;

					SHADER::UNIFORM::SetsMesh (material.program);

					glBindVertexArray (mesh.vao); // BOUND VAO
					mesh.drawFunc (GL_TRIANGLES, mesh.verticiesCount);
					glBindVertexArray (0); // UNBOUND VAO

					++globalIndex;
				}

			}

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