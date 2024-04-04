#pragma once
#include "types.hpp"

namespace RESOURCES::MANAGER {

    // SHADERS

	#define D_SHADERS "res/shaders/"
	#define D_SHADERS_SCREEN D_SHADERS "canvas/"
	#define D_SHADERS_WORLD D_SHADERS "world/"

    // svf - Shader Vertex FilePath
    // sff - Shader Fragment FilePath

	const char* const svfSimple 	  = D_SHADERS_SCREEN "Simple.vert";
	const char* const svfColorize 	  = D_SHADERS_SCREEN "Colorize.vert";

	const char* const sffSimpleOrange = D_SHADERS_SCREEN "SimpleOrange.frag";
	const char* const sffSimpleRed 	  = D_SHADERS_SCREEN "SimpleRed.frag";
	const char* const sffColorize 	  = D_SHADERS_SCREEN "Colorize.frag";

	const char* const svfWorld 		  = D_SHADERS_WORLD "Simple.vert";
	const char* const sffWorld 		  = D_SHADERS_WORLD "SimpleBlue.frag";


    // JSONS

    #define D_DATA "res/data/"
    
    const char* const dataMaterials   = D_DATA "materials.json";


	// TEXTURES

	#define D_TEXTURES "res/textures/"

	const char* const TEXTURE_BRICK	  = D_TEXTURES "brick.jpg";
}