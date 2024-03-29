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