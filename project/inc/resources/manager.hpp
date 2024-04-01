#pragma once
#include "types.hpp"

namespace RESOURCES::MANAGER {

    // SHADERS

	#define D_SHADERS "res/shaders/"
	#define D_SHADERS_SCREEN D_SHADERS "canvas/"
	#define D_SHADERS_WORLD D_SHADERS "world/"

    // svf - Shader Vertex FilePath
    // sff - Shader Fragment FilePath

	const char* svfSimple 		= D_SHADERS_SCREEN "Simple.vert";
	const char* svfColorize 	= D_SHADERS_SCREEN "Colorize.vert";

	const char* sffSimpleOrange = D_SHADERS_SCREEN "SimpleOrange.frag";
	const char* sffSimpleRed 	= D_SHADERS_SCREEN "SimpleRed.frag";
	const char* sffColorize 	= D_SHADERS_SCREEN "Colorize.frag";

	const char* svfWorld 		= D_SHADERS_WORLD "Simple.vert";
	const char* sffWorld 		= D_SHADERS_WORLD "SimpleBlue.frag";


    // JSONS

    #define D_DATA "res/data/"
    
    const char* dataMaterials   = D_DATA "materials.json";

}