#include "Code1RuntimeRng618A.h"

namespace ravenloft::game {

std::uint16_t Code1RuntimeRng618A::next15() noexcept {
    // CODE.1 0x6193..0x61A6. Unsigned 32-bit wraparound is intentional.
    state_ = state_ * 0x41C64E6Du + 0x3039u;
    return static_cast<std::uint16_t>((state_ >> 16u) & 0x7FFFu);
}

void Code1RuntimeRng618A::seed(std::uint32_t state) noexcept {
    // CODE.1 0x61B8..0x61BB.
    state_ = state;
}

} // namespace ravenloft::game
