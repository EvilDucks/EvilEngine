#pragma once
#include <glad/glad.h>
#include "tool/debug.hpp"

#ifdef PLATFORM_UNSPECIFIED_TOKEN
#include <GLFW/glfw3.h>
#endif

constexpr auto GetGLSLVersion() {
	#if defined(IMGUI_IMPL_OPENGL_ES2) 	// GLES 2.0 + GLSL 100
		return "#version 100";
	#elif defined(__APPLE__) 			// GLES 3.2 + GLSL 150
		return "#version 150";
	#else 								// GLES 3.0 + GLSL 130
		return "#version 130";
	#endif
}

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace GL {

	enum ET: u16 { // ERROR TYPE
		PRE_DRAW_BIND_VAO = 5000,
		UNIFORM_SET = 6000,
	};

	void GetError (const u16& code) {
		switch (glGetError ()) {
			case GL_NO_ERROR:
				break;
			case GL_INVALID_ENUM:
				spdlog::error ("{0}: GL_INVALID_ENUM", code);
				break;
			case GL_INVALID_VALUE:
				spdlog::error ("{0}: GL_INVALID_VALUE", code);
				break;
			case GL_INVALID_OPERATION:
				spdlog::error ("{0}: GL_INVALID_OPERATION", code);
				break;
			case GL_INVALID_FRAMEBUFFER_OPERATION:
				spdlog::error ("{0}: GL_INVALID_FRAMEBUFFER_OPERATION", code);
				break;
			case GL_OUT_OF_MEMORY:
				spdlog::error ("{0}: GL_OUT_OF_MEMORY", code);
				break;
			case GL_STACK_UNDERFLOW:
				spdlog::error ("{0}: GL_STACK_UNDERFLOW", code);
				break;
			case GL_STACK_OVERFLOW:
				spdlog::error ("{0}: GL_STACK_OVERFLOW", code);
				break;
			default:
				spdlog::error ("{0}: ERORR UNKNOWN!", code);
		}
	}

	void GetSpecification () {
		GLint extensionsCount = 0; 
		glGetIntegerv (GL_NUM_EXTENSIONS, &extensionsCount); 
		spdlog::info ("OPENGL version: {0}, Extensions: {1}", (const char*)glGetString(GL_VERSION), extensionsCount);
		for (GLint i = 0; i < extensionsCount; ++i) {
			const char* extensions = (const char*)glGetStringi(GL_EXTENSIONS, i);
			spdlog::info ("- {0}: {1}", i, extensions);
		}
	}

}
