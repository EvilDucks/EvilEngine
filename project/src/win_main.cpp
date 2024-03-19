// Made by Matthew Strumillo 2024-02-21
//

//#define WINDOW_ALPHA
//#define WINDOW_CUSTOM
//#define WINDOW_CUSTOM_WITH_SHADOW

#include "platform/win/console.hpp"
#include "platform/win/window.hpp"
#include "render/render.hpp"

#include "render/shader.hpp"

s32 WinMain (
	HINSTANCE	instance,
	HINSTANCE	previousInstance,
	c8*			arguments,
	s32			windowState
) {
	UNREFERENCED_PARAMETER (previousInstance);
	UNREFERENCED_PARAMETER (arguments);
	UNREFERENCED_PARAMETER (windowState);

	DEBUG { 
		AttachConsole (); // Create a new console host.
		spdlog::info ("Entered Windowns-x86_64-Platform execution."); 
	}

	c16 windowName[] { L"omw" };
	HWND window = nullptr;

	WIN::Create (instance, window, windowName, GLOBAL::windowSize);

	// Create Shader Programs.
	GLuint shaderID;
	SHADER::Create (shaderID, GLOBAL::svfTriangle, GLOBAL::sffTriangle);


	GLOBAL::Initialize();


	//------
	// MESH and MATERIAL data
	//------

	GLuint vao;
	GLuint buffers[2];
	
	auto& vbo = buffers[0];
	auto& ebo = buffers[1];

	//{ // STATIC Square MESH render.
//
	//	auto& verticesSize = MESH::DD::SQUARE::VERTICES_COUNT;
	//	auto& vertices = MESH::DD::SQUARE::VERTICES;
//
	//	auto& indicesSize = MESH::DD::SQUARE::INDICES_COUNT;
	//	auto& indices = MESH::DD::SQUARE::INDICES;
//
	//	MESH::DD::VI::CreateVAO (
	//		vao, buffers,
	//		verticesSize, vertices,
	//		indicesSize, indices
	//	);
//
	//	GLOBAL::sceneTree.programId = shaderID;
	//	GLOBAL::sceneTree.verticesId = vao;
	//	GLOBAL::sceneTree.verticiesCount = indicesSize;
	//}

	{ // STATIC Triangle MESH render.

		auto& verticesSize = MESH::DD::TRIANGLE::VERTICES_COUNT;
		auto& vertices = MESH::DD::TRIANGLE::VERTICES;

		MESH::DD::V::CreateVAO (
			vao, buffers,
			verticesSize, vertices
		);

		GLOBAL::sceneTree.programId = shaderID;
		GLOBAL::sceneTree.verticesId = vao;
		GLOBAL::sceneTree.verticiesCount = verticesSize;
	}

	
	

	//const u8 C_VERTICES = 3;
	//float vertices[] {
	//	-0.5f, -0.5f, 0.0f,
	// 	0.5f, -0.5f, 0.0f,
	// 	0.0f,  0.5f, 0.0f
	//};
	
	//const u8 C_VERTICES = 4;
	//float vertices[] {
	//	0.5f,  0.5f, 0.0f,  // top    right
	//	0.5f, -0.5f, 0.0f,  // bottom right
	//	-0.5f, -0.5f, 0.0f, // bottom left
	//	-0.5f,  0.5f, 0.0f  // top    left 
	//};

	//const u8 C_INDICES = 6;
	//unsigned int indices[] {
	//	0, 1, 3,  // first Triangle
	//	1, 2, 3   // second Triangle
	//};


	//auto& UNIT_SIZE = MESH::UNIT_SIZE;
	//auto& VERTEX = MESH::VERTEX;
//
	//auto& verticesCount = MESH::DD::SQUARE::VERTICES_COUNT;
	//auto& vertices = MESH::DD::SQUARE::VERTICES;
//
	//auto& indicesCount = MESH::DD::SQUARE::INDICES_COUNT;
	//auto& indices = MESH::DD::SQUARE::INDICES;

	//{ // Prepare Render Data.
	//	{
	//		glGenVertexArrays (1, &vao); 
	//		//glGenBuffers (1, &vbo);
	//		//glGenBuffers (1, &ebo);
	//		glGenBuffers (2, buffers);
	//		/* i */ glBindVertexArray (vao);
	//		/* i */ glBindBuffer (GL_ARRAY_BUFFER, vbo);
	//		/* i */ glBufferData (GL_ARRAY_BUFFER, verticesCount * VERTEX * UNIT_SIZE, vertices, GL_STATIC_DRAW);
	//		/* j */ glBindBuffer (GL_ELEMENT_ARRAY_BUFFER, ebo);
	//		/* j */ glBufferData (GL_ELEMENT_ARRAY_BUFFER, indicesCount * UNIT_SIZE, indices, GL_STATIC_DRAW); 
	//	}
	//	// Tell GL how to treat vertices pointer.
	//	const u64 VERTEX_ATTRIBUTE_LOCATION_0 = 0;
	//	// In this line VBO gets connected to VAO based on previous glBindVertexArray call.
	//	/* i */ glVertexAttribPointer (VERTEX_ATTRIBUTE_LOCATION_0, /* vec3 */ 3, GL_FLOAT, GL_FALSE, 3 * UNIT_SIZE, (void*)0);
	//	/* i */ glEnableVertexAttribArray (VERTEX_ATTRIBUTE_LOCATION_0);
	//	// Afert glVertexAttribPointer we can UNBOUND
	//	glBindBuffer (GL_ARRAY_BUFFER, 0);
	//	//glBindBuffer (GL_ELEMENT_ARRAY_BUFFER, 0); NOT THIS ONE!
	//	glBindVertexArray (0);
	//}

	//GLOBAL::sceneTree.programId = shaderID;
	//GLOBAL::sceneTree.verticesId = vao;
	//GLOBAL::sceneTree.verticiesCount = verticesCount; // WITHOUT EBO
	//GLOBAL::sceneTree.verticiesCount = indicesCount; // WITH EBO
	
	MSG msg { 0 }; // Main loop
	while (msg.message != WM_QUIT) {

		if (PeekMessage (&msg, NULL, 0U, 0U, PM_REMOVE)) {
			TranslateMessage (&msg);
			DispatchMessage (&msg);
			continue;
		} 

		RENDER::Render ();
		
	}

	GLOBAL::Destroy();

	glDeleteVertexArrays (1, &vao);
	//glDeleteBuffers(1, &ebo);
	//glDeleteBuffers (1, &vbo);
	glDeleteBuffers (2, buffers);
	glDeleteProgram (shaderID);

	WIN::Destroy (instance, window, windowName);

	return 0;
}