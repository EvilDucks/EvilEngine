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


	//------
	// MESH and MATERIAL data
	//------

	// VBO - Vertex Buffer Objects.
	// EBO - Element Buffer Objects.
	// VAO - Vertex Array Object.
	GLuint vao; //, vbo, ebo;
	GLuint buffers[2];
	auto& vbo = buffers[0];
	auto& ebo = buffers[1];

	const u8 C_VERTEX = 3;
	const u8 C_GL_FLOAT = 4;
	

	//const u8 C_VERTICES = 3;
	//float vertices[] {
	//	-0.5f, -0.5f, 0.0f,
    // 	0.5f, -0.5f, 0.0f,
    // 	0.0f,  0.5f, 0.0f
	//};
	
	const u8 C_VERTICES = 4;
	float vertices[] {
    	0.5f,  0.5f, 0.0f,  // top    right
    	0.5f, -0.5f, 0.0f,  // bottom right
    	-0.5f, -0.5f, 0.0f, // bottom left
    	-0.5f,  0.5f, 0.0f  // top    left 
	};

	const u8 C_INDICES = 6;
	unsigned int indices[] {
        0, 1, 3,  // first Triangle
        1, 2, 3   // second Triangle
    };

	{ // Prepare Render Data.

		{
			// Here we store vertex data within memory on the graphics card.
			// GL_STATIC_DRAW -> The position data of the triangle does not 
			//  change, is used a lot, and stays the same.

			// VAO stores information about many VERTEX_ATTRIBUTE_LOCATIONs where 
			//  each is defined by a different VBO.

			glGenVertexArrays (1, &vao); 
			//glGenBuffers (1, &vbo);
			//glGenBuffers (1, &ebo);
			glGenBuffers(2, buffers);

			/* i */ glBindVertexArray (vao);
			/* i */ glBindBuffer (GL_ARRAY_BUFFER, vbo);
			/* i */ glBufferData (GL_ARRAY_BUFFER, C_VERTICES * C_VERTEX * C_GL_FLOAT, vertices, GL_STATIC_DRAW);
			/* j */ glBindBuffer (GL_ELEMENT_ARRAY_BUFFER, ebo);
			/* j */ glBufferData (GL_ELEMENT_ARRAY_BUFFER, C_INDICES * C_GL_FLOAT, indices, GL_STATIC_DRAW); 
		}

		// Tell GL how to treat vertices pointer.
		const u64 VERTEX_ATTRIBUTE_LOCATION_0 = 0;
		// In this line VBO gets connected to VAO based on previous glBindVertexArray call.
		/* i */ glVertexAttribPointer (VERTEX_ATTRIBUTE_LOCATION_0, /* vec3 */ 3, GL_FLOAT, GL_FALSE, 3 * C_GL_FLOAT, (void*)0);
		/* i */ glEnableVertexAttribArray (VERTEX_ATTRIBUTE_LOCATION_0);

		// Afert glVertexAttribPointer we can UNBOUND
		glBindBuffer (GL_ARRAY_BUFFER, 0);
		//glBindBuffer (GL_ELEMENT_ARRAY_BUFFER, 0); NOT THIS ONE!
		glBindVertexArray (0);

		GLOBAL::sceneTree.programId = shaderID;
		GLOBAL::sceneTree.verticesId = vao;
		//GLOBAL::sceneTree.verticiesCount = C_VERTICES; // WITHOUT EBO
		GLOBAL::sceneTree.verticiesCount = C_INDICES; // WITH EBO
	}

	// So what we can change
	// - We can specify how we want to treat the vertices array.
	// - The way we want to store our verticies.
	// - Create more/less VBO's -> AttribPointers (without EBO).
	// - We can use or not EBO.
	// - Knowing the number of meshes we can create more then 1 vao buffer.

	// What can I do with a VAO that has multiple VBO's?
	// Can I have a VAO with 1 VBO&EBO and 1 VBO only?
	// AttribPointer vs Uniform?
	//  -> Mesh is AttribPointer, Material is Uniform.
	//  -> Uniform value changes each render frame, AttribPointer is set once.


	//
	// struct Mesh {
	//     vao
	//     vbo (always at 0 index)
	//	   ebo (if exists always at 1 index)
	//	   buffors_count (if ebo at 1 or 2)
	//	   buffors (will use glVertexAttribPointer)
	// };
	//
	// void GetMeshBufforsCount() -> (vbo != nullptr) + (ebo != nullptr) + buffors_count;
	// void GetMeshLayoutsCount() -> (vbo != nullptr) + buffors_count;
	//

	//
	// struct Material {
	//     shader
	//     uniforms_count
	//     uniforms
	// }
	//
	// struct uniform to byłoby odwołanie do funckji która zawsze przyjmuje 5 argumentów z tym, że niekiedy
	//  argument n'ty może być nie wykorzystany, ustawiamy wtedy w zawołaniu wartość null lub podobną
	//  problem? -> To dość krytyczne miejsce na pointer_funkcji, dalakie odległości względem storny/stronami
	//  musze dobrze pomyśleć jak od strony silnika będzie wyglądać tworzenie materiałów, a ich późniejsze działanie
	//  bazujące na czytaniu wartości działań wcześniejszych wewnątrz funkcji Render.
	
    MSG msg { 0 }; // Main loop
    while (msg.message != WM_QUIT) {

		if (PeekMessage (&msg, NULL, 0U, 0U, PM_REMOVE)) {
            TranslateMessage (&msg);
            DispatchMessage (&msg);
            continue;
        } 

		RENDER::Render ();
		
	}

	glDeleteVertexArrays (1, &vao);
	//glDeleteBuffers(1, &ebo);
	//glDeleteBuffers (1, &vbo);
	glDeleteBuffers (2, buffers);
    glDeleteProgram (shaderID);

	WIN::Destroy (instance, window, windowName);

	return 0;
}