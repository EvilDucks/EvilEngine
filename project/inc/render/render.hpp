#pragma once
#include "systems.hpp"
#include "global.hpp"

namespace RENDER {

	// ANIMATION CONST VARIABLES
	const double frameTime = 1.0f;
	const u8 frameMax = 6;
	const u8 frameMin = 0;
	// ANIMATION VARIABLES
	double passedTime = 0;
	u8 frame = 0;


	void Render ();
	void UpdateFrame ( SCENE::Scene& scene );
	void RenderFrame ( Color4& backgroundColor, SCENE::Scene& scene );
	void RenderText ( GLuint& s, const u16& textCount, const char* const text, float x, float y, float scale, glm::vec3 color);
		
	
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

				const auto& materialMeshesCount = MATERIAL::MESHTABLE::GetMeshCount (materialMeshTable, materialIndex);
				auto& material = materials[materialIndex];

				u64 meshIndex = 0;

				// { Example of Changing Uniform Buffor
				float timeValue = materialIndex + GLOBAL::timeCurrent;
				float greenValue = (sin (timeValue) / 2.0f) + 0.5f;
				GLOBAL::ubColor = { 0.0f, greenValue, 0.0f, 1.0f };
				// }

				DEBUG_RENDER if (material.program.id == 0) {
					spdlog::error ("Screen material {0} not properly created!", materialIndex);
					exit (1);
				}

				SHADER::Use (material.program);
				SHADER::UNIFORM::SetsMaterial (material.program);

				for (; meshIndex < materialMeshesCount; ++meshIndex) {
					const auto& meshId = MATERIAL::MESHTABLE::GetMesh (materialMeshTable, materialIndex, meshIndex);
					auto& mesh = meshes[meshId].base;
					
					DEBUG_RENDER if (mesh.vao == 0) {
						spdlog::error ("Screen mesh {0} not properly created!", meshIndex);
						exit (1);
					}

					switch (materialIndex) { // Should happend via shader !
						case 0: { // WHOLE SCREEN
							glUniform1i ( glGetUniformLocation (material.program.id, "texture1"), 0);
							DEBUG_RENDER GL::GetError (500);
							//glActiveTexture (GL_TEXTURE0);
							glBindTexture (GL_TEXTURE_2D, MESH::texture1);
							//
							const float shift = GLOBAL::timeCurrent * 0.25f;
							glUniform2f ( glGetUniformLocation (material.program.id, "shift"), shift, shift);
							DEBUG_RENDER GL::GetError (501);
						} break;
						case 1: {
							glUniform1i ( glGetUniformLocation (material.program.id, "texture1"), 0);
							DEBUG_RENDER GL::GetError (502);
							//glActiveTexture (GL_TEXTURE0);
							glBindTexture (GL_TEXTURE_2D, MESH::texture2);
						} break;
						case 2: {
							//glActiveTexture (GL_TEXTURE0);
							glUniform1i ( glGetUniformLocation (material.program.id, "sampler1"), 0);
							DEBUG_RENDER GL::GetError (503);
							glBindTexture (GL_TEXTURE_2D_ARRAY, MESH::textureAtlas1);
							DEBUG_RENDER GL::GetError (504);
							glUniform1i ( glGetUniformLocation (material.program.id, "tile"), frame);
							DEBUG_RENDER GL::GetError (505);
							//glUniform1f ( glGetUniformLocation(material.program.id, "x_dir"), x_dir);
							//DEBUG_RENDER GL::GetError (504);
        					//glUniform1f ( glGetUniformLocation(material.program.id, "y_dir"), y_dir);
							//DEBUG_RENDER GL::GetError (505);
        					//glUniform1f ( glGetUniformLocation(material.program.id, "uv_x"), uv_x);
							//DEBUG_RENDER GL::GetError (506);
        					//glUniform1f ( glGetUniformLocation(material.program.id, "uv_y"), uv_y);
							//DEBUG_RENDER GL::GetError (507);
        					//glUniform1f ( glGetUniformLocation(material.program.id, "nx_frames"), nx_frames);
							//DEBUG_RENDER GL::GetError (508);
        					//glUniform1f ( glGetUniformLocation(material.program.id, "ny_frames"), ny_frames);
							//DEBUG_RENDER GL::GetError (509);
						} break;
					}
					//
					SHADER::UNIFORM::SetsMesh (material.program);
					//
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

				const auto& materialMeshesCount = MATERIAL::MESHTABLE::GetMeshCount (materialMeshTable, materialIndex);
				auto& material = materials[materialIndex];

				u64 meshIndex = 0;

				DEBUG if (material.program.id == 0) {
					spdlog::error ("World material {0} not properly created!", materialIndex);
					exit (1);
				}

				SHADER::Use (material.program);
				SHADER::UNIFORM::SetsMaterial (material.program);
				GLOBAL::ubProjection = projection;
				GLOBAL::ubView = view;

				for (; meshIndex < materialMeshesCount; ++meshIndex) {
					const auto& meshId = MATERIAL::MESHTABLE::GetMesh (materialMeshTable, materialIndex, meshIndex);
					auto& mesh = meshes[meshId].base;

					DEBUG_RENDER if (mesh.vao == 0) {
						spdlog::error ("World mesh {0} not properly created!", meshIndex);
						exit (1);
					}

					GLOBAL::ubGlobalSpace = transforms[transformsCounter].global;
					SHADER::UNIFORM::SetsMesh (material.program);

					if (materialIndex == 1) { // should happend via shader !
						glUniform1i ( glGetUniformLocation (material.program.id, "texture1"), 0);
						glActiveTexture (GL_TEXTURE0);
						glBindTexture (GL_TEXTURE_2D, MESH::texture1);
						// some sort of deactivation???
						// loading multiple indexes ???
					}

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


		
		{ // CANVAS !!!!
			glm::mat4 projection = glm::ortho (0.0f, (float)framebufferX, 0.0f, (float)framebufferY);
			auto& shaderId = FONT::faceShader.id;
			//
			glUseProgram (shaderId);
			glUniformMatrix4fv (glGetUniformLocation (shaderId, "projection"), 1, GL_FALSE, glm::value_ptr (projection));
			//
			RenderText (shaderId, 19 - (u16)frame, "This is sample text", 25.0f,   25.0f, 1.0f, glm::vec3 (0.5, 0.8f, 0.2f));
			RenderText (shaderId, 19 - (u16)frame, "(C) LearnOpenGL.com", 540.0f, 570.0f, 0.5f, glm::vec3 (0.3, 0.7f, 0.9f));
		}
		
		
	}

	void UpdateFrame ( SCENE::Scene& scene ) {

		const u64 WORLD_ROOT_ID = 0;
		auto& world = *scene.world;

		
		{ // Recalculating Time Variables.
			GLOBAL::timeCurrent = glfwGetTime();
			GLOBAL::timeDelta = GLOBAL::timeCurrent - GLOBAL::timeSinceLastFrame;
			GLOBAL::timeSinceLastFrame = GLOBAL::timeCurrent;

			// For each timer component?
			passedTime += GLOBAL::timeDelta;
			if (passedTime >= frameTime) {
				passedTime = 0.0f;
				frame += 1;
				if (frame >= frameMax) {
					frame = frameMin;
				}
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



	void RenderText ( 
		GLuint& shaderId,
		const u16& textCount,
		const char* const text,
		float x, float y, 
		float scale,
		glm::vec3 color
	) {
		auto& VAO = FONT::faceVAO;
		auto& VBO = FONT::faceVBO;
		
		glUniform3f ( glGetUniformLocation (shaderId, "textColor"), color.x, color.y, color.z);
    	glActiveTexture (GL_TEXTURE0);
    	glBindVertexArray (VAO);

		for (u16 i = 0; i < textCount; ++i) {
			const char sign = text[i];
			const FONT::Character character = FONT::characters[sign];
			//
			const float xpos = x + character.bearing.x * scale;
			const float ypos = y - (character.size.y - character.bearing.y) * scale;
			//
			const float w = character.size.x * scale;
			const float h = character.size.y * scale;
			//
			const float vertices[6][4] = {
    	        { xpos,     ypos + h,   0.0f, 0.0f },            
    	        { xpos,     ypos,       0.0f, 1.0f },
    	        { xpos + w, ypos,       1.0f, 1.0f },
				//
    	        { xpos,     ypos + h,   0.0f, 0.0f },
    	        { xpos + w, ypos,       1.0f, 1.0f },
    	        { xpos + w, ypos + h,   1.0f, 0.0f },       
    	    };
    	    glBindTexture (GL_TEXTURE_2D, character.textureId);
    	    glBindBuffer (GL_ARRAY_BUFFER, VBO);
    	    glBufferSubData (GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices); 
    	    glBindBuffer (GL_ARRAY_BUFFER, 0);
    	    glDrawArrays (GL_TRIANGLES, 0, 6);
    	    x += (character.advance >> 6) * scale;
		}
		glBindVertexArray(0);
    	glBindTexture(GL_TEXTURE_2D, 0);
	}

}