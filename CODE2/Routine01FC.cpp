#include "Routine01FC.h"

#include <cstddef>

namespace ravenloft::game {
namespace {

std::uint16_t read16(std::span<const std::uint8_t> bytes,
                     std::size_t offset) noexcept {
    return static_cast<std::uint16_t>(bytes[offset]) |
           (static_cast<std::uint16_t>(bytes[offset + 1u]) << 8u);
}

} // namespace

std::int32_t Routine01FC::run(RoutineState01FC& state,
                              const RoutineOps01FC& ops) {
    // 0x01FC..0x0208.
    if (ops.call1C1DC(0x46) != 0) {
        return 0;
    }

    // 0x020A..0x0228.
    const std::int32_t character = ops.call14A60(4);
    if (character == -1) {
        state.valueBF44 = 0x4A;
        return 0;
    }

    // 0x0229..0x0240. The original tests bits 12 and 14 of the word at
    // character-record offset 0x18F0.
    const std::size_t record =
        static_cast<std::size_t>(character) * 0x248u;
    if ((read16(state.characters20000, record + 0x18F0u) & 0x5000u) != 0u) {
        return 0;
    }

    // 0x0242..0x026D. Addressing is [value25619 * 0x12 * 2 + 0x1994C].
    const std::int32_t scaled =
        static_cast<std::int32_t>(state.value25619) * 0x12;
    const std::size_t byteOffset =
        static_cast<std::size_t>(scaled * 2);
    state.data1994C[byteOffset] &= 0xC0u;

    ops.call1C124(0x46);
    state.valueBF44 = 0x33;
    return 1;
}

} // namespace ravenloft::game
