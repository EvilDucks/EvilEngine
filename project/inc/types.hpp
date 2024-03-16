// Made by Matthew Strumillo 2024-02-21
//

#pragma once

// C - Standard - Integer
#include <cstdint>
#include <array>

using u8  = uint8_t;
using u16 = uint16_t;
using u32 = uint32_t;
using u64 = uint64_t;

using s8  = int8_t;
using s16 = int16_t;
using s32 = int32_t;
using s64 = int64_t;

using r32 = float;
using r64 = double;

using c8  = char;
using c16 = wchar_t;

using any  = void*;

template<class T, const u64 length>
using array = std::array<T, length>;


struct Color3 {
    r32 r, g, b;
};

struct Color4 {
    r32 r, g, b, a;
};