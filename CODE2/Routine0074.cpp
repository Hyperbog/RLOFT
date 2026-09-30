#include "Routine0074.h"

#include <cstddef>

namespace ravenloft::game {
namespace {

std::uint16_t read16(std::span<const std::uint8_t> bytes,
                     std::size_t offset) noexcept {
    return static_cast<std::uint16_t>(bytes[offset]) |
           (static_cast<std::uint16_t>(bytes[offset + 1u]) << 8u);
}

} // namespace

void Routine0074::run(RoutineState0074& state,
                      const RoutineOps0074& ops) {
    // 0x0075..0x0086.
    ops.call16524(0x1C);
    ops.call1C124(0x3A);

    // 0x0089..0x00AF. EDX is compared signed against WORD DS:0x258A2.
    for (std::int32_t index = 0; index < state.count258A2; ++index) {
        const std::size_t base = static_cast<std::size_t>(index) * 0x16u;
        if (read16(state.table14D78, base + 0x16u) == 0x0008u) {
            // Original: AND BYTE PTR [base + 0x14D8B], 0xFD.
            state.table14D78[base + 0x13u] &= 0xFDu;
        }
    }

    // 0x00B1.
    ops.call1F1C8();
}

} // namespace ravenloft::game
