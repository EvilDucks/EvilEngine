#pragma once
#include "types.hpp"

namespace RESOURCES::MANAGER {

    // SHADERS

	#define D_SHADERS "res/shaders/"
	#define D_SHADERS_SCREEN D_SHADERS "canvas/"
	#define D_SHADERS_WORLD D_SHADERS "world/"

    // svf - Shader Vertex FilePath
    // sff - Shader Fragment FilePath

	const char svfSimple[] 	  	  = D_SHADERS_SCREEN "Simple.vert";
	const char svfColorize[] 	  = D_SHADERS_SCREEN "Colorize.vert";

	const char sffSimpleOrange[]  = D_SHADERS_SCREEN "SimpleOrange.frag";
	const char sffSimpleRed[] 	  = D_SHADERS_SCREEN "SimpleRed.frag";
	const char sffColorize[] 	  = D_SHADERS_SCREEN "Colorize.frag";

	const char svfWorldA[]		  = D_SHADERS_WORLD "SpaceOnly.vert";
	const char sffWorldA[]		  = D_SHADERS_WORLD "SimpleBlue.frag";

	const char svfWorldTexture[]  = D_SHADERS_WORLD "SimpleTexture.vert";
	const char sffWorldTexture[]  = D_SHADERS_WORLD "SimpleTexture.frag";


    // JSONS

    #define D_DATA "res/data/"
    
    const char* const dataMaterials   = D_DATA "materials.json";


	// TEXTURES

	#define D_TEXTURES "res/textures/"

	const char* const TEXTURE_BRICK	  = D_TEXTURES "brick.jpg";
}