// Made by Matthew Strumillo 2024-02-21
//

#pragma once

#include "platform/types.hpp"

#ifdef DEBUG_LEVEL
	#if DEBUG_LEVEL > 0
		#define DEBUG_TOKEN
		// To debug only certain aspects of the engine 
		//  change the following flags: 0 - don't, 1 - do
		#define DEBUG_RENDER_VALUE 1
	#else
		#define DEBUG_RENDER_VALUE 0
	#endif
#endif

#define DEBUG if constexpr (DEBUG_LEVEL > 0)
#define DEBUG_RENDER if constexpr (DEBUG_RENDER_VALUE > 0)

// Release or not we need to include all headers always and compile against it.
// To disable spdlog warnings only.
#pragma GCC diagnostic push 
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#include <spdlog/spdlog.h>
#pragma GCC diagnostic pop

// IMGUI START
#include "imgui.hpp"
// IMGUI END