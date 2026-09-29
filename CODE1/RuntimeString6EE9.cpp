#include "RuntimeString6EE9.h"

namespace ravenloft::game {

std::size_t RuntimeString6EE9::length(const char* text) noexcept {
    // 0x6EF5..0x6F00 implements the REPNE SCASB strlen sequence.
    const char* cursor = text;
    while (*cursor != '\0') {
        ++cursor;
    }
    return static_cast<std::size_t>(cursor - text);
}

char* RuntimeString6EE9::copy(char* destination,
                                   const char* source) noexcept {
    // 0x72A9 saves the original destination for the return value. The body
    // copies bytes including the terminating NUL, two at a time when possible.
    char* const result = destination;
    for (;;) {
        const char first = *source++;
        *destination++ = first;
        if (first == '\0') {
            return result;
        }

        const char second = *source++;
        *destination++ = second;
        if (second == '\0') {
            return result;
        }
    }
}

} // namespace ravenloft::game
