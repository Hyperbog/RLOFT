#pragma once

#include <array>
#include <cstdint>
#include <functional>

namespace ravenloft::game {

struct CharacterOptionState2198 {
    std::uint8_t field680C = 0;
    std::uint8_t field680D = 0;
    std::uint8_t field680E = 0;
};

struct CharacterOptionOps2198 {
    // Signed byte read from the original DS image. 0x2198 performs unchecked
    // address arithmetic for the 06D8/06FC tables; keeping the read explicit
    // preserves the original behavior even for indices outside the nominal
    // shipped table dimensions instead of inventing C++ bounds checks.
    std::function<std::int8_t(std::uint32_t)> readDsS8;
};

class CharacterOptions2198 {
public:
    // Fidelity reconstruction of CODE.1 0x2198..0x24D0.
    // EAX returns the final EDX value. For mode values other than 0,1,2 the
    // routine reaches 0x24C9 without defining EDX, so inheritedEdx is returned.
    static std::uint32_t build(std::array<std::int32_t, 6>& out,
                               std::int32_t mode,
                               const CharacterOptionState2198& state,
                               std::uint32_t inheritedEdx,
                               const CharacterOptionOps2198& ops) noexcept;
};

} // namespace ravenloft::game
