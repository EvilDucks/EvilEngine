#pragma once

#include "tool/debug.hpp"
#include "global.hpp"
#include "gl.hpp"

#ifdef PLATFORM_WINDOWS_TOKEN
#include "platform/win/loader.hpp"
#endif

#include "scenes.hpp"


#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace RENDER {

	void Update ( Color4& backgroundColor, SCENES::SceneTree& sceneTree );

	
	void Render () {
		IMGUI::Render (*(ImVec4*)(&GLOBAL::backgroundColor));

		#if PLATFORM == PLATFORM_WINDOWS
			wglMakeCurrent (WIN::LOADER::graphicalContext, WIN::LOADER::openGLRenderContext);
		#else
			glfwMakeContextCurrent(GLOBAL::mainWindow);
		#endif

		Update (GLOBAL::backgroundColor, GLOBAL::sceneTree);
		IMGUI::PostRender ();

		#if PLATFORM == PLATFORM_WINDOWS
			wglMakeCurrent (WIN::LOADER::graphicalContext, WIN::LOADER::openGLRenderContext);
			SwapBuffers (WIN::LOADER::graphicalContext);
		#else
			glfwMakeContextCurrent(GLOBAL::mainWindow);
			glfwSwapBuffers(GLOBAL::mainWindow);
		#endif
	}
	

	void Update (
		Color4& backgroundColor,
		SCENES::SceneTree& sceneTree
	) {

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

		{ // Render Camera Object

			// We dont render for each mesh. We render for each material !
			
			for (u64 i = 0; i < sceneTree.materialsCount; ++i) {
				auto& material = sceneTree.materials[i];

				glUseProgram (material.program);

				for (u64 j = 0; j < material.meshes.length; ++j) {
					auto &mesh = ((MESH::Base*)(material.meshes.data))[j];

					glBindVertexArray(mesh.vao); // BOUND VAO
        			mesh.drawFunc(GL_TRIANGLES, mesh.verticiesCount);
        			glBindVertexArray(0); // UNBOUND VAO

				}
			}
		}

		{ // Render Screen Object

		}

		glm::mat4 view = glm::mat4(1.0f);
		glm::mat4 projection = glm::mat4(1.0f);
		glm::mat4 localSpace;

		view = glm::translate(view, glm::vec3(0.0, 0.0, -3.0));
		projection = glm::perspective (
			glm::radians(45.0f),
			(float)framebufferX / (float)framebufferY,
			0.1f,
			100.0f
		);

		localSpace = glm::translate(localSpace, glm::vec3(1.0, 1.0, 1.0));
		
	}

}