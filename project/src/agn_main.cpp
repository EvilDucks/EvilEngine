// Made by Matthew Strumillo 2024-03-08
//

#include "platform/agn/window.hpp"
#include "frame.hpp"
#include "hid/inputMappings.hpp"
#include "components/ui/uiMappings.hpp"
#include "components/collisions/collisionMappings.hpp"

#include "resources/manager.hpp"

// AUDIO -> OPENAL, WAV reader 
#include "audio/openal.hpp"
#include "audio/io.hpp"

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

	DEBUG_ENGINE { spdlog::info ("Entered Agnostic-x86_64-Platform execution."); }

	ALCcontext* audioContext = nullptr;
	ALCdevice* audioDevice = nullptr;

	{
		PROFILER { ZoneScopedN ("Create window and initialize inputs"); }
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

    if (GLOBAL::collisionManager) {
        COLLISION_MAP::RegisterCallbacks(GLOBAL::collisionManager);
    }

	{ // FREETYPE
		// tutorials :
		// https://freetype.org/freetype2/docs/tutorial/step1.html
		// https://learnopengl.com/In-Practice/Text-Rendering
		// https://www.youtube.com/embed/S0PyZKX4lyI?t=480
		PROFILER { ZoneScopedN ("Initialize FREETYPE"); }

		FT_Library freeType;
		FT_Face face;

		u32 errorCode = FT_Init_FreeType( &freeType );

		DEBUG_ENGINE { 
			if ( errorCode == FT_Err_Ok ) spdlog::info ("FreeType initialized successfully");
			else ErrorSilent ("FreeType: {}", errorCode);
		}
		
		FONT::Load (face, freeType, RESOURCES::MANAGER::FONT_LATO_R);
		FONT::CreateTexture (face);

		// Everything is now in GPU memory and we can free CPU memory.
		FT_Done_Face (face);
		FT_Done_FreeType (freeType);
	}

	{ // OPENAL
		PROFILER { ZoneScopedN ("Initialize OpenAL"); }

		/* ! Sound has to be created after listener ! */
		auto& musicSource = GLOBAL::sources[0];
		auto& musicSound = GLOBAL::sounds[0];

		// LISTENER
		AUDIO::DEVICE::Create 			(audioDevice);
		AUDIO::CONTEXT::Create 			(audioDevice, audioContext);
		AUDIO::LISTENER::Create 		(AUDIO::ZERO, AUDIO::ZERO);

		// SOUND
		GLOBAL::CreateSounds ();

		// SOURCE
		GLOBAL::CreateGlobalSources ();

		auto& springTrapActivate = GLOBAL::sources[0];
		GLOBAL::CreateSource (springTrapActivate, AUDIO::ZERO); // for now only later its gonna be 3d positioned same as listener
		
	};
	
	GLOBAL::timeCurrent = GLOBAL::timeSinceLastFrame = glfwGetTime ();
	FRAME::Initialize ();

	while (!glfwWindowShouldClose (GLOBAL::mainWindow)) {

		if (GLOBAL::inputManager) {
			INPUT_MANAGER::ProcessInput (GLOBAL::inputManager, GLOBAL::input);
		}
		
		glfwGetFramebufferSize (
			GLOBAL::mainWindow, 
			&GLOBAL::windowTransform[2], 
			&GLOBAL::windowTransform[3]
		);

		FRAME::Frame ();

		{
			PROFILER { ZoneScopedN("GLFW Poll Events"); }
			glfwPollEvents ();
		}

		//TracyGpuCollect;
		//FrameMark;
	}

	{
		PROFILER { ZoneScopedN("Finishing Execution"); }

		DEBUG_ENGINE { spdlog::info ("Finishing execution."); }
		GLOBAL::Destroy ();

		GLOBAL::DestroySources();
		GLOBAL::DestroySounds();

		// LISTENER
		AUDIO::CONTEXT::Destory (audioContext);
		AUDIO::DEVICE::Destory (audioDevice);

		WIN::Destroy (GLOBAL::mainWindow);
		DEBUG_ENGINE { spdlog::info ("Closing Program."); }
	}
	return 0;
}