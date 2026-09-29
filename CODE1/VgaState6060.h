#pragma once

#include <array>
#include <cstdint>
#include <functional>

namespace ravenloft::game {

struct VgaState6060 {
    // Original 0x55-byte live/saved regions at DS:2B28 and DS:136E.
    std::array<std::uint8_t, 0x55> live2B28{};
    std::array<std::uint8_t, 0x55> saved136E{};

    std::uint8_t sequencerData1369 = 0;
    std::uint8_t sequencerIndex136A = 0;
    std::uint8_t graphicsData5136B = 0;
    std::uint8_t graphicsIndex136C = 0;
    std::uint8_t graphicsData4136D = 0;
};

struct VgaOps6060 {
    std::function<std::uint8_t(std::uint16_t)> in8;
    std::function<void(std::uint16_t, std::uint8_t)> out8;
    std::function<void(std::uint16_t, std::uint16_t)> out16;
};

class VgaSave6060 {
public:
    // Direct reconstruction of CODE.1 0x6060..0x60B1.
    // CODE.1 0x60B2..0x60B3 is XCHG EBX,EBX alignment.
    static void run(VgaState6060& state,
                    const VgaOps6060& ops);
};

class VgaRestore60B4 {
public:
    // Direct reconstruction of CODE.1 0x60B4..0x6100.
    // CODE.1 0x6101..0x6103 is XCHG EBX,EBX + NOP alignment.
    static void run(VgaState6060& state,
                    const VgaOps6060& ops);
};

} // namespace ravenloft::game
