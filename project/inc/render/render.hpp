#pragma once
#include "systems.hpp"
#include "global.hpp"
#include <tracy/Tracy.hpp>
#include <tracy/TracyOpenGL.hpp>

namespace RENDER {

	ANIMATION::Animation sharedAnimation1 { 1.0f, 6, 0, 0.0f, 0 };


	void Initialize();
	void Frame ();

	void Update ( SCENE::Scene& scene );
	void Base ( const Color4& backgroundColor, s32& framebufferX, s32& framebufferY );

	void Screen ( const SCENE::Screen& screen );
	void World ( const SCENE::World& world, const glm::mat4& projection, const glm::mat4& view );
	void Canvas ( const SCENE::Canvas& canvas, const glm::mat4& projection );


	void Initialize () {
		ZoneScopedN ("Render: InitializeRender");
		glEnable (GL_BLEND);
		glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glDepthFunc(GL_LESS);
		//glPolygonMode ( GL_FRONT_AND_BACK, GL_LINE );
		glActiveTexture (GL_TEXTURE0);
	}
		
	
	void Frame () {
		ZoneScopedN("Render: Render");
		DEBUG { IMGUI::Render (*(ImVec4*)(&GLOBAL::backgroundColor)); }

		#if PLATFORM == PLATFORM_WINDOWS
			wglMakeCurrent (WIN::LOADER::graphicalContext, WIN::LOADER::openGLRenderContext);
		#else
			glfwMakeContextCurrent(GLOBAL::mainWindow);
		#endif

		Update (GLOBAL::scene);

		#if PLATFORM == PLATFORM_WINDOWS
			auto& framebufferX = GLOBAL::windowTransform.right;
			auto& framebufferY = GLOBAL::windowTransform.bottom;
		#else
			auto& framebufferX = GLOBAL::windowTransform[2];
			auto& framebufferY = GLOBAL::windowTransform[3];
		#endif

		glm::mat4 view, projection;

		DEBUG_RENDER assert (
			GLOBAL::scene.screen != nullptr && 
			GLOBAL::scene.canvas != nullptr && 
			GLOBAL::scene.world != nullptr
		);

		auto& screen = *GLOBAL::scene.screen;
		auto& canvas = *GLOBAL::scene.canvas;
		auto& world = *GLOBAL::scene.world;

		{ // Actual components of a release-build frame.
			ZoneScopedN("Render: Frame");

			Base (GLOBAL::backgroundColor, framebufferX, framebufferY);
			Screen (screen);

			// Perspective Camera
			view = GetViewMatrix (world.camera);
			projection = glm::perspective (
				glm::radians(world.camera.local.zoom),
				(float)framebufferX / (float)framebufferY,
				0.1f, 100.0f
			);

			World (world, projection, view);

			// Orthographic Camera
			//projection = glm::ortho (0.0f, (float)framebufferX, 0.0f, (float)framebufferY);

			Canvas (canvas, projection);
		}
		

		DEBUG { IMGUI::PostRender (); }

		#if PLATFORM == PLATFORM_WINDOWS
			SwapBuffers (WIN::LOADER::graphicalContext);
		#else
			glfwSwapBuffers (GLOBAL::mainWindow);
			//TracyGpuCollect;
		#endif
	}


	void Base (
		const Color4& backgroundColor,
		s32& framebufferX,
		s32& framebufferY
	) {
		glViewport (0, 0, framebufferX, framebufferY);

		glClearColor (
			backgroundColor.r * backgroundColor.a, 
			backgroundColor.g * backgroundColor.a, 
			backgroundColor.b * backgroundColor.a, 
			backgroundColor.a
		);

		glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}


	void Screen (
		const SCENE::Screen& screen
	) {
		ZoneScopedN("Render Screen Object");

		glDisable (GL_DEPTH_TEST);
		u16 uniformsTableBytesRead = 0;

		auto& uniformsTable = screen.uniformsTable;
		auto& materialMeshTable = screen.materialMeshTable;
		auto& materialsCount = screen.materialsCount;
		auto& materials = screen.materials;
		auto& meshes = screen.meshes;

		assert (uniformsTable != nullptr);
		
		for (u64 materialIndex = 0; materialIndex < materialsCount; ++materialIndex) {
		
			DEBUG_RENDER if (materials == nullptr) {
				spdlog::error ("Screen has no materials assigned!");
				exit (1);
			}
		
			const auto& materialMeshesCount = *MATERIAL::MESHTABLE::GetMeshCount (materialMeshTable, materialIndex);
			auto& material = materials[materialIndex];	
		
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

			// Get shader uniforms range of data defined in the table.
			const auto&& uniformsRange = uniformsTable + 1 + materialIndex + uniformsTableBytesRead;
			auto&& uniforms = (SHADER::UNIFORM::Uniform*)(uniformsRange + 1);
			const auto& uniformsCount = *(uniformsRange);
		
			for (u64 meshIndex = 0; meshIndex < materialMeshesCount; ++meshIndex) {
				const auto& meshId = *MATERIAL::MESHTABLE::GetMesh (materialMeshTable, materialIndex, meshIndex);
				auto& mesh = meshes[meshId].base;
					
				DEBUG_RENDER if (mesh.vao == 0) {
					spdlog::error ("Screen mesh {0} not properly created!", meshIndex);
					exit (1);
				}
		
				SHADER::UNIFORM::SetsMesh (material.program, uniformsCount, uniforms);
		
				glBindVertexArray (mesh.vao); // BOUND VAO
				DEBUG_RENDER  GL::GetError (GL::ET::PRE_DRAW_BIND_VAO);
				mesh.drawFunc (GL_TRIANGLES, mesh.verticiesCount);
				glBindVertexArray (0); // UNBOUND VAO
		
			}
			MATERIAL::MESHTABLE::AddRead (materialMeshesCount);
			uniformsTableBytesRead += uniformsCount * SHADER::UNIFORM::UNIFORM_BYTES;
		}
		MATERIAL::MESHTABLE::SetRead (0);

		glEnable (GL_DEPTH_TEST);
	}


	void World ( 
		const SCENE::World& world, 
		const glm::mat4& projection, 
		const glm::mat4& view 
	) {
		const u8 TRANSFORMS_ROOT_OFFSET = 1;

		ZoneScopedN("Render Camera");

		u16 uniformsTableBytesRead = 0;
			
		auto& uniformsTable = world.uniformsTable;
		auto& materialMeshTable = world.materialMeshTable;
		auto& materialsCount = world.materialsCount;
		auto& transforms = world.transforms;
		auto& materials = world.materials;
		auto& meshes = world.meshes;

		u64 transformsCounter = TRANSFORMS_ROOT_OFFSET;

		for (u64 materialIndex = 0; materialIndex < materialsCount; ++materialIndex) {
			ZoneScopedN("Use Shaders");

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

			// Get shader uniforms range of data defined in the table.
			const auto&& uniformsRange = uniformsTable + 1 + materialIndex + uniformsTableBytesRead;
			auto&& uniforms = (SHADER::UNIFORM::Uniform*)(uniformsRange + 1);
			const auto& uniformsCount = *(uniformsRange);

			for (; meshIndex < materialMeshesCount; ++meshIndex) {
				const auto& meshId = *MATERIAL::MESHTABLE::GetMesh (materialMeshTable, materialIndex, meshIndex);
				auto& mesh = meshes[meshId].base;

				DEBUG_RENDER if (mesh.vao == 0) {
					spdlog::error ("World mesh {0} not properly created!", meshIndex);
					exit (1);
				}

				SHADER::UNIFORM::BUFFORS::model = transforms[transformsCounter].global;
				SHADER::UNIFORM::SetsMesh (material.program, uniformsCount, uniforms);

				glBindVertexArray (mesh.vao); // BOUND VAO
				DEBUG_RENDER  GL::GetError (GL::ET::PRE_DRAW_BIND_VAO);
				mesh.drawFunc (GL_TRIANGLES, mesh.verticiesCount);
				glBindVertexArray (0); // UNBOUND VAO
				++transformsCounter;
			} 
			MATERIAL::MESHTABLE::AddRead (meshIndex);
			uniformsTableBytesRead += uniformsCount * SHADER::UNIFORM::UNIFORM_BYTES;
		} 
		MATERIAL::MESHTABLE::SetRead (0);
	}


	void Canvas ( 
		const SCENE::Canvas& canvas, 
		const glm::mat4& projection 
	) {
		const u8 TRANSFORMS_ROOT_OFFSET = 1;

		ZoneScopedN("Render Canvas");

		u16 uniformsTableBytesRead = 0;

		auto& uniformsTable = canvas.uniformsTable;
		auto& program = FONT::faceShader;

		SHADER::UNIFORM::BUFFORS::projection = projection;
		SHADER::Use (program);
		SHADER::UNIFORM::SetsMaterial (program);

		// Get shader uniforms range of data defined in the table.
		const auto&& uniformsRange = uniformsTable + 1;
		auto&& uniforms = (SHADER::UNIFORM::Uniform*)(uniformsRange + 1);
		const auto& uniformsCount = *uniformsRange;

		{
			SHADER::UNIFORM::BUFFORS::color = { 0.5, 0.8f, 0.2f, 1.0f };
			SHADER::UNIFORM::SetsMesh (program, uniformsCount, uniforms);
			FONT::RenderText (19 - (u16)sharedAnimation1.frameCurrent, "This is sample text", 25.0f, 25.0f, 1.0f);
		}
		{
			SHADER::UNIFORM::BUFFORS::color = { 0.3, 0.7f, 0.9f, 1.0f };
			SHADER::UNIFORM::SetsMesh (program, uniformsCount, uniforms);
			FONT::RenderText (19 - (u16)sharedAnimation1.frameCurrent, "(C) LearnOpenGL.com", 540.0f, 570.0f, 0.5f);
		}
	}
	

	void Update ( SCENE::Scene& scene ) {
		ZoneScopedN("Render: UpdateFrame");
		const u64 WORLD_ROOT_ID = 0;
		auto& world = *scene.world;


		const float shift = GLOBAL::timeCurrent * 0.25f;
		SHADER::UNIFORM::BUFFORS::shift = { shift, shift };
		
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