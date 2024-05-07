#pragma once

#include "tool/debug.hpp"
#include "render/gl.hpp"

#include "imgui.h"
#if PLATFORM == PLATFORM_WINDOWS
#include "platform\win\imgui.hpp"
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler (HWND, UINT, WPARAM, LPARAM);
using Window = HWND;
#else
#include "platform\agn\imgui.hpp"
using Window = GLFWwindow*;
#endif

#include "imgui_impl_opengl3.h"

#include <tracy/Tracy.hpp>
#include "ImGuizmo.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "editor.hpp"

glm::mat4 matrix = glm::mat4(1.f);
glm::mat4 view = glm::mat4(1.f);
glm::mat4 projection = glm::mat4(1.f);

namespace IMGUI {

	bool show_demo_window = false;
	bool show_another_window = false;



	void Create ( const Window& window ) {
        ZoneScopedN("IMGUI: Create");

		IMGUI_CHECKVERSION ();
		ImGui::CreateContext ();
		ImGuiIO& io = ImGui::GetIO (); (void)io;
		//io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
		//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
	
		// Setup Dear ImGui style
		ImGui::StyleColorsDark ();
		//ImGui::StyleColorsLight();
	
		// Setup Platform/Renderer
		PlatformInitialize(window);
		//#if PLATFORM == PLATFORM_WINDOWS
		//ImGui_ImplWin32_Init (window);
		//#else
		//ImGui_ImplGlfw_InitForOpenGL (window, true);
		//#endif
		ImGui_ImplOpenGL3_Init ("#version 130"/*GetGLSLVersion ()*/);

		// Load Fonts
		// - If no fonts are loaded, dear imgui will use the default font. You can also load multiple fonts and use ImGui::PushFont()/PopFont() to select them.
		// - AddFontFromFileTTF() will return the ImFont* so you can store it if you need to select the font among multiple.
		// - If the file cannot be loaded, the function will return NULL. Please handle those errors in your application (e.g. use an assertion, or display an error and quit).
		// - The fonts will be rasterized at a given size (w/ oversampling) and stored into a texture when calling ImFontAtlas::Build()/GetTexDataAsXXXX(), which ImGui_ImplXXXX_NewFrame below will call.
		// - Use '#define IMGUI_ENABLE_FREETYPE' in your imconfig file to use Freetype for higher quality font rendering.
		// - Read 'docs/FONTS.md' for more instructions and details.
		// - Remember that in C/C++ if you want to include a backslash \ in a string literal you need to write a double backslash \\ !
		//io.Fonts->AddFontDefault();
		//io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\segoeui.ttf", 18.0f);
		//io.Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf", 16.0f);
		//io.Fonts->AddFontFromFileTTF("../../misc/fonts/Roboto-Medium.ttf", 16.0f);
		//io.Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf", 15.0f);
		//ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf", 18.0f, NULL, io.Fonts->GetGlyphRangesJapanese());
		//IM_ASSERT(font != NULL);
	}

	void Destroy () {
		ImGui_ImplOpenGL3_Shutdown ();
		PlatformShutdown ();
		//#if PLATFORM == PLATFORM_WINDOWS
		//	ImGui_ImplWin32_Shutdown ();
		//#else
		//	ImGui_ImplGlfw_Shutdown ();
		//#endif
		ImGui::DestroyContext ();
	}

	void Render(
	//	Color::Color4& backgroundColor,
	//	Texture::Texture& texture
		ImVec4& backgroundColor
	) {
        ZoneScopedN("IMGUI: Render");
		
		// Start the Dear ImGui frame
		ImGui_ImplOpenGL3_NewFrame();

		PlatformNewFrame ();
		//#if PLATFORM == PLATFORM_WINDOWS
		//	ImGui_ImplWin32_NewFrame();
		//#else
		//	ImGui_ImplGlfw_NewFrame();
		//#endif
		
		ImGui::NewFrame();

        ImGuizmo::BeginFrame();

		// 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
		if (show_demo_window) ImGui::ShowDemoWindow(&show_demo_window);


		//CreateWindowWithImageBackground("WindowTitle1", ImVec2(256, 256), texture, 0);

		{
			// https://github.com/ocornut/imgui/issues/934
			// https://github.com/ocornut/imgui/issues/3756
			
			// ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus | ImGuiWindowFlags_AlwaysAutoResize
			
			//RenderWindowButtonsWindowed(texture, texture, texture);
			
			//RenderSkillBar(texture);
			//ImGui::SetNextWindowSize(ImVec2(400, 200)); //, 0.9, 
			//
			//ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f)); // To remove window padding
			//ImGui::Begin("SkillBar", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize);
			//
			//
			//// Thats interesting ! it conflitcs with window padding. and 1 is for 1 px border thats inside the window.
			//const float borderLength = 1;
			//ImGui::SetCursorPosX(0 + borderLength);
			//ImGui::SetCursorPosY(0 + borderLength);
			//
			////ImGui::PushStyleVar(ImGuiStyleVar_ItemInnerSpacing, ImVec2(0, 0)); // Does not work on these elements !
			//
			//ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(2.0f, 2.0f));				// Buttons border that highlights
			//ImVec2 area = ImVec2(32.0f, 32.0f);                         						// Size of the image we want to make visible
			//ImVec2 uv0 = ImVec2(0.0f, 0.0f);                            						// UV coordinates for lower-left
			//ImVec2 uv1 = ImVec2(32.0f / texture.area.x, 32.0f / texture.area.y);    			// UV coordinates for (32,32) in our texture
			////ImVec4 bg_col = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);             						// Black background
			////ImVec4 tint_col = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);           						// No tint
			////ImGui::Indent( 0.0f );
			////ImGui::Unindent( 4.0f );
			//ImGui::ImageButton((void*)(size)texture.id, area, uv0, uv1);	//
			////ImGui::Unindent( 0.0f );
			//ImGui::SameLine(32 + 2 + 2 + borderLength);
			////ImGui::Unindent( 4.0f );
			//ImGui::ImageButton((void*)(size)texture.id, area, uv0, uv1);	//
			//ImGui::PopStyleVar();
			
			
			//ImGui::Image((void*)(size)texture.id, smallIconArea);
			//ImGui::SameLine();
			//ImGui::Image((void*)(size)texture.id, smallIconArea);
			//ImGui::Image((void*)(size)texture.id, ImVec2(texture.area.x, texture.area.y));
			//ImGui::SameLine();
			//ImGui::Image((void*)(size)texture.id, ImVec2(texture.area.x, texture.area.y));
			
			//ImGui::End();
			//ImGui::PopStyleVar();
		}
		
		{ // 2. Show a simple window that we create ourselves. We use a Begin/End pair to create a named window.
			static float f = 0.0f;
			static int counter = 0;

			ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.

			ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
			ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state
			ImGui::Checkbox("Another Window", &show_another_window);

			ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
			ImGui::ColorEdit4("clear color", (float*)&backgroundColor); // Edit 3 floats representing a color

			if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
				counter++;
			ImGui::SameLine();
			ImGui::Text("counter = %d", counter);

			ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
			ImGui::End();
		}

		// 3. Show another simple window.
		if (show_another_window)
		{
			ImGui::Begin("Another Window", &show_another_window);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
			ImGui::Text("Hello from another window!");
			if (ImGui::Button("Close Me"))
				show_another_window = false;
			ImGui::End();
		}

        EDITOR::EditTransform(matrix, view, projection);
		
		ImGui::Render();
	}
	
	void PostRender() {
		ImGui_ImplOpenGL3_RenderDrawData ( ImGui::GetDrawData () );
	}
	
}