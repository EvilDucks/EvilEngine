#pragma once
#include "types.hpp"

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
	const char svfPhongLight[]     		= D_SHADERS_WORLD "PhongLight.vert";
	const char sffPhongLight[]		    = D_SHADERS_WORLD "PhongLight.frag";

	


	// JSONS

	#define D_DATA "res/data/"
	#define D_SCENES D_DATA "scenes/"
	#define D_SEGMENTS D_SCENES "segments/"
	
	const char dataMaterials[]			= D_DATA "materials.json";
	const char SEGMENTS[] 				= D_SEGMENTS;

	namespace SCENES {

		const char ALPHA[]				= D_SCENES "alpha.json";
		const char SOME_SPINNING[]		= D_SCENES "someSpinning.json";
		const char TOWER[]              = D_SCENES "tower.json";

		const char* SEGMENTS[] {
			// 5 diff, 4 rot types 

			D_SEGMENTS "t_1_0.json",
			D_SEGMENTS "t_2_0.json",
			D_SEGMENTS "t_3_0.json",
			D_SEGMENTS "t_4_0.json",
			D_SEGMENTS "t_5_0.json",

			D_SEGMENTS "t_1_3.json",
			D_SEGMENTS "t_2_3.json",
			D_SEGMENTS "t_3_3.json",
			D_SEGMENTS "t_4_3.json",
			D_SEGMENTS "t_5_3.json",

			D_SEGMENTS "t_1_2.json",
			D_SEGMENTS "t_2_2.json",
			D_SEGMENTS "t_3_2.json",
			D_SEGMENTS "t_4_2.json",
			D_SEGMENTS "t_5_2.json",

			D_SEGMENTS "t_1_3.json",
			D_SEGMENTS "t_2_3.json",
			D_SEGMENTS "t_3_3.json",
			D_SEGMENTS "t_4_3.json",
			D_SEGMENTS "t_5_3.json",
		};

	}


	// TEXTURES

	#define D_TEXTURES "res/textures/dummy/"
	#define D_SKYBOXES "res/textures/skyboxes/"

	const char TEXTURE_BRICK[]			= D_TEXTURES "single_brick.jpg";
	const char TEXTURE_TIN_SHEARS[]		= D_TEXTURES "tin_ore.png";
	const char ANIMATED_TEXTURE_1[]		= D_TEXTURES "animated_texture_1.png";
	const char ANIMATED_TEXTURE_2[]		= D_TEXTURES "animated_texture_2.png";
    const char SMTH_DIFFUSE_TEXTURE[] = D_TEXTURES "smth_diffuse.png";
    const char SMTH_SPECULAR_TEXTURE[] = D_TEXTURES "smth_specular.png";


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
}
