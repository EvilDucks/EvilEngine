#pragma once
#include "global.hpp"

namespace RENDER {

	void Base ( s32& originX, s32& originY, s32 framebufferX, s32 framebufferY );
    void Clear ( const Color4& backgroundColor );

	void Screen ( const SCENE::SHARED::Screen& sharedScreen, const SCENE::Screen& screen );
	void Canvas ( const SCENE::SHARED::Canvas& sharedCanvas, const SCENE::Canvas& canvas, const glm::mat4& projection );
	void World ( const SCENE::SHARED::World& sharedWorld, const SCENE::World& world, const glm::mat4& projection, const glm::mat4& view, BOUNDINGFRUSTUM::Frustum& frustum);
	void Skybox ( const SCENE::Skybox& skybox, const glm::mat4& projection, const glm::mat4& view );

	void Base (
        s32& originX,
        s32& originY,
		s32 framebufferX,
		s32 framebufferY
	) {
        PROFILER { ZoneScopedN("Render: base"); }
		glViewport (originX, originY, framebufferX, framebufferY);
	}

    void Clear (
            const Color4& backgroundColor
            ) {
        glClearColor (
                backgroundColor.r * backgroundColor.a,
                backgroundColor.g * backgroundColor.a,
                backgroundColor.b * backgroundColor.a,
                backgroundColor.a
        );
        glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    };


	void Screen (
		const SCENE::SHARED::Screen& sharedScreen,
		const SCENE::Screen& screen
	) {
		PROFILER { ZoneScopedN("Render Screen Object"); }

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
			SHADER::UNIFORM::BUFFORS::tile = GLOBAL::sharedAnimation1.frameCurrent;

			// Get shader uniforms range of data defined in the table.
			const auto&& uniformsRange = SIZED_BUFFOR::GetCount (uniformsTable, materialIndex, uniformsTableBytesRead);
			auto&& uniforms = (SHADER::UNIFORM::Uniform*)(uniformsRange + 1);
			const auto& uniformsCount = *(uniformsRange);

            //TracyGpuZone("Draw Screen");
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
		const glm::mat4& view,
        BOUNDINGFRUSTUM::Frustum& frustum

	) {
		PROFILER { ZoneScopedN("Render: World"); }

		u16 uniformsTableBytesRead = 0;
			
		auto& materialMeshTable = world.tables.meshes;
		auto& lTransforms = world.lTransforms;
		auto& gTransforms = world.gTransforms;

		auto& uniformsTable = sharedWorld.tables.uniforms;
		auto& materialsCount = sharedWorld.materialsCount;
		auto& materials = sharedWorld.materials;
		auto& meshes = sharedWorld.meshes;

		u16 transformsCounter = world.transformsOffset;

		// SET LIGHT
		SHADER::UNIFORM::BUFFORS::lightPosition			= GLOBAL::lightPosition; // this can be simplified (remove GLOBAL::lightPosition)!
		SHADER::UNIFORM::BUFFORS::lightConstant 		= 1.0f;
		SHADER::UNIFORM::BUFFORS::lightLinear 			= 0.1f;
		SHADER::UNIFORM::BUFFORS::lightQuadratic 		= 0.1f;
		SHADER::UNIFORM::BUFFORS::lightAmbient			= glm::vec3 (1.0f, 1.0f, 1.0f);
		SHADER::UNIFORM::BUFFORS::lightAmbientIntensity	= 1.0f;
		SHADER::UNIFORM::BUFFORS::lightDiffuse			= glm::vec3 (0.7f, 0.7f, 0.7f);
		SHADER::UNIFORM::BUFFORS::lightDiffuseIntensity	= 1.0f;

		for (u64 materialIndex = 0; materialIndex < materialsCount; ++materialIndex) {
			PROFILER { ZoneScopedN("World RenderLoop"); }

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

            //TracyGpuZone("Draw World");
			for (; meshIndex < materialMeshesCount; ++meshIndex) {
                PROFILER { ZoneScopedN("World Instancing"); }
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
                        frustum, gTransforms + transformsCounter,
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
        PROFILER { ZoneScopedN("Render: Skybox"); }
		glDepthMask (GL_FALSE);

		{
            //TracyGpuZone("Draw Skybox");
			auto& shader = skybox.shader.id;
			//skyboxShader.use(); // attach and set view and projection matrix

			glUseProgram (shader);
			glUniformMatrix4fv ( glGetUniformLocation (shader, "projection") , 1, GL_FALSE, &projection[0][0]);
			glUniformMatrix4fv ( glGetUniformLocation (shader, "view") , 1, GL_FALSE, &view[0][0]);
			//glGetUniformLocation (shader, "skybox");

			auto& mesh = skybox.mesh.base;
			glBindVertexArray (mesh.vao);
			glBindTexture (GL_TEXTURE_CUBE_MAP, skybox.texture);
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

		PROFILER { ZoneScopedN("Render: Canvas"); }

		u16 uniformsTableBytesRead = 0;
		u8 materialIndex = 0;

		auto& framebufferX = GLOBAL::windowTransform[2];
		auto& framebufferY = GLOBAL::windowTransform[3];

		auto& uniformsTable = sharedCanvas.tables.uniforms;
		auto& materialsCount = sharedCanvas.materialsCount;
		auto& materials = sharedCanvas.materials;
		auto& meshes = sharedCanvas.meshes;
		
		//SHADER::UNIFORM::BUFFORS::projection = glm::ortho (0.0f, 1200.0f, 0.0f, 640.0f);
		SHADER::UNIFORM::BUFFORS::projection = projection;

		{ // FONTS MATERIAL
			auto& program = materials[0].program;
			SHADER::Use (program);
			SHADER::UNIFORM::SetsMaterial (program);

			// Get shader uniforms range of data defined in the table.
			const auto&& uniformsRange = uniformsTable + 1 + uniformsTableBytesRead + materialIndex;
			auto&& uniforms = (SHADER::UNIFORM::Uniform*)(uniformsRange + 1);
			const auto& uniformsCount = *uniformsRange;
			auto& mesh = meshes[0].base;

			{
				// TEXT
				const SHADER::UNIFORM::F4 color = { 0.5, 0.8f, 0.2f, 1.0f };
				const u8 textSize = 19;
				const char* text = "This is sample text";

				// RECT
				const r32 positionX = 25.0f;
				const r32 positionY = 25.0f;
				const r32 anchorX = 0.0f;
				const r32 anchorY = 0.0f;
				const r32 scaleX = 1.0f;
				const r32 scaleY = 1.0f;
				// GLOBAL-CALCULATED
				const r32 gPositionX = (framebufferX * anchorX) + positionX;
				const r32 gPositionY = (framebufferY * anchorY) + positionY;

				SHADER::UNIFORM::BUFFORS::color = color;
				SHADER::UNIFORM::SetsMesh (program, uniformsCount, uniforms);

				glBindVertexArray (mesh.vao);
				FONT::RenderText (
					mesh.buffers, 
					textSize - (u8)GLOBAL::sharedAnimation1.frameCurrent, text, 
					gPositionX, gPositionY, scaleX, scaleY
				);
				glBindVertexArray (0);
				
			}
			{
				// TEXT
				const SHADER::UNIFORM::F4 color = { 0.3, 0.7f, 0.9f, 1.0f };
				const u8 textSize = 19;
				const char* text = "(C) LearnOpenGL.com";

				// RECT
				const r32 positionX = -300.0f;
				const r32 positionY = -100.0f;
				const r32 anchorX = 1.0f;
				const r32 anchorY = 1.0f;
				const r32 scaleX = 0.5f;
				const r32 scaleY = 0.5f;
				const r32 sizeX = 100.0f;
				const r32 sizeY = 100.0f;
				// GLOBAL-CALCULATED
				const r32 gPositionX = (framebufferX * anchorX) + positionX;
				const r32 gPositionY = (framebufferY * anchorY) + positionY;
				
				SHADER::UNIFORM::BUFFORS::color = color;
				SHADER::UNIFORM::SetsMesh (program, uniformsCount, uniforms);

				glBindVertexArray (mesh.vao);
				FONT::RenderText (
					mesh.buffers, 
					textSize - (u8)GLOBAL::sharedAnimation1.frameCurrent, text, 
					gPositionX, gPositionY, scaleX, scaleY
				);
				glBindVertexArray (0);
			}
			uniformsTableBytesRead += uniformsCount * SHADER::UNIFORM::UNIFORM_BYTES;
			++materialIndex;
		}

		{ // SPRITE MATERIAL
			auto& material = materials[1];
			auto& program = material.program;
			SHADER::UNIFORM::BUFFORS::sampler1.texture = material.texture;
			SHADER::Use (program);
			SHADER::UNIFORM::SetsMaterial (program);

			// Get shader uniforms range of data defined in the table.
			const auto&& uniformsRange = uniformsTable + 1 + uniformsTableBytesRead + materialIndex;
			auto&& uniforms = (SHADER::UNIFORM::Uniform*)(uniformsRange + 1);
			const auto& uniformsCount = *uniformsRange;
			auto& mesh = meshes[1].base;

			{
				SHADER::UNIFORM::BUFFORS::buttonState = (float)(GLOBAL::canvas.buttons[0].local.state);

				auto& rectangle = canvas.lRectangles[2].base;

				glm::mat4 model = glm::mat4(1.0);
				RECTANGLE::ApplyModel(model, rectangle);
				SHADER::UNIFORM::BUFFORS::model = model;

				SHADER::UNIFORM::SetsMesh (program, uniformsCount, uniforms);

				glBindVertexArray (mesh.vao);
				mesh.drawFunc (GL_TRIANGLES, mesh.verticiesCount, 0);
				glBindVertexArray (0);
			}
			uniformsTableBytesRead += uniformsCount * SHADER::UNIFORM::UNIFORM_BYTES;
			++materialIndex;
		}
	}

}