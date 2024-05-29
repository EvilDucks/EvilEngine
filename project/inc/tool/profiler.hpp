#pragma once

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wformat"
#include <tracy/Tracy.hpp>
#include <tracy/TracyOpenGL.hpp>
#pragma GCC diagnostic pop

#define PROFILER_LEVEL 0

#ifdef PROFILER_LEVEL
    #if PROFILER_LEVEL > 0
        #define PROFILER_TOKEN
    #endif
#endif

#define PROFILER if constexpr (PROFILER_LEVEL > 0)

