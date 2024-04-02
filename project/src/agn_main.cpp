// Made by Matthew Strumillo 2024-03-08
//

#include "platform/agn/window.hpp"
#include "render/render.hpp"
#include "hid/inputMappings.hpp"

// OPENAL
#include "audio/openal.hpp"

// FreeType inc
#include <ft2build.h>
#include FT_FREETYPE_H

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

// TRACY
#define TRACY_ENABLE // once for the whole project
#include <tracy/Tracy.hpp> // in every file ... what???


int main() {

	DEBUG { spdlog::info ("Entered Agnostic-x86_64-Platform execution."); }

    HID_INPUT::Create(GLOBAL::input);
    INPUT_MANAGER::Create (GLOBAL::inputManager);
	WIN::Create (GLOBAL::mainWindow);

    if (GLOBAL::inputManager) {
		INPUT_MAP::MapInputs(GLOBAL::inputManager);
		INPUT_MAP::RegisterCallbacks(GLOBAL::inputManager);
	}

	DEBUG {


		// FREETYPE
    	// https://freetype.org/freetype2/docs/tutorial/step1.html
    	FT_Library freeType;
    	auto error = FT_Init_FreeType( &freeType );

    	if ( error == FT_Err_Ok ) {
    	    spdlog::info("FreeType: {}", error);
    	} else {
    	    spdlog::error("FreeType: {}", error);
    	}

		// OPENAL
		ALCdevice* device = OpenAL::CreateAudioDevice();
		spdlog::info("OpenAL Device: {}", alcGetString(device, ALC_DEVICE_SPECIFIER));
		OpenAL::DestoryDevice(device);

		// CGLTF
		// ...

		// TINYOBJLOADER
		// ...

		// EFFOLKRONIUM_RANDOM
		auto random = Random::get(-1, 1);
		spdlog::info("Random Value: {0}", random);

		// TRACY
		// ...

		// IMGUIZMO
		// ...

		// IMGUI_CONSOLE
		// ...
	};

	while (!glfwWindowShouldClose (GLOBAL::mainWindow)) {
        if (GLOBAL::inputManager) {
            INPUT_MANAGER::ProcessInput(GLOBAL::inputManager);
        }

        glfwPollEvents ();
		
		glfwGetFramebufferSize (
			GLOBAL::mainWindow, 
			&GLOBAL::windowTransform[2], 
			&GLOBAL::windowTransform[3]
		);

        RENDER::Render ();
	}

	DEBUG { spdlog::info ("Finishing execution."); }
	GLOBAL::Destroy();
	WIN::Destroy(GLOBAL::mainWindow);
    INPUT_MANAGER::Destroy(GLOBAL::inputManager);
    HID_INPUT::Destroy(GLOBAL::input);
	DEBUG { spdlog::info ("Closing Program."); }

	return 0;
}