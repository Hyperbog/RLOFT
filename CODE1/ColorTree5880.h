#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <span>

namespace ravenloft::game {

#pragma pack(push, 1)
struct ColorTreeNode5880 {
    std::uint8_t axis0 = 0; // original byte +0: palette G
    std::uint8_t axis1 = 0; // original byte +1: palette R
    std::uint8_t axis2 = 0; // original byte +2: palette B
    std::int16_t payload = -1;
    std::int16_t left = 0x0101;
    std::int16_t right = 0x0101;
};
#pragma pack(pop)
static_assert(sizeof(ColorTreeNode5880) == 9);

struct ColorTreeState5880 {
    static constexpr std::int16_t kSentinel = 0x0101;

    // Original first tree at DS:7FF0. Index 0 is the synthetic root;
    // index 0x101 lands exactly on the sentinel record initialized at DS:88F9.
    std::array<ColorTreeNode5880, 0x102> nodes{};

    // DS:7BF0..7FEF is 0x400 bytes = 0x200 WORD traversal entries.
    std::array<std::int16_t, 0x200> traversalStack{};

    std::int16_t stackDepth980 = 0;
    std::int16_t ready982 = 0;
    std::int16_t visited984 = 0;
};

class ColorTreeInsert5880 {
public:
    // Direct reconstruction of CODE.1 0x5880..0x59AB.
    static void insert(ColorTreeState5880& state,
                       std::uint8_t axis0,
                       std::uint8_t axis1,
                       std::uint8_t axis2,
                       std::int16_t payload,
                       std::int16_t nodeIndex) noexcept;
};

class PaletteMiddle59AC {
public:
    // Direct reconstruction of CODE.1 0x59AC..0x5A33.
    static std::int32_t find(std::span<const std::uint8_t> palette) noexcept;
};

class ColorTreeBuild5A34 {
public:
    // Direct reconstruction of CODE.1 0x5A34..0x5B63.
    static void build(ColorTreeState5880& state,
                      std::span<const std::uint8_t> palette) noexcept;
};

} // namespace ravenloft::game
