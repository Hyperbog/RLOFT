#pragma once

#include <array>
#include <cstdint>

namespace ravenloft::game {

struct Code1PaletteTreeNode5880 {
    // Original packed order at DS:7FF0: green, red, blue.
    std::array<std::uint8_t, 3> keyGRB{};
    std::uint16_t value = 0;
    std::uint16_t low = 0x0101;
    std::uint16_t high = 0x0101;
};

struct Code1PaletteSearchState5880 {
    static constexpr std::uint16_t kSentinel = 0x0101;

    // 257 real records. Sentinel index 257 falls immediately into the
    // secondary tree's record zero in the original contiguous memory.
    std::array<Code1PaletteTreeNode5880, 257> tree7FF0{};
    std::array<Code1PaletteTreeNode5880, 257> tree88F9{};

    // Original WORD traversal stack at DS:7BF0.
    std::array<std::int16_t, 512> stack7BF0{};

    std::int16_t word980 = 0;
    std::int16_t word982 = 0;
    std::int16_t word984 = 0;
};

class Code1PaletteInsert5880 {
public:
    // Direct reconstruction of CODE.1 0x5880..0x59AB.
    static void insert(Code1PaletteSearchState5880& state,
                       const Code1PaletteTreeNode5880& input,
                       std::int16_t newIndex) noexcept;
};

class Code1PaletteCenter59AC {
public:
    // Direct reconstruction of CODE.1 0x59AC..0x5A33.
    static std::int32_t find(
        const std::array<std::uint8_t, 0x300>& palette) noexcept;
};

class Code1PaletteTreeBuild5A34 {
public:
    // Direct reconstruction of CODE.1 0x5A34..0x5B63.
    static void build(Code1PaletteSearchState5880& state,
                      const std::array<std::uint8_t, 0x300>& palette) noexcept;
};

class Code1PaletteNearest5B64 {
public:
    // Direct reconstruction of CODE.1 0x5B64..0x5D8E.
    // CODE.1 0x5D8F is alignment.
    static std::int32_t find(Code1PaletteSearchState5880& state,
                             std::uint8_t red,
                             std::uint8_t green,
                             std::uint8_t blue,
                             std::int32_t margin) noexcept;
};

} // namespace ravenloft::game
