// Made by Matthew Strumillo 2024-02-21
//

#pragma once
#include "platform/types.hpp"

namespace EDITOR {
	const u8 PLAY_MODE = 0;
	const u8 EDIT_MODE = 1;

	u8 mode = PLAY_MODE;
}

#ifdef DEBUG_LEVEL
	// To debug only certain aspects of the engine 
	//  change the following flags: 0 - don't, 1 - do
	#if DEBUG_LEVEL > 0
		#define DEBUG_TOKEN
		#define DEBUG_ENGINE_VALUE 0
		#define DEBUG_RENDER_VALUE 1
		#define DEBUG_SHADER_VALUE 0
		#define DEBUG_FILE_VALUE 1
	#else
		#define DEBUG_ENGINE_VALUE 0
		#define DEBUG_RENDER_VALUE 0
		#define DEBUG_FILE_VALUE 0
		#define DEBUG_SHADER_VALUE 0
	#endif
#endif


#define DEBUG if constexpr (DEBUG_LEVEL > 0)
#define DEBUG_ENGINE if constexpr (DEBUG_ENGINE_VALUE > 0)
#define DEBUG_RENDER if constexpr (DEBUG_RENDER_VALUE > 0)
#define DEBUG_FILE if constexpr (DEBUG_FILE_VALUE > 0)
#define DEBUG_SHADER if constexpr (DEBUG_SHADER_VALUE > 0)

// ... Not really. but check it eventually.
//#define DEBUG_ELSE(ifCode..., ifBody..., elseBody...) \
//	DEBUG { \
//		if (ifCode) { \
//			ifBody \
//		} else { \
//			elseBody \
//		} \
//	} else { \
//		if (ifCode) { \
//			ifBody \
//		} \
//	}

#define EDITOR_PLAY_MODE_OR_RELEASE_ONLY(...) \
	DEBUG { if (EDITOR::mode == EDITOR::PLAY_MODE) \
		__VA_ARGS__ \
	} else { \
		__VA_ARGS__ \
	}

#define EDITOR_EDIT_MODE_OR_RELEASE_ONLY(...) \
	DEBUG { if (EDITOR::mode == EDITOR::EDIT_MODE) \
		__VA_ARGS__ \
	} else { \
		__VA_ARGS__ \
	}

// Release or not we need to include all headers always and compile against it.
// To disable spdlog warnings only.
#pragma GCC diagnostic push 
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#include <spdlog/spdlog.h>
#include <spdlog/fmt/bin_to_hex.h>
#pragma GCC diagnostic pop

#define ErrorExit(message, ...) { \
	DEBUG spdlog::error (message, __VA_ARGS__); \
	exit (1); \
}
