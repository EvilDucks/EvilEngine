// Made by Matthew Strumillo 2024-02-21
//

#pragma once
#include "../types.hpp"

// FOR NOW
//#define DEBUG_LEVEL 3
//#define PLATFORM BUILD_TYPE

#define PLATFORM_WINDOWS	    1
#define PLATFORM_APPLE		    2
#define PLATFORM_LINUX		    3

enum Platform: u8 {
	Unspecified = 0,
	Windows = PLATFORM_WINDOWS,
	Apple = PLATFORM_APPLE,
	Linux = PLATFORM_LINUX,
};

#ifdef PLATFORM

	#define WINDOWS if constexpr (PLATFORM == Platform::Windows)
	#define APPLE if constexpr (PLATFORM == Platform::Apple)
	#define LINUX if constexpr (PLATFORM == Platform::Linux)

	#if   PLATFORM == PLATFORM_WINDOWS
		#define PLATFORM_WINDOWS_TOKEN
	#elif PLATFORM == PLATFORM_APPLE
		#define PLATFORM_APPLE_TOKEN
	#elif PLATFORM == PLATFORM_LINUX
		#define PLATFORM_LINUX_TOKEN
	#else
		#define PLATFORM_UNSPECIFIED_TOKEN
	#endif

#endif