// Made by Matthew Strumillo 2024-02-21
//

#pragma once

#define PLATFORM_WINDOWS	    1
#define PLATFORM_APPLE		    2
#define PLATFORM_LINUX		    3

enum Platform: byte {
	Unspecified = 0,
	Windows = PLATFORM_WINDOWS,
	Apple = PLATFORM_APPLE,
	Linux = PLATFORM_LINUX,
};

#ifdef PLATFORM

#define WINDOWS if constexpr (PLATFORM == Platform::Windows)
#define APPLE if constexpr (PLATFORM == Platform::Apple)
#define LINUX if constexpr (PLATFORM == Platform::Linux)

#if   PLATFORM == 1
#define PLATFORM_WINDOWS_TOKEN
#elif PLATFORM == 2
#define PLATFORM_APPLE_TOKEN
#elif PLATFORM == 3
#define PLATFORM_LINUX_TOKEN
#else
#define PLATFORM_UNSPECIFIED
#endif

#endif