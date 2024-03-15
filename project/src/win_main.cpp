// Made by Matthew Strumillo 2024-02-21
//

//#define WINDOW_ALPHA
//#define WINDOW_CUSTOM
//#define WINDOW_CUSTOM_WITH_SHADOW

#include "platform/win/console.hpp"
#include "platform/win/window.hpp"
#include "render/render.hpp"

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

	{ // Prepare Render Data

		// VBO - Vertex Buffer Objects

		float vertices[] = {
    		-0.5f, -0.5f, 0.0f,
     		0.5f, -0.5f, 0.0f,
     		0.0f,  0.5f, 0.0f
		};  

		unsigned int VBO;
		glGenBuffers(1, &VBO); 
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
		// GL_STATIC_DRAW -> The position data of the triangle does not change, is used a lot, and stays the same

		// As of now we stored the vertex data within memory on the graphics card 

		// NOW SHADERS!
		const char *vertexShaderSRC = 
			"#version 330 core\n"
    		"layout (location = 0) in vec3 aPos;\n"
    		"void main() {\n"
    		"   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
    		"}\0";

		unsigned int vertexShader;
		vertexShader = glCreateShader (GL_VERTEX_SHADER);
		glShaderSource (vertexShader, 1, &vertexShaderSRC, NULL);
		glCompileShader (vertexShader);

		DEBUG { // check if compilation was successful

			// compile-time errors
			int  success;
			char infoLog[512];
			glGetShaderiv (vertexShader, GL_COMPILE_STATUS, &success);

			if (!success) {
				glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
				spdlog::error("ERROR::SHADER::VERTEX::COMPILATION_FAILED\n", infoLog);
			} else {
				spdlog::info("SHADER::VERTEX::COMPILATION_SUCCESS");
			}
			
		}

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

	WIN::Destroy (instance, window, windowName);

	return 0;
}