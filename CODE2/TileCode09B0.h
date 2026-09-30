#pragma once

#include <cstdint>
#include <span>

namespace ravenloft::game {

struct TileCodeState09B0 {
    // Raw storage such that original DS:0x1760 + base is byte offset 0x1760.
    std::span<const std::uint8_t> data;
    std::int16_t height12754 = 0;\n    std::int16_t width12756 = 0;

    // Original DWORD lookup table at DS:0x5FF0, indexed by four times the
    // accumulated low-value-neighbor mask.
    std::span<const std::uint32_t> table5FF0;
};

class TileCode09B0 {
public:
    // Direct reconstruction of CODE.2 0x09B0..0x0B6B.
    static std::int32_t run(const TileCodeState09B0& state,
                            std::int32_t base,
                            std::int32_t row,
                            std::int32_t column) noexcept;
};

} // namespace ravenloft::game
