#pragma once

#include <array>
#include <cstdint>
#include <functional>
#include <span>

namespace ravenloft::game {

struct PartyState00B8 {
    // Four original 0x248-byte character records beginning at DS:0x20000.
    // This routine touches +0x18E0/+0x18F0/+0x18F1 within each record.
    std::span<std::uint8_t> characters20000;

    // Original one-byte per-character arrays at DS:0xCF5A and DS:0xCF5B.
    std::array<std::uint8_t, 4> flagsCF5A{};
    std::array<std::uint8_t, 4> flagsCF5B{};
};

struct PartyOps00B8 {
    std::function<std::int32_t(std::int32_t)> call1C1DC;
    std::function<void(std::int32_t)> call16524;
    std::function<void(std::int32_t)> call1C124;
    std::function<void(std::int32_t)> call1C160;
    std::function<std::int32_t(const std::uint8_t*, std::int32_t)> call27C70;
};

class PartyRoutine00B8 {
public:
    // Direct reconstruction of CODE.2 0x00B8..0x01F9.
    static void run(PartyState00B8& state,
                    const PartyOps00B8& ops);
};

} // namespace ravenloft::game
