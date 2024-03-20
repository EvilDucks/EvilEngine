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

	void Update ( u16* windowSize, Color4& backgroundColor, SCENES::SceneTree& sceneTree );

	void Render () {
		IMGUI::Render (*(ImVec4*)(&GLOBAL::backgroundColor));
		wglMakeCurrent (WIN::LOADER::graphicalContext, WIN::LOADER::openGLRenderContext);

		Update (GLOBAL::windowSize, GLOBAL::backgroundColor, GLOBAL::sceneTree);

		IMGUI::PostRender ();
		wglMakeCurrent (WIN::LOADER::graphicalContext, WIN::LOADER::openGLRenderContext);

		SwapBuffers (WIN::LOADER::graphicalContext);
	}

	void Update (
		u16* windowSize,
		Color4& backgroundColor,
		SCENES::SceneTree& sceneTree
	) {
		glViewport (0, 0, windowSize[0], windowSize[1]); // Display Size got from Resize Command

		glClearColor (
			backgroundColor.r * backgroundColor.a, 
			backgroundColor.g * backgroundColor.a, 
			backgroundColor.b * backgroundColor.a, 
			backgroundColor.a
		);

		glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glm::mat4 view = glm::mat4(1.0f);
		glm::mat4 projection = glm::mat4(1.0f);
		glm::mat4 localSpace;

		view = glm::translate(view, glm::vec3(0.0, 0.0, -3.0));
		projection = glm::perspective (
			glm::radians(45.0f),
			(float)windowSize[0] / (float)windowSize[1],
			0.1f,
			100.0f
		);

		localSpace = glm::translate(localSpace, glm::vec3(1.0, 1.0, 1.0));

        for (u64 i = 0; i < sceneTree.meshesCount; ++i) {
            auto &mesh = sceneTree.meshes[i];
			auto& material = sceneTree.materials[i];

			{ // This will brake !!!
				//GLuint location = glGetUniformLocation(material.program, "view");
				//glUniform3f(location, view.);

				//glGetUniformLocation(material.program, "projection");
				//glGetUniformLocation(material.program, "localSpace");
				glUseProgram(material.program);
			}




            glBindVertexArray(mesh.vao); // BOUND VAO
            mesh.drawFunc(GL_TRIANGLES, mesh.verticiesCount);
            glBindVertexArray(0); // UNBOUND VAO
        }
		
	}

}