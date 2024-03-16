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

	GLuint VAO, VBO;

	{ // Prepare Render Data.

		// VBO - Vertex Buffer Objects.
		

		float vertices[] {
    		-0.5f, -0.5f, 0.0f,
     		0.5f, -0.5f, 0.0f,
     		0.0f,  0.5f, 0.0f
		};  

		{
			// Here we store vertex data within memory on the graphics card.
			// GL_STATIC_DRAW -> The position data of the triangle does not 
			//  change, is used a lot, and stays the same.

			glGenVertexArrays(1, &VAO); 
			glGenBuffers (1, &VBO); 
			/* i */ glBindVertexArray(VAO);
			/* i */ glBindBuffer (GL_ARRAY_BUFFER, VBO);
			/* i */ glBufferData (GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
		}

		// Tell GL how to treat vertices pointer.
		const u64 VERTEX_ATTRIBUTE_LOCATION_0 = 0;
		/* i */ glVertexAttribPointer(VERTEX_ATTRIBUTE_LOCATION_0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GL_FLOAT), (void*)0);
		/* i */ glEnableVertexAttribArray(VERTEX_ATTRIBUTE_LOCATION_0);

		// Afert glVertexAttribPointer we can UNBOUND
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);

		GLOBAL::sceneTree.programId = shaderID;
		GLOBAL::sceneTree.verticesId = VAO;
	}
	
    MSG msg { 0 }; // Main loop
    while (msg.message != WM_QUIT) {

		if (PeekMessage (&msg, NULL, 0U, 0U, PM_REMOVE)) {
            TranslateMessage (&msg);
            DispatchMessage (&msg);
            continue;
        } 

		RENDER::Render ();
		
	}

	glDeleteVertexArrays (1, &VAO);
	glDeleteBuffers (1, &VBO);
    glDeleteProgram (shaderID);

	WIN::Destroy (instance, window, windowName);

	return 0;
}