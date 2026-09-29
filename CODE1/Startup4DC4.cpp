#include "Startup4DC4.h"

#include <algorithm>
#include <cstddef>
#include <cstdint>

namespace ravenloft::game {
namespace {

void putU32(std::array<std::uint8_t, 0x920>& bytes,
            std::size_t offset,
            std::uint32_t value) noexcept {
    bytes[offset] = static_cast<std::uint8_t>(value);
    bytes[offset + 1u] = static_cast<std::uint8_t>(value >> 8u);
    bytes[offset + 2u] = static_cast<std::uint8_t>(value >> 16u);
    bytes[offset + 3u] = static_cast<std::uint8_t>(value >> 24u);
}

} // namespace

void Startup4DC4::initialize(
    StartupState4DC4& state,
    const StartupOps4DC4& ops) {
    // 0x4DC8..0x4DF9.
    ops.call95BC(0x84, 0x0B);
    ops.call9724(0x88, 1, 0x3C);

    RecordBlock4D54::load(state.resources, ops.resourceOps);
    ResourceOpen4CB0::run(state.resources, ops.resourceOps);

    ops.call64DC(3);

    // 0x4DFC..0x4E56.
    ops.callC254(state.palette.palette64F8, 0, 8, 0x64, 0x100);
    ops.call6618(state.palette.palette64F8);
    ops.call9AC4();

    LuminanceTable4B18::build(state.palette, ops.paletteOps);
    IdentityTable4B00::build(state.palette);
    PaletteTable4A0C::build(state.palette, ops.paletteOps);

    // 0x4E38..0x4E71.
    state.palette.palette64F8.fill(0);
    ops.call6618(state.palette.palette64F8);
    ops.callC254(state.palette.palette64F8, 0, 8, 0x64, 0x100);

    // 0x4E74..0x4EA1.
    state.buffer7138 = 1;
    ops.call6588(state.buffer7138);
    ops.call65A4(static_cast<std::uint8_t>(state.buffer7138 ^ 1u));
    ops.call52AC();

    // 0x4EA6..0x4EE6.
    state.records67F8.fill(0xFF);

    for (std::size_t record = 0; record < 4; ++record) {
        const std::size_t base = record * 0x248u;

        state.records67F8[base + 0x11u] |= 0x80u;

        for (std::size_t field = 0; field < 4; ++field) {
            putU32(state.records67F8,
                   base + 0x10Bu + field * 4u,
                   0u);
        }
    }
}

void CharacterFinalize4EF0::run(
    std::array<std::uint8_t, 0x920>& records67F8,
    const CharacterFinalizeOps4EF0& ops) {
    // 0x4EF5..0x4FB9: finalize the first two 0x248-byte records.
    for (std::size_t record = 0; record < 2; ++record) {
        const std::size_t base = record * 0x248u;

        // Zero record offset 0x10B through the end of the record.
        std::fill(records67F8.begin() + base + 0x10Bu,
                  records67F8.begin() + base + 0x248u,
                  0u);

        records67F8[base + 0x1Bu] = 0x0Au;

        // WORD 6816 -> WORD 6814.
        records67F8[base + 0x1Cu] = records67F8[base + 0x1Eu];
        records67F8[base + 0x1Du] = records67F8[base + 0x1Fu];

        // Copy seven alternating bytes from 682A/2C/.../36 into
        // 682B/2D/.../37.
        for (std::size_t field = 0; field < 7; ++field) {
            records67F8[base + 0x33u + field * 2u] =
                records67F8[base + 0x32u + field * 2u];
        }

        // DS:6929 flags.
        records67F8[base + 0x131u] = 0;

        for (std::size_t slot = 0; slot < 3; ++slot) {
            const std::uint8_t value =
                records67F8[base + 0x15u + slot];

            if (value == 3u) {
                records67F8[base + 0x131u] |= 0x01u;
            }
            if (value == 4u) {
                records67F8[base + 0x131u] |= 0x02u;
            }
        }
    }

    // 0x4FBF..0x500A: original save-file service sequence.
    ops.call63A4(0x94);

    std::int32_t handle = ops.call62D8(0x94);
    ops.call62F0(handle);

    handle = ops.call62C0(0x94);
    ops.call6320(records67F8.data(), 0x920, handle);
    ops.call62F0(handle);
}

} // namespace ravenloft::game
