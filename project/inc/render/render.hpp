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

		//DEBUG spdlog::info ("mc: {0}", materialsCount);

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

			if (material.type == MATERIAL::TYPE::COLOR_ONLY) {
				const SHADER::UNIFORM::F4 color = { 
					material.packed.color.r, material.packed.color.g, material.packed.color.b, 1.0f 
				};

				SHADER::UNIFORM::BUFFORS::color = color;
			}
			//if(materialIndex == 6 || materialIndex == 3)
            {
                glUniform3f ( glGetUniformLocation (material.program.id, "camPos"), SHADER::UNIFORM::BUFFORS::viewPosition.x, SHADER::UNIFORM::BUFFORS::viewPosition.y, SHADER::UNIFORM::BUFFORS::viewPosition.z);
                DEBUG GL::GetError (1235);
            }

			// Get shader uniforms range of data defined in the table.
			const auto&& uniformsRange = SIZED_BUFFOR::GetCount (uniformsTable, materialIndex, uniformsTableBytesRead);
			auto&& uniforms = (SHADER::UNIFORM::Uniform*)(uniformsRange + 1);
			const auto& uniformsCount = *(uniformsRange);

			//DEBUG spdlog::info ("mmc: {0}", materialMeshesCount);

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

				//spdlog::info ("post instances {0}", instances);
					
				SHADER::UNIFORM::SetsMesh (material.program, uniformsCount, uniforms);

				//DEBUG spdlog::info ("val! {0}", BOUNDINGFRUSTUM::frustumTransfroms[0]);

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
			auto& material = materials[materialIndex];
			auto& program = material.program;

			SHADER::Use (program);
			SHADER::UNIFORM::SetsMaterial (program);

			// Get shader uniforms range of data defined in the table.
			const auto&& uniformsRange = uniformsTable + 1 + uniformsTableBytesRead + materialIndex;
			auto&& uniforms = (SHADER::UNIFORM::Uniform*)(uniformsRange + 1);
			const auto& uniformsCount = *uniformsRange;
			auto& mesh = meshes[0].base;

			{
				if (GLOBAL::world.players[0].local.movement.chargeData.chargeCooldown/GLOBAL::world.players[0].local.movement.chargeData.chargeCooldownDuration > 0)
                {
                    glUniform1f ( glGetUniformLocation (material.program.id, "visibility"), 0.f);
                }
                else
                {
                    glUniform1f ( glGetUniformLocation (material.program.id, "visibility"), 1.f);
                }

                // TEXT
				const SHADER::UNIFORM::F4 color = { 1.f, 1.f, 1.f, 1.f };
				u8 textSize = 27;
				char* text = "Press LSHIFT to\nCHARGE/PUSH";
                if (PLAYER::Gamepad(GLOBAL::world.players[0]))
                {
                    textSize = 23;
                    text = "Press RT to\nCHARGE/PUSH";
                }

				auto& rectangle = canvas.lRectangles[0].base;
				// GLOBAL-CALCULATED
				const r32 gPositionX = (framebufferX * rectangle.anchor.x) + rectangle.position.x;
				const r32 gPositionY = (framebufferY * rectangle.anchor.y) + rectangle.position.y;
				
				SHADER::UNIFORM::BUFFORS::color = color;
                glm::mat4 model = glm::mat4(1.0);
                RECTANGLE::ApplyModel(model, rectangle, framebufferX, framebufferY);
                SHADER::UNIFORM::BUFFORS::model = model;
                FONT::RenderText (
                        mesh.buffers,
                        textSize, text,
                        gPositionX, gPositionY, rectangle.scale.x, rectangle.scale.y,
                        mesh.vao, program, uniformsCount, uniforms
                );
				glBindVertexArray (0);
				
			}
			{
                if (GLOBAL::world.players[1].local.movement.chargeData.chargeCooldown/GLOBAL::world.players[1].local.movement.chargeData.chargeCooldownDuration > 0)
                {
                    glUniform1f ( glGetUniformLocation (material.program.id, "visibility"), 0.f);
                }
                else
                {
                    glUniform1f ( glGetUniformLocation (material.program.id, "visibility"), 1.f);
                }

                // TEXT
				const SHADER::UNIFORM::F4 color = { 1.f, 1.f, 1.f, 1.f };
				u8 textSize = 27;
                char* text = "Press LSHIFT to\nCHARGE/PUSH";
                if (PLAYER::Gamepad(GLOBAL::world.players[1]))
                {
                    textSize = 23;
                    text = "Press RT to\nCHARGE/PUSH";
                }

				auto& rectangle = canvas.lRectangles[1].base;
				// GLOBAL-CALCULATED
				const r32 gPositionX = (framebufferX * rectangle.anchor.x) + rectangle.position.x;
				const r32 gPositionY = (framebufferY * rectangle.anchor.y) + rectangle.position.y;
				
				SHADER::UNIFORM::BUFFORS::color = color;
                glm::mat4 model = glm::mat4(1.0);
                RECTANGLE::ApplyModel(model, rectangle, framebufferX, framebufferY);
                SHADER::UNIFORM::BUFFORS::model = model;
				FONT::RenderText (
					mesh.buffers,
					textSize, text,
					gPositionX, gPositionY, rectangle.scale.x, rectangle.scale.y,
					mesh.vao, program, uniformsCount, uniforms
				);
				glBindVertexArray (0);
			}
            {
                if (GLOBAL::world.players[0].local.powerUp.type == POWER_UP::PowerUpType::NONE)
                {
                    glUniform1f ( glGetUniformLocation (material.program.id, "visibility"), 0.f);
                }
                else
                {
                    glUniform1f ( glGetUniformLocation (material.program.id, "visibility"), 1.f);
                }

                // TEXT
                const SHADER::UNIFORM::F4 color = { 0.f, 0.f, 0.f, 1.f };
                u8 textSize = 30;
                char* text = POWER_UP::PowerUpMassage(GLOBAL::world.players[0].local.powerUp.type, PLAYER::Gamepad(GLOBAL::world.players[0]), textSize);

                auto& rectangle = canvas.lRectangles[10].base;
                // GLOBAL-CALCULATED
                const r32 gPositionX = (framebufferX * rectangle.anchor.x) + rectangle.position.x;
                const r32 gPositionY = (framebufferY * rectangle.anchor.y) + rectangle.position.y;

                SHADER::UNIFORM::BUFFORS::color = color;
                glm::mat4 model = glm::mat4(1.0);
                RECTANGLE::ApplyModel(model, rectangle, framebufferX, framebufferY);
                SHADER::UNIFORM::BUFFORS::model = model;
                FONT::RenderText (
                        mesh.buffers,
                        textSize, text,
                        gPositionX, gPositionY, rectangle.scale.x, rectangle.scale.y,
                        mesh.vao, program, uniformsCount, uniforms
                );
                glBindVertexArray (0);
            }
            {
                if (GLOBAL::world.players[1].local.powerUp.type == POWER_UP::PowerUpType::NONE)
                {
                    glUniform1f ( glGetUniformLocation (material.program.id, "visibility"), 0.f);
                }
                else
                {
                    glUniform1f ( glGetUniformLocation (material.program.id, "visibility"), 1.f);
                }

                // TEXT
                const SHADER::UNIFORM::F4 color = { 0.f, 0.f, 0.f, 1.f };
                u8 textSize = 30;
                char* text = POWER_UP::PowerUpMassage(GLOBAL::world.players[1].local.powerUp.type, PLAYER::Gamepad(GLOBAL::world.players[1]), textSize);

                auto& rectangle = canvas.lRectangles[11].base;
                // GLOBAL-CALCULATED
                const r32 gPositionX = (framebufferX * rectangle.anchor.x) + rectangle.position.x;
                const r32 gPositionY = (framebufferY * rectangle.anchor.y) + rectangle.position.y;

                SHADER::UNIFORM::BUFFORS::color = color;
                glm::mat4 model = glm::mat4(1.0);
                RECTANGLE::ApplyModel(model, rectangle, framebufferX, framebufferY);
                SHADER::UNIFORM::BUFFORS::model = model;
                FONT::RenderText (
                        mesh.buffers,
                        textSize, text,
                        gPositionX, gPositionY, rectangle.scale.x, rectangle.scale.y,
                        mesh.vao, program, uniformsCount, uniforms
                );
                glBindVertexArray (0);
            }
			uniformsTableBytesRead += uniformsCount * SHADER::UNIFORM::UNIFORM_BYTES;
			++materialIndex;
		}



		{ // SPRITE MATERIAL
			auto& material = materials[materialIndex];
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
				RECTANGLE::ApplyModel(model, rectangle, framebufferX, framebufferY);
				SHADER::UNIFORM::BUFFORS::model = model;

				SHADER::UNIFORM::SetsMesh (program, uniformsCount, uniforms);

				glBindVertexArray (mesh.vao);
				//mesh.drawFunc (GL_TRIANGLES, mesh.verticiesCount, 0);
				glBindVertexArray (0);
			}
			uniformsTableBytesRead += uniformsCount * SHADER::UNIFORM::UNIFORM_BYTES;
			++materialIndex;
		}

        { // MINIMAP PLAYER ICON
            auto& material = materials[materialIndex];
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
                auto& rectangle = canvas.lRectangles[4].base;

                glm::mat4 model = glm::mat4(1.0);
                RECTANGLE::ApplyModel(model, rectangle, framebufferX, framebufferY);
                SHADER::UNIFORM::BUFFORS::model = model;

                SHADER::UNIFORM::SetsMesh (program, uniformsCount, uniforms);

                glBindVertexArray (mesh.vao);
                mesh.drawFunc (GL_TRIANGLES, mesh.verticiesCount, 0);
                glBindVertexArray (0);
            }

            SHADER::UNIFORM::BUFFORS::sampler1.texture = material.texture2;

            {
                auto& rectangle = canvas.lRectangles[5].base;

                glm::mat4 model = glm::mat4(1.0);
                RECTANGLE::ApplyModel(model, rectangle, framebufferX, framebufferY);
                SHADER::UNIFORM::BUFFORS::model = model;

                SHADER::UNIFORM::SetsMesh (program, uniformsCount, uniforms);

                glBindVertexArray (mesh.vao);
                mesh.drawFunc (GL_TRIANGLES, mesh.verticiesCount, 0);
                glBindVertexArray (0);
            }

            uniformsTableBytesRead += uniformsCount * SHADER::UNIFORM::UNIFORM_BYTES;
            ++materialIndex;
        }

        { // TOWER MINIMAP MATERIAL
            auto& material = materials[materialIndex];
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
                auto& rectangle = canvas.lRectangles[3].base;

                glm::mat4 model = glm::mat4(1.0);
                RECTANGLE::ApplyModel(model, rectangle, framebufferX, framebufferY);
                SHADER::UNIFORM::BUFFORS::model = model;

                SHADER::UNIFORM::SetsMesh (program, uniformsCount, uniforms);

                glBindVertexArray (mesh.vao);
                mesh.drawFunc (GL_TRIANGLES, mesh.verticiesCount, 0);
                glBindVertexArray (0);
            }
            uniformsTableBytesRead += uniformsCount * SHADER::UNIFORM::UNIFORM_BYTES;
            ++materialIndex;
        }

        { // PLAYER POWER UP ICON
            auto& material = materials[materialIndex];
            auto& program = material.program;

            SHADER::UNIFORM::BUFFORS::sampler1.texture = POWER_UP::PowerUpIcon(GLOBAL::world.players[0].local.powerUp.type, material.texture, material.texture1, material.texture2, material.texture3);
            SHADER::Use (program);
            SHADER::UNIFORM::SetsMaterial (program);

            // Get shader uniforms range of data defined in the table.
            const auto&& uniformsRange = uniformsTable + 1 + uniformsTableBytesRead + materialIndex;
            auto&& uniforms = (SHADER::UNIFORM::Uniform*)(uniformsRange + 1);
            const auto& uniformsCount = *uniformsRange;
            auto& mesh = meshes[1].base;

            {
                auto& rectangle = canvas.lRectangles[6].base;

                glm::mat4 model = glm::mat4(1.0);
                RECTANGLE::ApplyModel(model, rectangle, framebufferX, framebufferY);
                SHADER::UNIFORM::BUFFORS::model = model;

                SHADER::UNIFORM::SetsMesh (program, uniformsCount, uniforms);

                glBindVertexArray (mesh.vao);
                mesh.drawFunc (GL_TRIANGLES, mesh.verticiesCount, 0);
                glBindVertexArray (0);
            }

            SHADER::UNIFORM::BUFFORS::sampler1.texture = POWER_UP::PowerUpIcon(GLOBAL::world.players[1].local.powerUp.type, material.texture, material.texture1, material.texture2, material.texture3);

            {
                auto& rectangle = canvas.lRectangles[7].base;

                glm::mat4 model = glm::mat4(1.0);
                RECTANGLE::ApplyModel(model, rectangle, framebufferX, framebufferY);
                SHADER::UNIFORM::BUFFORS::model = model;

                SHADER::UNIFORM::SetsMesh (program, uniformsCount, uniforms);

                glBindVertexArray (mesh.vao);
                mesh.drawFunc (GL_TRIANGLES, mesh.verticiesCount, 0);
                glBindVertexArray (0);
            }
            uniformsTableBytesRead += uniformsCount * SHADER::UNIFORM::UNIFORM_BYTES;
            ++materialIndex;
        }

        { // PLAYER CHARGE INDICATOR
            auto& material = materials[materialIndex];
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
                glUniform1f ( glGetUniformLocation (material.program.id, "charge"), GLOBAL::world.players[0].local.movement.chargeData.chargeCooldown/GLOBAL::world.players[0].local.movement.chargeData.chargeCooldownDuration);
                auto& rectangle = canvas.lRectangles[8].base;

                glm::mat4 model = glm::mat4(1.0);
                RECTANGLE::ApplyModel(model, rectangle, framebufferX, framebufferY);
                SHADER::UNIFORM::BUFFORS::model = model;

                SHADER::UNIFORM::SetsMesh (program, uniformsCount, uniforms);

                glBindVertexArray (mesh.vao);
                mesh.drawFunc (GL_TRIANGLES, mesh.verticiesCount, 0);
                glBindVertexArray (0);
            }

            {
                glUniform1f ( glGetUniformLocation (material.program.id, "charge"), GLOBAL::world.players[1].local.movement.chargeData.chargeCooldown/GLOBAL::world.players[1].local.movement.chargeData.chargeCooldownDuration);
                SHADER::UNIFORM::BUFFORS::sampler1.texture = material.texture2;
                auto& rectangle = canvas.lRectangles[9].base;

                glm::mat4 model = glm::mat4(1.0);
                RECTANGLE::ApplyModel(model, rectangle, framebufferX, framebufferY);
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