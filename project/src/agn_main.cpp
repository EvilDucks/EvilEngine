// Made by Matthew Strumillo 2024-03-08
//

#include "platform/agn/window.hpp"
#include "render/render.hpp"
#include "hid/inputMappings.hpp"
#include "components/ui/uiMappings.hpp"

// OPENAL
#include "audio/openal.hpp"

// FreeType
#include "render/font.hpp"

// CGLTF inc
#define CGLTF_IMPLEMENTATION // .c
#include <cgltf.h>

// EFFOLKRONIUM_RANDOM
#include <effolkronium/random.hpp>
using Random = effolkronium::random_static;

// IMGUIZMO
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wswitch"
#include <ImGuizmo.h>
#include <ImSequencer.h>
#pragma GCC diagnostic pop

// IMGUI_CONSOLE
#ifdef ERROR
#undef ERROR
#include <imgui_console/imgui_console.h>
#endif

int main() {
    ZoneScoped;
	DEBUG { spdlog::info ("Entered Agnostic-x86_64-Platform execution."); }

    {
    	PROFILER { ZoneScopedN("Create window and initialize inputs"); }
		HID_INPUT::Create(GLOBAL::input);
		INPUT_MANAGER::Create (GLOBAL::inputManager);
		WIN::Create (GLOBAL::mainWindow);
    }

	if (GLOBAL::inputManager) {
		INPUT_MAP::MapInputs(GLOBAL::inputManager);
		INPUT_MAP::RegisterCallbacks(GLOBAL::inputManager);
	}

    if (GLOBAL::uiManager) {
        UI_MAP::RegisterCallbacks(GLOBAL::uiManager);
    }

		
	{ // FREETYPE
		// tutorials :
		// https://freetype.org/freetype2/docs/tutorial/step1.html
		// https://learnopengl.com/In-Practice/Text-Rendering
		// https://www.youtube.com/embed/S0PyZKX4lyI?t=480
        PROFILER { ZoneScopedN("Initialize FREETYPE"); }

		FT_Library freeType;
		FT_Face face;

		u32 errorCode = FT_Init_FreeType( &freeType );

		DEBUG { 
			if ( errorCode == FT_Err_Ok ) spdlog::info ("FreeType initialized successfully");
			else spdlog::error ("FreeType: {}", errorCode);
		}
		
		FONT::Load (face, freeType, RESOURCES::MANAGER::FONT_LATO_R);
		FONT::Create (face);

		// Everything is now in GPU memory and we can free CPU memory.
		FT_Done_Face (face);
		FT_Done_FreeType (freeType);
	}

	DEBUG {
        PROFILER { ZoneScopedN("TEST OpenAL, EFFOLKRONIUM_RANDOM"); }

		// OPENAL
		ALCdevice* device = OpenAL::CreateAudioDevice();
		spdlog::info("OpenAL Device: {}", alcGetString(device, ALC_DEVICE_SPECIFIER));
		OpenAL::DestoryDevice(device);

		// CGLTF
		// ...

		// TINYOBJLOADER
		// ...

		// EFFOLKRONIUM_RANDOM
		//auto random = Random::get(-1, 1);
		//spdlog::info("Random Value: {0}", random);

		// TRACY
		// ...

		// IMGUIZMO
		// ...

		// IMGUI_CONSOLE
		// ...
	};
	
	GLOBAL::timeCurrent = GLOBAL::timeSinceLastFrame = glfwGetTime();
	RENDER::Initialize();

	//DEBUG spdlog::info ("pre renderring queue");
    //TracyGpuContext;
	while (!glfwWindowShouldClose (GLOBAL::mainWindow)) {

		if (GLOBAL::inputManager) {
			INPUT_MANAGER::ProcessInput(GLOBAL::inputManager, GLOBAL::input);
		}
		
        GLOBAL::Collisions( GLOBAL::scene.world->colliders, GLOBAL::scene.world->collidersCount, GLOBAL::players, GLOBAL::playerCount);
        GLOBAL::UICollisions( GLOBAL::scene.canvas->colliders, GLOBAL::scene.canvas->collidersCount, GLOBAL::players, GLOBAL::playerCount);
		
		glfwGetFramebufferSize (
			GLOBAL::mainWindow, 
			&GLOBAL::windowTransform[2], 
			&GLOBAL::windowTransform[3]
		);

		RENDER::Frame ();

        {
            PROFILER { ZoneScopedN("GLFW Poll Events"); }
            glfwPollEvents ();
        }

        //TracyGpuCollect;
        //FrameMark;
	}

    {
        PROFILER { ZoneScopedN("Finishing Execution"); }

        DEBUG { spdlog::info("Finishing execution."); }
        GLOBAL::Destroy();
        WIN::Destroy(GLOBAL::mainWindow);
        DEBUG { spdlog::info("Closing Program."); }
    }
	return 0;
}