#pragma once

#include "Code1ColorTree5880.h"

#include <cstdint>

namespace ravenloft::game {

class Code1NearestColor5B64 {
public:
    // Direct reconstruction of CODE.1 0x5B64..0x5D8E.
    // CODE.1 0x5D8F is non-executable alignment before the next routine.
    static std::int32_t find(Code1ColorTreeState5880& state,
                             std::uint8_t red,
                             std::uint8_t green,
                             std::uint8_t blue,
                             std::int32_t radius) noexcept;
};

} // namespace ravenloft::game
