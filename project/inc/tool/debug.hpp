// Made by Matthew Strumillo 2024-02-21
//

#pragma once

#include "platform/types.hpp"

#ifdef DEBUG_LEVEL
#if DEBUG_LEVEL > 0
#define DEBUG_TOKEN
#endif
#endif

#define DEBUG if constexpr (DEBUG_LEVEL > 0)

// Release or not we need to include all headers always and compile against it.
// To disable spdlog warnings only.
#pragma GCC diagnostic push 
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#include <spdlog/spdlog.h>
#pragma GCC diagnostic pop

// IMGUI START
#include "imgui.hpp"
// IMGUI END