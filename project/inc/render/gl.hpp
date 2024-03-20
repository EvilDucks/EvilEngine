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

//#ifdef PLATFORM_UNSPECIFIED_TOKEN 
//auto SetupGLESVersion() {
//	#if defined(IMGUI_IMPL_OPENGL_ES2) 										// GLES 2.0 + GLSL 100
//		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);						//
//		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);						//
//		glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);				//
//	#elif defined(__APPLE__) 												// GLES 3.2 + GLSL 150
//		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);						//
//		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);						//
//		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  	// 3.2+ only
//		glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            	// Required on Mac
//	#else 																	// GLES 3.0 + GLSL 130
//		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);						//
//		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);						//
//		//glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);	// 3.2+ only
//		//glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);          	// 3.0+ only
//	#endif
//}
//#endif