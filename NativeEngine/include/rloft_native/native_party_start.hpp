#pragma once
#include <array>
#include <cstddef>
#include <cstdint>
#include <string>

namespace rloft_native {

// Runtime party imported from the existing exact CODE.1 0x920-byte CHARS
// transfer. The input is immutable original-format generated party data,
// not a fabricated demo party or DOS save-file overwrite.
using NativeCharsImage=std::array<std::uint8_t,0x920>;

struct NativePartyActor {
    std::string name;
    std::uint8_t portrait{};
    std::uint8_t sex{};
    std::uint8_t race{};
    std::array<int,3> classes{{-1,-1,-1}};
    std::uint8_t alignment{};
    std::int16_t current_hp{};
    std::int16_t max_hp{};
};

struct NativePartyStart {
    static constexpr unsigned kFirstLevel=22U;
    static constexpr double kColumn=4.5;
    static constexpr double kRow=4.5;
    static constexpr unsigned kHeadingDegrees=270U;

    NativeCharsImage chars{};
    std::array<NativePartyActor,2> actors{};
    unsigned level{kFirstLevel};
    double column{kColumn};
    double row{kRow};
    unsigned heading_degrees{kHeadingDegrees};
};

// Validate source CHARS actor boundaries/identity and retain every byte.
// This is a SOURCE-party import, not save-file or quest-state synthesis.
[[nodiscard]] NativePartyStart decode_native_party_start(
    const NativeCharsImage& chars);

} // namespace rloft_native
