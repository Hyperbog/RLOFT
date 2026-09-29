#pragma once

#include <cstddef>

namespace ravenloft::game {

// Direct reconstructions of CODE.1 C-runtime-compatible string helpers used by
// game logic. They remain explicit so address-level provenance is not lost.
class Code1RuntimeString6EE9 {
public:
    // CODE.1 0x6EE9..0x6F04: strlen.
    static std::size_t length(const char* text) noexcept;

    // CODE.1 0x72A0..0x72C8: strcpy, returning the original destination.
    static char* copy(char* destination, const char* source) noexcept;
};

} // namespace ravenloft::game
