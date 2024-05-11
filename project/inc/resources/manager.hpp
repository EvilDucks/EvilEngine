#pragma once
#include "types.hpp"
#include "../../dependencies/cgltf/cgltf.h"
#include "../../dependencies/cgltf/cgltf_write.h"
#include "render/mesh.hpp"
#include "model.hpp"
#include <filesystem>

namespace RESOURCES::MANAGER {

	// SHADERS

	#define D_SHADERS "res/shaders/"
	#define D_SHADERS_CANVAS D_SHADERS "canvas/"
	#define D_SHADERS_SCREEN D_SHADERS "screen/"
	#define D_SHADERS_WORLD D_SHADERS "world/"
	#define D_SHADERS_SKYBOX D_SHADERS "world/cubemap/"

	// svf - Shader Vertex FilePath
	// sff - Shader Fragment FilePath

	const char SVF_S_TEXTURE[]			= D_SHADERS_SCREEN "STexture.vert";
	const char SFF_S_TEXTURE[]			= D_SHADERS_SCREEN "STexture.frag";
	const char SFF_M_TEXTURE[]			= D_SHADERS_SCREEN "MTexture.frag";
	const char SVF_ATLAS_TEXTURE[]			= D_SHADERS_SCREEN "AtlasTexture.vert";
	const char SFF_ATLAS_TEXTURE[]			= D_SHADERS_SCREEN "AtlasTexture.frag";
	const char SVF_ARRAY_TEXTURE[]			= D_SHADERS_SCREEN "ArrayTexture.vert";
	const char SFF_ARRAY_TEXTURE[]			= D_SHADERS_SCREEN "ArrayTexture.frag";
	const char svfSimple[]				= D_SHADERS_SCREEN "Simple.vert";
	const char svfColorize[]			= D_SHADERS_SCREEN "Colorize.vert";
	const char sffSimpleOrange[]		= D_SHADERS_SCREEN "SimpleOrange.frag";
	const char sffSimpleRed[]			= D_SHADERS_SCREEN "SimpleRed.frag";
	const char sffColorize[]			= D_SHADERS_SCREEN "Colorize.frag";


	const char SVF_FONT[]				= D_SHADERS_CANVAS "Font.vert";
	const char SFF_FONT[]				= D_SHADERS_CANVAS "Font.frag";
	const char SVF_SKYBOX[]				= D_SHADERS_SKYBOX "Skybox.vert";
	const char SFF_SKYBOX[]				= D_SHADERS_SKYBOX "Skybox.frag";


	const char svfWorldA[]				= D_SHADERS_WORLD "SpaceOnly.vert";
	const char sffWorldA[]				= D_SHADERS_WORLD "SimpleBlue.frag";
	const char svfWorldTexture[]		= D_SHADERS_WORLD "SimpleTexture.vert";
	const char sffWorldTexture[]		= D_SHADERS_WORLD "SimpleTexture.frag";

	


	// JSONS

	#define D_DATA "res/data/"
	
	const char dataMaterials[]			= D_DATA "materials.json";


	// TEXTURES

	#define D_TEXTURES "res/textures/dummy/"
	#define D_SKYBOXES "res/textures/skyboxes/"

	const char TEXTURE_BRICK[]			= D_TEXTURES "single_brick.jpg";
	const char TEXTURE_TIN_SHEARS[]		= D_TEXTURES "tin_ore.png";
	const char ANIMATED_TEXTURE_1[]		= D_TEXTURES "animated_texture_1.png";
	const char ANIMATED_TEXTURE_2[]		= D_TEXTURES "animated_texture_2.png";

	//const char SKYBOX_DEFAULT_FACE_TOP[]	= D_SKYBOXES "default/top.jpg";
	//const char SKYBOX_DEFAULT_FACE_LFT[]	= D_SKYBOXES "default/left.jpg";
	//const char SKYBOX_DEFAULT_FACE_FRT[]	= D_SKYBOXES "default/front.jpg";
	//const char SKYBOX_DEFAULT_FACE_RGT[]	= D_SKYBOXES "default/right.jpg";
	//const char SKYBOX_DEFAULT_FACE_BCK[]	= D_SKYBOXES "default/back.jpg";
	//const char SKYBOX_DEFAULT_FACE_BOT[]	= D_SKYBOXES "default/bottom.jpg";

	const char* SKYBOX_DEFAULT[6] {
		D_SKYBOXES "default/right.jpg",		// RGT
		D_SKYBOXES "default/left.jpg",		// LFT
		D_SKYBOXES "default/top.jpg",		// TOP
		D_SKYBOXES "default/bottom.jpg",	// BOT
		D_SKYBOXES "default/front.jpg",		// FRT
		D_SKYBOXES "default/back.jpg",		// BCK
	};

	const char* SKYBOX_NIGHT[6] {
		D_SKYBOXES "night/right.png",		// RGT
		D_SKYBOXES "night/left.png",		// LFT
		D_SKYBOXES "night/top.png",			// TOP
		D_SKYBOXES "night/bottom.png",		// BOT
		D_SKYBOXES "night/front.png",		// FRT
		D_SKYBOXES "night/back.png",		// BCK
	};

	// FONTS

	#define D_FONTS		"res/fonts/"

	#define F_BLACK		"Black"
	#define F_BOLD		"Bold"
	#define F_REGULAR	"Regular"
	#define F_LIGHT		"Light"
	#define F_THIN		"Thin"
	#define F_ITALIC	"Italic"

	const char FONT_LATO_C[]	= D_FONTS "lato/Lato-" F_BLACK		".ttf";
	const char FONT_LATO_CI[]	= D_FONTS "lato/Lato-" F_BLACK		F_ITALIC ".ttf";
	const char FONT_LATO_B[]	= D_FONTS "lato/Lato-" F_BOLD		".ttf";
	const char FONT_LATO_BI[]	= D_FONTS "lato/Lato-" F_BOLD		F_ITALIC ".ttf";
	const char FONT_LATO_R[]	= D_FONTS "lato/Lato-" F_REGULAR 	".ttf";
	const char FONT_LATO_RI[]	= D_FONTS "lato/Lato-" F_ITALIC 	".ttf";
	const char FONT_LATO_L[]	= D_FONTS "lato/Lato-" F_LIGHT 		".ttf";
	const char FONT_LATO_LI[]	= D_FONTS "lato/Lato-" F_LIGHT 		F_ITALIC ".ttf";
	const char FONT_LATO_T[]	= D_FONTS "lato/Lato-" F_THIN		".ttf";
	const char FONT_LATO_TI[]	= D_FONTS "lato/Lato-" F_THIN		F_ITALIC ".ttf";

    // MODELS

    void LoadModels(u8& modelsCount, MODEL::Model* models)
    {

        for (auto& p : std::filesystem::directory_iterator("res/models/"))
        {
            std::string fileExtention = p.path().extension().generic_string();

            if (fileExtention == ".gltf")
            {
                std::string fileStr = p.path().generic_string();
                const char *modelPath = fileStr.c_str();
                MODEL::Create(models[0], modelPath);

                {
                    // ! ORDER OF CHILDREN IS IMPORTANT WHEN USING "GetComponentFast" !
                    //  meaning if OBJECT::_A is later in TRANSFORMS then OBJECT::_B
                    //  then OBJECT_B should be first on the list and later OBJECT::_A.
                    auto& model = models[0];


                }

            }
        }
    }

}
