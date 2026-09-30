#include "UiEntry10A4.h"

#include <array>
#include <bit>
#include <cstddef>
#include <cstdint>

namespace ravenloft::game {
namespace {

struct SavedRect10A4 {
    std::uintptr_t buffer = 0;
    std::int32_t x = 0;
    std::int32_t y = 0;
    std::int32_t width = 0;
    std::int32_t height = 0;
};

std::uint32_t bits(std::int32_t value) noexcept {
    return std::bit_cast<std::uint32_t>(value);
}

std::int32_t signedBits(std::uint32_t value) noexcept {
    return std::bit_cast<std::int32_t>(value);
}

std::int32_t add32(std::int32_t lhs, std::int32_t rhs) noexcept {
    return signedBits(bits(lhs) + bits(rhs));
}

std::int32_t sub32(std::int32_t lhs, std::int32_t rhs) noexcept {
    return signedBits(bits(lhs) - bits(rhs));
}

std::int32_t mul32(std::int32_t lhs, std::int32_t rhs) noexcept {
    return signedBits(static_cast<std::uint32_t>(
        static_cast<std::uint64_t>(bits(lhs)) * bits(rhs)));
}

std::int32_t idiv12(std::int32_t value) noexcept {
    // The original emits CDQ; IDIV 12.  C++ signed division has the same
    // truncation-toward-zero rule; widening avoids INT_MIN/-1 concerns (the
    // divisor here is positive 12).
    return static_cast<std::int32_t>(static_cast<std::int64_t>(value) / 12);
}

std::int32_t interpolate12(std::int32_t base,
                           std::int32_t target,
                           std::int32_t step) noexcept {
    // 0x1311..0x134C: SUB/IMUL/LEA +6/IDIV 12/ADD, all in 32-bit registers.
    const std::int32_t delta = sub32(target, base);
    const std::int32_t product = mul32(delta, step);
    const std::int32_t rounded = add32(product, 6);
    return add32(base, idiv12(rounded));
}

std::uint8_t lowByte(std::uint32_t value) noexcept {
    return static_cast<std::uint8_t>(value);
}

void xorLowByteWithOne(std::uint32_t& value) noexcept {
    value = (value & 0xFFFFFF00u) |
            static_cast<std::uint32_t>(lowByte(value) ^ 1u);
}

void restore(const SavedRect10A4& r, const UiEntryOps10A4& ops) {
    ops.call6744(r.buffer, r.x, r.y, r.width, r.height);
}

} // namespace

void UiEntry10A4::animate(std::int32_t characterPanelStage,
                          UiEntryState10A4& state,
                          const UiEntryOps10A4& ops) {
    std::array<SavedRect10A4, 2> saved{};

    // 0x10AD..0x10CA: two independent 0x3540-byte scratch allocations.
    saved[0].buffer = ops.call71C8(0x3540u);
    saved[1].buffer = ops.call71C8(0x3540u);

    // 0x10CD..0x1186: prime both display pages.
    for (std::int32_t pass = 0; pass < 2; ++pass) {
        ops.call6588(lowByte(state.buffer7138));

        if (state.mode4D0 == 0) {
            ops.call86C4(0, 0, 0x18, 7);
        }
        ops.call86C4(0xC8, 0, 0x19, 7);

        ops.draw.call6C60(0, 5, 0, 0x2A, 7);
        ops.draw.call7D80(5, 0, 0x140, state.text7140, 1);

        for (std::int32_t slot = 0; slot < 9; ++slot) {
            const auto i = static_cast<std::size_t>(slot);
            ops.draw.call6C60(0, state.x87C[i], state.y880[i],
                              slot + 0x1E, 7);
        }

        if (characterPanelStage != 0) {
            ops.call04B0(characterPanelStage);
        }

        // 0x116D PUSHes the complete DWORD, then 0x117B XORs only its byte.
        ops.call53E4(state.buffer7138);
        xorLowByteWithOne(state.buffer7138);
    }

    // 0x118C..0x11B9: resource-cache warm-up calls; return values discarded.
    (void)ops.call954C(0x15, 7);
    (void)ops.call954C(0x27, 7);
    (void)ops.call954C(0x28, 7);
    (void)ops.call954C(0x29, 7);

    // 0x11BC..0x143C: animate each non-empty slot.
    for (std::int32_t slot = 0; slot < 9; ++slot) {
        const auto i = static_cast<std::size_t>(slot);
        if (state.current8F8[i] == -1) {
            continue;
        }

        const std::int32_t baseY = 0xA0;
        const std::int32_t baseX = -0x38;
        std::int32_t currentY = baseY;
        std::int32_t currentX = baseX;
        const std::int32_t targetY = add32(state.y880[i], 0x18);
        const std::int32_t targetX = add32(state.x87C[i], 0x3D);
        std::int32_t sequenceIndex = 0;

        for (std::int32_t frame = 0; frame < 12; ++frame) {
            ops.call6588(lowByte(state.buffer7138));

            // 0x122C uses the complete dword DS:7138 as the local-array
            // index.  In valid game state it is 0/1; preserving this distinction
            // is important because nearby calls use only the low byte.
            const auto page = static_cast<std::size_t>(state.buffer7138);
            if (frame >= 2) {
                restore(saved[page], ops);
            }

            const auto resource = ops.call954C(
                state.animation964[static_cast<std::size_t>(sequenceIndex)], 7);

            // 0x1261 indexes BYTE [0x970 + ESI], not a scalar flag.
            if (state.flag970[static_cast<std::size_t>(sequenceIndex)] != 0) {
                ops.callA5C8(0, resource.pointer);
            }

            SavedRect10A4& r = saved[page];

            // WORD +4 is MOVZX then signed /2 via SAR correction.  Because it
            // was zero-extended, ordinary /2 is exact for this operand.
            const std::int32_t halfWidth =
                static_cast<std::int32_t>(resource.word4) / 2;
            r.x = sub32(currentX, halfWidth);

            // BYTE +6 is zero-extended and doubled with ADD EAX,EAX.
            const std::int32_t twiceHeight = static_cast<std::int32_t>(
                static_cast<std::uint32_t>(resource.byte6) * 2u);
            r.y = sub32(currentY, twiceHeight);
            r.width = static_cast<std::int32_t>(resource.word4);
            r.height = static_cast<std::int32_t>(
                static_cast<std::uint32_t>(resource.byte6) * 8u);

            ops.call66C0(r.buffer, r.x, r.y, r.width, r.height);

            // 0x12D4 pushes EBX itself: the resource object pointer returned
            // by 0x954C, not a separate abstract handle.
            ops.call6D54(0, r.x, r.y, resource.pointer);

            if (state.flag970[static_cast<std::size_t>(sequenceIndex)] != 0) {
                ops.callA5C8(0, resource.pointer);
            }

            // 0x1300..0x130F computes signed (ESI+1) % 12 with IDIV.  ESI is
            // constrained to 0..11 here, so the observable result is 0..11.
            sequenceIndex = (sequenceIndex + 1) % 12;

            const std::int32_t step = frame + 1;
            currentY = interpolate12(baseY, targetY, step);
            currentX = interpolate12(baseX, targetX, step);

            ops.call53E4(state.buffer7138);
            xorLowByteWithOne(state.buffer7138);
        }

        // 0x136E..0x13CC: restore and redraw the first page.
        ops.call6588(lowByte(state.buffer7138));
        restore(saved[static_cast<std::size_t>(state.buffer7138)], ops);
        ops.draw.call6C60(0, state.x87C[i], state.y880[i], 0x15, 7);
        ops.call53E4(state.buffer7138);
        xorLowByteWithOne(state.buffer7138);

        // 0x13D3..0x142E: repeat on the other page.
        ops.call6588(lowByte(state.buffer7138));
        restore(saved[static_cast<std::size_t>(state.buffer7138)], ops);
        ops.draw.call6C60(0, state.x87C[i], state.y880[i], 0x15, 7);
        ops.call53E4(state.buffer7138);
        xorLowByteWithOne(state.buffer7138);
    }

    // 0x1442..0x1457: release the scratch buffers in allocation order.
    ops.call7240(saved[0].buffer);
    ops.call7240(saved[1].buffer);

    // 0x145A..0x1461 epilogue; 0x1462..0x1463 MOV EAX,EAX padding.
}

} // namespace ravenloft::game
