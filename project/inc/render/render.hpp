#pragma once
#include "global.hpp"

#include <tracy/Tracy.hpp>
#include <tracy/TracyOpenGL.hpp>

namespace RENDER {

	ANIMATION::Animation sharedAnimation1 { 1.0f, 6, 0, 0.0f, 0 };

	// Release only because light has no mesh or material
	// HACK!! HERE!!
	//u8 TRANSFORMS_OFFSET = 2;

	void Initialize();
	void Frame ();

	void Update ( SCENE::Scene& scene );
	void Base ( const Color4& backgroundColor, s32& framebufferX, s32& framebufferY );

	void Screen ( const SCENE::SHARED::Screen& sharedScreen, const SCENE::Screen& screen );
	void Canvas ( const SCENE::SHARED::Canvas& sharedCanvas, const SCENE::Canvas& canvas, const glm::mat4& projection );
	void World ( const SCENE::SHARED::World& sharedWorld, const SCENE::World& world, const glm::mat4& projection, const glm::mat4& view );
	void Skybox ( const SCENE::Skybox& skybox, const glm::mat4& projection, const glm::mat4& view );
	




	void Initialize () {
		ZoneScopedN ("Render: InitializeRender");
		glEnable (GL_BLEND);
		glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glEnable (GL_DEPTH_TEST);
		glDepthFunc(GL_LESS);
		//glPolygonMode ( GL_FRONT_AND_BACK, GL_LINE );
		glActiveTexture (GL_TEXTURE0);

		//DEBUG TRANSFORMS_OFFSET = 1;
	}
		
	
	void Frame () {
		ZoneScopedN("Render: Render");

		#if PLATFORM == PLATFORM_WINDOWS
			wglMakeCurrent (WIN::LOADER::graphicalContext, WIN::LOADER::openGLRenderContext);
		#else
			glfwMakeContextCurrent(GLOBAL::mainWindow);
		#endif

		//DEBUG if (GLOBAL::additionalWorld.lTransforms == nullptr) {
		//	spdlog::info ("HMMM!");
		//}

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

		{ 
			ZoneScopedN("Render: Frame");

			Base (GLOBAL::backgroundColor, framebufferX, framebufferY);
			//Screen (sharedScreen, screen);

			// Perspective Camera + Skybox
			view = glm::mat4 ( glm::mat3( GetViewMatrix (world.camera) ) );

			projection = glm::perspective (
				glm::radians(world.camera.local.zoom),
				(float)framebufferX / (float)framebufferY,
				0.1f, 100.0f
			);

			world.camFrustum = world.camFrustum.createFrustumFromCamera(
				world.camera,
				(float)framebufferX / (float)framebufferY,
				glm::radians(world.camera.local.zoom),
				0.1f, 100.0f
			);

			Skybox (skybox, projection, view);
			
			// Perspective Camera - Skybox
			view = GetViewMatrix (world.camera);
			//World (sharedWorld, world, projection, view);

			// SEGMENTS
			for (u8 iSegment = 0; iSegment < GLOBAL::segmentsCount; ++iSegment) { 
				auto& cWorld = segmentWorlds[iSegment];
				World (sharedWorld, cWorld, projection, view);
			}

			DEBUG if (GLOBAL::mode == EDITOR::EDIT_MODE) {
				IMGUI::Render (
					*(ImVec4*)(&GLOBAL::backgroundColor), view, projection, 
					GLOBAL::world.lTransforms, GLOBAL::world.gTransforms, GLOBAL::world.transformsCount
				);
			}

			// Orthographic Camera
			projection = glm::ortho (0.0f, (float)framebufferX, 0.0f, (float)framebufferY);
			//Canvas (canvas, sample);
		}

		DEBUG if (GLOBAL::mode == EDITOR::EDIT_MODE) {
			IMGUI::PostRender ();
		}

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
		const SCENE::SHARED::Screen& sharedScreen,
		const SCENE::Screen& screen
	) {
		ZoneScopedN("Render Screen Object");

		glDisable (GL_DEPTH_TEST);
		u16 uniformsTableBytesRead = 0;

		auto& materialMeshTable = screen.tables.meshes;
		//
		auto& materialsCount = sharedScreen.materialsCount;
		auto& uniformsTable = sharedScreen.tables.uniforms;
		auto& materials = sharedScreen.materials;
		auto& meshes = sharedScreen.meshes;

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
			const auto&& uniformsRange = SIZED_BUFFOR::GetCount (uniformsTable, materialIndex, uniformsTableBytesRead);
			auto&& uniforms = (SHADER::UNIFORM::Uniform*)(uniformsRange + 1);
			const auto& uniformsCount = *(uniformsRange);

            TracyGpuZone("Screen drawFunc");
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
				mesh.drawFunc (GL_TRIANGLES, mesh.verticiesCount, 0);
				glBindVertexArray (0); // UNBOUND VAO
		
			}
			MATERIAL::MESHTABLE::AddRead (materialMeshesCount);
			uniformsTableBytesRead += uniformsCount * SHADER::UNIFORM::UNIFORM_BYTES;
		}
		MATERIAL::MESHTABLE::SetRead (0);

		glEnable (GL_DEPTH_TEST);
	}


	void World ( 
		const SCENE::SHARED::World& sharedWorld, 
		const SCENE::World& world, 
		const glm::mat4& projection, 
		const glm::mat4& view 
	) {

		ZoneScopedN("Render Camera");

		u16 uniformsTableBytesRead = 0;
			
		auto& materialMeshTable = world.tables.meshes;
		auto& lTransforms = world.lTransforms;
		auto& gTransforms = world.gTransforms;

		auto& uniformsTable = sharedWorld.tables.uniforms;
		auto& materialsCount = sharedWorld.materialsCount;
		auto& materials = sharedWorld.materials;
		auto& meshes = sharedWorld.meshes;

		u16 transformsCounter = world.transformsOffset;
		//u64 transformsCounter = TRANSFORMS_OFFSET;

		// SET LIGHT
		SHADER::UNIFORM::BUFFORS::lightPosition			= GLOBAL::lightPosition; // this can be simplified (remove GLOBAL::lightPosition)!
		SHADER::UNIFORM::BUFFORS::lightConstant 		= 1.0f;
		SHADER::UNIFORM::BUFFORS::lightLinear 			= 0.1f;
		SHADER::UNIFORM::BUFFORS::lightQuadratic 		= 0.1f;
		SHADER::UNIFORM::BUFFORS::lightAmbient			= glm::vec3 (1.0f, 1.0f, 1.0f);
		SHADER::UNIFORM::BUFFORS::lightAmbientIntensity	= 1.0f;
		SHADER::UNIFORM::BUFFORS::lightDiffuse			= glm::vec3 (0.7f, 0.7f, 0.7f);
		SHADER::UNIFORM::BUFFORS::lightDiffuseIntensity	= 5.0f;

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
			const auto&& uniformsRange = SIZED_BUFFOR::GetCount (uniformsTable, materialIndex, uniformsTableBytesRead);
			auto&& uniforms = (SHADER::UNIFORM::Uniform*)(uniformsRange + 1);
			const auto& uniformsCount = *(uniformsRange);

            TracyGpuZone("World drawFunc");
			for (; meshIndex < materialMeshesCount; ++meshIndex) {
				const auto& meshId = *MATERIAL::MESHTABLE::GetMesh (materialMeshTable, materialIndex, meshIndex);
				const auto& oInstances = *MATERIAL::MESHTABLE::GetMeshInstancesCount (materialMeshTable, materialIndex, meshIndex);
				/* CPY */ auto instances = oInstances;

				//spdlog::info ("material: {0}, mesh: {1}, instances {2}", materialIndex, meshId, instances);
				auto& mesh = meshes[meshId].base;

				DEBUG_RENDER if (mesh.vao == 0) {
					spdlog::error ("World mesh {0} in material {1} with meshId: {2} not properly created!", 
						meshIndex, materialIndex, meshId
					);
					exit (1);
				}

				BOUNDINGFRUSTUM::IsOnFrustum (
					world.camFrustum, gTransforms + transformsCounter, 
					instances, mesh.boundsRadius
				);
					
				SHADER::UNIFORM::SetsMesh (material.program, uniformsCount, uniforms);

				glBindVertexArray (mesh.vao); // BOUND VAO
				DEBUG_RENDER GL::GetError (GL::ET::PRE_DRAW_BIND_VAO);

				{ // Updating Instances
					auto& inm = mesh.buffers[MESH::INM_BUFFER_INDEX];
					glBindBuffer (GL_ARRAY_BUFFER, inm);
					DEBUG_RENDER GL::GetError (8786);
					glBufferSubData (
						GL_ARRAY_BUFFER,
 						0,
 						instances * sizeof (glm::mat4),
						&BOUNDINGFRUSTUM::frustumTransfroms[0]
					);
					DEBUG_RENDER GL::GetError (8787);
				}
				mesh.drawFunc (GL_TRIANGLES, mesh.verticiesCount, instances);
				glBindVertexArray (0); // UNBOUND VAO

				transformsCounter += oInstances;
			} 
			MATERIAL::MESHTABLE::AddRead (materialMeshesCount * 2);
			uniformsTableBytesRead += uniformsCount * SHADER::UNIFORM::UNIFORM_BYTES;
		} 
		MATERIAL::MESHTABLE::SetRead (0);
	}


	void Skybox ( 
		const SCENE::Skybox& skybox, 
		const glm::mat4& projection, 
		const glm::mat4& view 
	) {
		glDepthMask (GL_FALSE);

		{
			auto& shader = skybox.shader.id;
			//skyboxShader.use(); // attach and set view and projection matrix

			glUseProgram (shader);
			glUniformMatrix4fv ( glGetUniformLocation (shader, "projection") , 1, GL_FALSE, &projection[0][0]);
			glUniformMatrix4fv ( glGetUniformLocation (shader, "view") , 1, GL_FALSE, &view[0][0]);
			//glGetUniformLocation (shader, "skybox");

			auto& mesh = skybox.mesh.base;
			glBindVertexArray (mesh.vao);
			glBindTexture (GL_TEXTURE_CUBE_MAP, skybox.texture);
            TracyGpuZone("Skybox drawFunc");
			mesh.drawFunc (GL_TRIANGLES, mesh.verticiesCount, 0);
			glBindVertexArray (0);
			glBindTexture (GL_TEXTURE_CUBE_MAP, 0);
		}
		

		glDepthMask (GL_TRUE);
	}


	void Canvas ( 
		const SCENE::SHARED::Canvas& sharedCanvas,
		const SCENE::Canvas& canvas, 
		const glm::mat4& projection 
	) {

		ZoneScopedN("Render Canvas");

		u16 uniformsTableBytesRead = 0;

		auto& uniformsTable = sharedCanvas.tables.uniforms;
		auto& program = FONT::faceShader;

		//SHADER::UNIFORM::BUFFORS::projection = glm::ortho (0.0f, 1200.0f, 0.0f, 640.0f);
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

			//spdlog::info ("{0}", uniformsCount);
			DEBUG_RENDER GL::GetError (1236);
		}
		{
			SHADER::UNIFORM::BUFFORS::color = { 0.3, 0.7f, 0.9f, 1.0f };
			SHADER::UNIFORM::SetsMesh (program, uniformsCount, uniforms);
			FONT::RenderText (19 - (u16)sharedAnimation1.frameCurrent, "(C) LearnOpenGL.com", 540.0f, 570.0f, 0.5f);
			DEBUG_RENDER GL::GetError (1236);
		}
	}
	

	void Update ( SCENE::Scene& scene ) {
		ZoneScopedN("Render: UpdateFrame");
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
		// For now we hardcode it... so theres something always ratating
		//  Constant rotation should be a component and that logic should be component based
		//if (world.parenthoodsCount > 1) { 
		//	//assert(world.parenthoodsCount == 2);
		//	//
		//	//auto& transformsCount = world.transformsCount;
		//	auto& transforms = world.lTransforms;
		//	auto& thisParenthood = world.parenthoods[1];	// Get node (child of root)
		//	auto& parent = thisParenthood.id;
		//	auto& child = thisParenthood.base.children[0];	// Get node (child of child)
		//	//auto& transformIndex = SYSTEMS::tempIndex;
		//	//
		//	{ // PARENT
		//		//transformIndex = OBJECT::ID_DEFAULT;
		//		//
		//		//OBJECT::GetComponentFast<TRANSFORM::LTransform> (
		//		//	transformIndex, transformsCount, transforms, parent
		//		//);
		//		//
		//		auto& thisTransfrom = transforms[parent];
		//		thisTransfrom.local.rotation.z += 1; 
		//		thisTransfrom.flags = TRANSFORM::DIRTY;
		//	}
		//	{ // CHILD
		//		//transformIndex = OBJECT::ID_DEFAULT;
		//		//
		//		//OBJECT::GetComponentFast<TRANSFORM::LTransform> (
		//		//	transformIndex, transformsCount, transforms, child
		//		//);
		//		//
		//		auto& thisTransfrom = transforms[child];
		//		thisTransfrom.local.rotation.y += 1; 
		//		thisTransfrom.flags = TRANSFORM::DIRTY;
		//	}
		//}

		TRANSFORM::ApplyDirtyFlag (
			world.parenthoodsCount, world.parenthoods,
			world.transformsCount, world.lTransforms, world.gTransforms
		);

	}

}