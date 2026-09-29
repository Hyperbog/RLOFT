#include "PaletteTables4A0C.h"

#include <cstddef>

namespace ravenloft::game {

void PaletteTable4A0C::build(
    PaletteState4A0C& state,
    const PaletteOps4A0C& ops) {
    // 0x4A15.
    ops.call5A34(state.palette64F8);

    // EBX = 0..4, ESI = 0..255.
    for (std::int32_t level = 0; level < 5; ++level) {
        for (std::size_t index = 0; index < 0x100; ++index) {
            const std::size_t paletteOffset = index * 3u;
            const int r = state.palette64F8[paletteOffset];
            const int g = state.palette64F8[paletteOffset + 1u];
            const int b = state.palette64F8[paletteOffset + 2u];

            // 0x4A67..0x4AB9. Signed IDIV-by-four semantics are reproduced
            // by C++ integer division (truncate toward zero).
            const auto blend = [level](int component) {
                return component +
                    (((63 - component) * level + 2) / 4);
            };

            state.table5FF8[
                static_cast<std::size_t>(level) * 0x100u + index] =
                ops.call5B64(
                    static_cast<std::uint8_t>(blend(r)),
                    static_cast<std::uint8_t>(blend(g)),
                    static_cast<std::uint8_t>(blend(b)),
                    0x0F);
        }
    }
}

void IdentityTable4B00::build(
    PaletteState4A0C& state) noexcept {
    for (std::size_t index = 0; index < 0x100; ++index) {
        state.table59F8[index] = static_cast<std::uint8_t>(index);
    }
}

void LuminanceTable4B18::build(
    PaletteState4A0C& state,
    const PaletteOps4A0C& ops) {
    // 0x4B21..0x4BC6: build page zero.
    for (std::size_t index = 0; index < 0x100; ++index) {
        const std::size_t paletteOffset = index * 3u;

        const int r = state.palette64F8[paletteOffset];
        const int g = state.palette64F8[paletteOffset + 1u];
        const int b = state.palette64F8[paletteOffset + 2u];

        const std::uint16_t target =
            static_cast<std::uint16_t>(
                (r * 300) / 1000 +
                (g * 590) / 1000 +
                (b * 110) / 1000);

        std::int32_t paletteIndex = 0;
        while (paletteIndex < 0x1C &&
               static_cast<std::uint32_t>(
                   state.palette64F8[
                       static_cast<std::size_t>(paletteIndex) * 3u]) <= target) {
            ++paletteIndex;
        }

        --paletteIndex;
        state.table5AF8[index] =
            static_cast<std::uint8_t>(paletteIndex);
    }

    // 0x4BCC..0x4C64: derive pages one through four.
    for (std::int32_t level = 1; level < 5; ++level) {
        for (std::size_t index = 0; index < 0x100; ++index) {
            const std::uint8_t baseIndex = state.table5AF8[index];
            const int red =
                state.palette64F8[static_cast<std::size_t>(baseIndex) * 3u];

            const int target =
                red + (((63 - red) * level + 2) / 4);

            std::uint16_t paletteIndex = 0;
            while (paletteIndex < 0x1C &&
                   static_cast<int>(
                       state.palette64F8[
                           static_cast<std::size_t>(paletteIndex) * 3u]) <= target) {
                ++paletteIndex;
            }

            --paletteIndex;
            state.table5AF8[
                static_cast<std::size_t>(level) * 0x100u + index] =
                static_cast<std::uint8_t>(paletteIndex);
        }
    }

    // 0x4C6A.
    ops.callCBD4(state.table5AF8);
}

} // namespace ravenloft::game
