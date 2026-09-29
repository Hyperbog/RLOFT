#pragma once

#include <array>
#include <cstdint>
#include <functional>

namespace ravenloft::game {

struct PaletteState4A0C {
    std::array<std::uint8_t, 0x300> palette64F8{};
    std::array<std::uint8_t, 0x500> table5FF8{};
    std::array<std::uint8_t, 0x100> table59F8{};
    std::array<std::uint8_t, 0x500> table5AF8{};
};

struct PaletteOps4A0C {
    std::function<void(std::array<std::uint8_t, 0x300>&)> call5A34;
    std::function<std::uint8_t(std::uint8_t, std::uint8_t,
                               std::uint8_t, std::int32_t)> call5B64;
    std::function<void(const std::array<std::uint8_t, 0x500>&)> callCBD4;
};

class PaletteTable4A0C {
public:
    // Direct reconstruction of CODE.1 0x4A0C..0x4AFF.
    static void build(PaletteState4A0C& state,
                      const PaletteOps4A0C& ops);
};

class IdentityTable4B00 {
public:
    // Direct reconstruction of CODE.1 0x4B00..0x4B16.
    // CODE.1 0x4B17 is alignment.
    static void build(PaletteState4A0C& state) noexcept;
};

class LuminanceTable4B18 {
public:
    // Direct reconstruction of CODE.1 0x4B18..0x4C7E.
    // CODE.1 0x4C7F is alignment.
    static void build(PaletteState4A0C& state,
                      const PaletteOps4A0C& ops);
};

} // namespace ravenloft::game
