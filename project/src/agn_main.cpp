// Made by Matthew Strumillo 2024-03-08
//

#include "platform/agn/window.hpp"
#include "render/render.hpp"
#include "hid/inputMappings.hpp"

// OPENAL
#include "audio/openal.hpp"

// FreeType inc
#include "ft2build.h"
#include FT_FREETYPE_H

// CGLTF inc
#define CGLTF_IMPLEMENTATION // .c
#include "cgltf.h"

// TinyObjLoader inc
#define TINYOBJLOADER_IMPLEMENTATION // .c
#include "tiny_obj_loader.h"

int main() {

	DEBUG { spdlog::info ("Entered Agnostic-x86_64-Platform execution."); }

    HID_INPUT::Create(GLOBAL::input);
    INPUT_MANAGER::Create (GLOBAL::inputManager);
	WIN::Create (GLOBAL::mainWindow);

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

    if (GLOBAL::inputManager) {
        INPUT_MAP::MapInputs(GLOBAL::inputManager);
        INPUT_MAP::RegisterCallbacks(GLOBAL::inputManager);
    }

    // CGLTF
    // TINYOBJLOADER
    
    // JSNO-LIB
    // RAND-LIB

    // TRACY
    // IMGUILIBS
    // other?

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