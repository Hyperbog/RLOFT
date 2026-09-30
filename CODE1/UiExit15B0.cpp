#include "UiExit15B0.h"

#include <array>
#include <bit>
#include <cstddef>
#include <cstdint>

namespace ravenloft::game {
namespace {

std::uint32_t bits(std::int32_t value) noexcept {
    return std::bit_cast<std::uint32_t>(value);
}

std::int32_t signedBits(std::uint32_t value) noexcept {
    return std::bit_cast<std::int32_t>(value);
}

std::int32_t add32(std::int32_t a, std::int32_t b) noexcept {
    return signedBits(bits(a) + bits(b));
}

std::int32_t sub32(std::int32_t a, std::int32_t b) noexcept {
    return signedBits(bits(a) - bits(b));
}

std::int32_t mul32(std::int32_t a, std::int32_t b) noexcept {
    return signedBits(static_cast<std::uint32_t>(
        static_cast<std::uint64_t>(bits(a)) * bits(b)));
}

std::int32_t shl8(std::int32_t value) noexcept {
    return signedBits(bits(value) << 8);
}

std::uint8_t lowByte(std::uint32_t value) noexcept {
    return static_cast<std::uint8_t>(value);
}

void xorLowByteWithOne(std::uint32_t& value) noexcept {
    value = (value & 0xFFFFFF00u) |
            static_cast<std::uint32_t>(lowByte(value) ^ 1u);
}

bool inResourceWindow(std::int32_t value, std::int32_t base) noexcept {
    // 0x1658..0x166F / 0x18AA..0x18C5: signed comparisons around a wrapping
    // 32-bit ADD base,0x2E.  Do not replace with non-wrapping C++ arithmetic.
    return value >= base && value < add32(base, 0x2E);
}

std::int32_t interpolate8(std::int32_t base,
                          std::int32_t target,
                          std::int32_t step) noexcept {
    // 0x193F..0x195E and 0x1960..0x197C. SUB and IMUL wrap to 32 bits,
    // LEA adds four with 32-bit wrap, then SAR/SHL/SBB/SAR implements signed
    // division by eight truncated toward zero.
    const std::int32_t delta = sub32(target, base);
    const std::int32_t product = mul32(delta, step);
    const std::int32_t rounded = add32(product, 4);
    const std::int32_t quotient =
        static_cast<std::int32_t>(static_cast<std::int64_t>(rounded) / 8);
    return add32(base, quotient);
}

struct SavedPage15B0 {
    std::uintptr_t buffer = 0;
    std::int32_t x = 0;
    std::int32_t y = 0;
};

} // namespace

void UiExit15B0::animate(std::int32_t selectedSlot,
                         std::int32_t targetX,
                         std::int32_t targetY,
                         std::int32_t characterPanelStage,
                         UiExitState15B0& state,
                         const UiExitOps15B0& ops) {
    // 0x15B9.
    ops.call56DC();

    // 0x15BE..0x1711: five intensity passes.  0x15C0 selects the page from
    // BYTE DS:7138, while 0x16F8 pushes the complete DWORD to 0x53E4.
    for (std::int32_t intensity = 0; intensity < 5; ++intensity) {
        ops.call6588(lowByte(state.buffer7138));

        for (std::int32_t slot = 0; slot < 9; ++slot) {
            if (slot == selectedSlot) continue;
            const auto i = static_cast<std::size_t>(slot);
            const std::int32_t value = state.current8F8[i];
            if (value == -1) continue;

            const std::int32_t layerOffset = shl8(intensity);
            const std::int32_t surface5FF8 = add32(0x5FF8, layerOffset);

            if (state.mode4D0 == 0) {
                ops.call6824(0, state.x87C[i], state.y880[i],
                             static_cast<std::uintptr_t>(
                                 static_cast<std::uint32_t>(state.resource954[i])),
                             surface5FF8);
                continue;
            }

            if (slot == 7) {
                // 0x1631 reads DS:0958, which aliases resource954[1].
                ops.call6824(0, state.x87C[i], state.y880[i],
                             static_cast<std::uintptr_t>(
                                 static_cast<std::uint32_t>(state.resource954[1])),
                             surface5FF8);
                continue;
            }

            if (inResourceWindow(value, state.global713C)) {
                ops.draw.call6EB0(0, state.x87C[i], state.y880[i],
                                  0x14, 7, surface5FF8);
                const auto overlay = ops.call954C(value, 1);
                ops.call6824(0, add32(state.x87C[i], 0x15), state.y880[i],
                             overlay.pointer, add32(0x5AF8, layerOffset));
                continue;
            }

            ops.draw.call6EB0(0, state.x87C[i], state.y880[i],
                              value, 7, surface5FF8);
        }

        ops.call53E4(state.buffer7138);
        xorLowByteWithOne(state.buffer7138);
    }

    // 0x1717..0x179F: intensity 4 down through 0.  The final pass calls
    // 0x5454; all earlier passes call 0x53E4.  Both receive DWORD DS:7138.
    for (std::int32_t intensity = 4; intensity >= 0; --intensity) {
        ops.call6588(lowByte(state.buffer7138));

        for (std::int32_t slot = 0; slot < 9; ++slot) {
            if (slot == selectedSlot) continue;
            const auto i = static_cast<std::size_t>(slot);
            if (state.current8F8[i] == -1) continue;

            ops.draw.call6EB0(0, state.x87C[i], state.y880[i],
                              slot + 0x1E, 7,
                              add32(0x5FF8, shl8(intensity)));
        }

        if (intensity == 0) ops.call5454(state.buffer7138);
        else ops.call53E4(state.buffer7138);
        xorLowByteWithOne(state.buffer7138);
    }

    // 0x17A1: -1 suppresses the selected-slot flight animation entirely.
    if (selectedSlot == -1) return;

    // 0x17AB..0x184F: two 0x1E80 scratch pages and their saved coordinates.
    std::array<SavedPage15B0, 2> saved{};
    saved[0].buffer = ops.call71C8(0x1E80u);
    saved[1].buffer = ops.call71C8(0x1E80u);

    const auto selected = static_cast<std::size_t>(selectedSlot);
    ops.call6588(lowByte(state.buffer7138));
    ops.draw.call6C60(0, state.x87C[selected], state.y880[selected],
                      add32(selectedSlot, 0x1E), 7);

    const std::int32_t startX = state.x87C[selected];
    const std::int32_t startY = state.y880[selected];
    for (auto& page : saved) {
        page.x = startX;
        page.y = startY;
        ops.call66C0(page.buffer, startX, startY, 0x7A, 0x30);
    }

    std::int32_t currentX = startX;
    std::int32_t currentY = startY;

    // 0x1852..0x1997: nine frames (EDI 0..8).  The local arrays are indexed
    // with the complete DWORD DS:7138 at 0x1868/1881/etc.; valid game state is
    // 0/1, but this is intentionally not reduced to its low byte here.
    for (std::int32_t frame = 0; frame <= 8; ++frame) {
        ops.call6588(lowByte(state.buffer7138));
        auto& page = saved[static_cast<std::size_t>(state.buffer7138)];

        ops.call6744(page.buffer, page.x, page.y, 0x7A, 0x30);
        page.x = currentX;
        page.y = currentY;
        ops.call66C0(page.buffer, currentX, currentY, 0x7A, 0x30);

        const std::int32_t value = state.current8F8[selected];
        if (inResourceWindow(value, state.global713C)) {
            const auto baseResource = ops.call954C(0x14, 7);
            ops.call67C8(0, page.x, page.y, baseResource.pointer);
            const auto overlay = ops.call954C(value, 1);
            ops.call67C8(0, add32(page.x, 0x15), page.y, overlay.pointer);
        } else {
            const auto resource = ops.call954C(value, 7);
            ops.call67C8(0, page.x, page.y, resource.pointer);
        }

        const std::int32_t step = add32(frame, 1);
        currentX = interpolate8(startX, targetX, step);
        currentY = interpolate8(startY, targetY, step);

        ops.call53E4(state.buffer7138);
        xorLowByteWithOne(state.buffer7138);
    }

    // 0x199D..0x19F2: free in allocation order, restore right-side panel,
    // redraw the requested character-panel stage, present, then flip low byte.
    ops.call7240(saved[0].buffer);
    ops.call7240(saved[1].buffer);

    ops.call6588(lowByte(state.buffer7138));
    ops.call86C4(0xC8, 0, 0x19, 7);
    ops.call04B0(characterPanelStage);
    ops.call5454(state.buffer7138);
    xorLowByteWithOne(state.buffer7138);

    // 0x19F9..0x1A00: epilogue/RET.  Next 0x1A01..0x1A03 is alignment LEA.
}

} // namespace ravenloft::game
