// Made by Matthew Strumillo 2024-02-21
//

#pragma once

#include "platform.hpp"

#ifdef DEBUG_LEVEL
#if DEBUG_LEVEL > 0
#define DEBUG_TOKEN
#endif
#endif

#define DEBUG if constexpr (DEBUG_LEVEL > 0)

// Release or not we need to include all headers always and compile against it.
#include <spdlog/spdlog.h>

// IMGUI START
#include "imgui.hpp"
// IMGUI END