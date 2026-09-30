#include "PartyRoutine00B8.h"

#include <cstddef>

namespace ravenloft::game {
namespace {

constexpr std::size_t kCharacterStride = 0x248u;
constexpr std::size_t kField18E0 = 0x18E0u;
constexpr std::size_t kField18F0 = 0x18F0u;
constexpr std::size_t kField18F1 = 0x18F1u;

std::uint16_t read16(std::span<const std::uint8_t> bytes,
                     std::size_t offset) noexcept {
    return static_cast<std::uint16_t>(bytes[offset]) |
           (static_cast<std::uint16_t>(bytes[offset + 1u]) << 8u);
}

void write16(std::span<std::uint8_t> bytes,
             std::size_t offset,
             std::uint16_t value) noexcept {
    bytes[offset] = static_cast<std::uint8_t>(value & 0xFFu);
    bytes[offset + 1u] = static_cast<std::uint8_t>(value >> 8u);
}

bool upperStateBitsSet(std::span<const std::uint8_t> bytes,
                       std::size_t recordBase) noexcept {
    return (read16(bytes, recordBase + kField18F0) & 0xE000u) != 0u;
}

} // namespace

void PartyRoutine00B8::run(PartyState00B8& state,
                           const PartyOps00B8& ops) {
    // 0x00BB..0x00C8.
    if (ops.call1C1DC(0x3D) == 1) {
        return;
    }

    // 0x00CE..0x011D.
    if (ops.call1C1DC(0x20) != 0) {
        ops.call16524(0x09);
        ops.call1C124(0x3D);
        ops.call1C160(0x3C);

        for (std::size_t i = 0; i < 4u; ++i) {
            state.flagsCF5B[i] = 1u;
            const std::size_t base = i * kCharacterStride;
            const std::uint16_t value = read16(state.characters20000,
                                               base + kField18F0);
            write16(state.characters20000,
                    base + kField18F0,
                    static_cast<std::uint16_t>(value & 0xFEBFu));
        }
        return;
    }

    // 0x011F..0x0169.  The original first seeks a character whose 0x18F0
    // word has neither 0xE000 nor 0x0080 set.
    if (ops.call1C1DC(0x3C) != 0) {
        return;
    }

    std::int32_t selected = 0;
    for (; selected < 4; ++selected) {
        const std::size_t base =
            static_cast<std::size_t>(selected) * kCharacterStride;
        const std::uint16_t value =
            read16(state.characters20000, base + kField18F0);
        if ((value & 0xE000u) == 0u && (value & 0x0080u) == 0u) {
            break;
        }
    }

    // 0x016B..0x01CC. If all four failed the first test, scan for a record
    // with no 0xE000 bits whose +0x18E0 data passes all three original
    // predicate calls (4, 0x10, 0x1C).
    if (selected == 4) {
        selected = 0;
        for (; selected < 4; ++selected) {
            const std::size_t base =
                static_cast<std::size_t>(selected) * kCharacterStride;
            if (upperStateBitsSet(state.characters20000, base)) {
                continue;
            }

            const std::uint8_t* field =
                state.characters20000.data() + base + kField18E0;
            if (ops.call27C70(field, 4) != 0 &&
                ops.call27C70(field, 0x10) != 0 &&
                ops.call27C70(field, 0x1C) != 0) {
                break;
            }
        }
    }

    // 0x01CE..0x01F3. The original can reach this with selected == 4; retain
    // the address arithmetic exactly rather than clamping the index.
    ops.call16524(0x09);
    const std::size_t base =
        static_cast<std::size_t>(selected) * kCharacterStride;
    state.flagsCF5A[static_cast<std::size_t>(selected)] = 1u;
    state.characters20000[base + kField18F1] |= 0x01u;
    ops.call1C124(0x3C);
}

} // namespace ravenloft::game
