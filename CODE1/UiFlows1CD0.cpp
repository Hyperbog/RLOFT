#include "UiFlows1CD0.h"

#include <array>
#include <bit>
#include <cstddef>
#include <cstdint>

namespace ravenloft::game {
namespace {

std::int32_t wrapAdd32(std::int32_t value, std::uint32_t delta) noexcept {
    return std::bit_cast<std::int32_t>(
        std::bit_cast<std::uint32_t>(value) + delta);
}

std::int32_t wrapSub32(std::int32_t value, std::uint32_t delta) noexcept {
    return std::bit_cast<std::int32_t>(
        std::bit_cast<std::uint32_t>(value) - delta);
}

} // namespace

void UiFlow1CD0::run(UiFlowCharacterState& state,
                     const UiFlowOps1CD0& ops) {
    // 0x1CD0..0x1CD9: ordinary frame, then direct CALL 0x1A04.
    ops.call1A04();

    // 0x1CDA..0x1D13: BYTE tests at DS:04C4/04C5 choose the third
    // 0x1464 slot value (1 only when both bytes are nonzero, otherwise 2).
    // The remaining pushed values are literal constants in the original order.
    const std::array<std::int32_t, 9> slots{
        -1, -1, 1, 1, -1, -1, -1, -1, -1
    };
    const std::int32_t text940 =
        (state.flags4C4[0] != 0 && state.flags4C4[1] != 0) ? 1 : 2;
    ops.call1464(0, slots, text940, -1, -1);

    // 0x1D16..0x1D8D.  TEST EAX,EAX / JBE has CF cleared by TEST, so the
    // first branch is taken only for EAX==0.  The later JB/JA comparisons are
    // unsigned; negative signed C++ values therefore do NOT behave as <=0.
    bool finished = false;
    while (!finished) {
        const std::int32_t selection = ops.call043C();
        const std::uint32_t rawSelection =
            std::bit_cast<std::uint32_t>(selection);

        if (rawSelection == 0u) {
            state.selectedCharacter4CC = -1;
            finished = true;
            continue;
        }

        if (rawSelection < 3u || rawSelection > 4u) {
            continue;
        }

        // 0x1D2B..0x1D3B: LEA EBX,[EAX-3], store DWORD DS:04CC, then use
        // the unscaled EBX as the BYTE index into DS:04C4.
        const std::int32_t characterIndex = selection - 3;
        state.selectedCharacter4CC = characterIndex;

        if (state.flags4C4[static_cast<std::size_t>(characterIndex)] != 0) {
            ops.call1BD4();
            continue;
        }

        // 0x1D44..0x1D7A: four -1 arguments, INC DWORD DS:04D0, then the
        // selected character is scaled by 0x248 before clearing DWORDs at
        // record offsets 0x10B and 0x10F (absolute DS:6903/6907 for record 0).
        ops.call15B0(-1, -1, -1, -1);
        state.mode4D0 = wrapAdd32(state.mode4D0, 1u);
        state.selectedCharacter4CC = characterIndex;
        state.dword6903[static_cast<std::size_t>(characterIndex)] = 0;
        state.dword6907[static_cast<std::size_t>(characterIndex)] = 0;
        finished = true;
    }

    // 0x1D8F..0x1DD0: destroy each nonzero cache handle independently and
    // clear its full DWORD storage immediately after the corresponding call.
    if (state.handle95C != 0) {
        ops.call7830(state.handle95C);
        state.handle95C = 0;
    }
    if (state.handle960 != 0) {
        ops.call7830(state.handle960);
        state.handle960 = 0;
    }
    // 0x1DD1..0x1DD7: epilogue/RET.
}

void UiFlow1DD8::run(UiFlowCharacterState& state,
                     const UiFlowOps1CD0& ops) {
    // 0x1DD8..0x1DFB: frame and literal argument sequence for CALL 0x1464.
    const std::array<std::int32_t, 9> slots{
        -1, -1, 0x0C, 0x0D, -1, -1, -1, -1, -1
    };
    ops.call1464(0, slots, 8, -1, 0x26);

    // 0x1DFE..0x1E5A: same unsigned selection tests as 0x1CD0.
    for (;;) {
        const std::int32_t selection = ops.call043C();
        const std::uint32_t rawSelection =
            std::bit_cast<std::uint32_t>(selection);

        if (rawSelection == 0u) {
            // 0x1E3D..0x1E50: push four -1 values, DEC DWORD DS:04D0, call.
            ops.call15B0(-1, -1, -1, -1);
            state.mode4D0 = wrapSub32(state.mode4D0, 1u);
            break;
        }

        if (rawSelection < 3u || rawSelection > 4u) {
            continue;
        }

        // 0x1E15..0x1E24: BL receives AL and only BL is decremented.  The
        // character record address is DS:680B + DS:04CC*0x248.
        const std::uint8_t value =
            static_cast<std::uint8_t>(static_cast<std::uint8_t>(selection) - 3u);
        state.byte680B[static_cast<std::size_t>(state.selectedCharacter4CC)] = value;

        // 0x1E2A..0x1E4B: INC is 32-bit wrapping.  EDX holds the original
        // EAX result and DEC EDX wraps before being pushed as argument one.
        state.mode4D0 = wrapAdd32(state.mode4D0, 1u);
        const std::int32_t selectedSlot = wrapSub32(selection, 1u);
        ops.call15B0(selectedSlot, 0x114, 3, 1);
        break;
    }
    // 0x1E5C..0x1E61: epilogue/RET; 0x1E62..0x1E63 is MOV EAX,EAX padding.
}

void UiFlow1E64::run(UiFlowCharacterState& state,
                     const UiFlowOps1CD0& ops) {
    // 0x1E64..0x1E87: frame and literal argument sequence for CALL 0x1464.
    const std::array<std::int32_t, 9> slots{
        0, 1, 2, 3, 4, 5, -1, -1, -1
    };
    ops.call1464(1, slots, 9, -1, 0x27);

    // 0x1E8A..0x1ED9. TEST/JE distinguishes exactly zero from nonzero.
    // The original then MOV EAX,1 before TEST EAX,EAX, so the apparent loop
    // executes exactly one selection result.
    const std::int32_t selection = ops.call043C();
    if (selection != 0) {
        // MOV BL,AL / DEC BL stores byte(AL-1), independently of upper EAX.
        state.byte680C[static_cast<std::size_t>(state.selectedCharacter4CC)] =
            static_cast<std::uint8_t>(static_cast<std::uint8_t>(selection) - 1u);
        state.mode4D0 = wrapAdd32(state.mode4D0, 1u);
        ops.call15B0(wrapSub32(selection, 1u), 0x114, 0x38, 2);
    } else {
        ops.call15B0(-1, -1, -1, -1);
        state.mode4D0 = wrapSub32(state.mode4D0, 1u);
    }
    // 0x1EDB..0x1EE0: epilogue/RET; 0x1EE1..0x1EE3 is LEA EAX,[EAX+0].
}

void SwapByte1EE4::swap(std::uint8_t& a, std::uint8_t& b) noexcept {
    // 0x1EE4..0x1EFE: the routine copies the first byte through DH and the
    // second through DL, then writes DL to *arg1 and DH to *arg2.  No wider
    // loads/stores occur.  0x1EFF is a one-byte NOP after the RET.
    const std::uint8_t temporary = a;
    a = b;
    b = temporary;
}

// CODE.1 0x1F00..0x1F23 is not executable fall-through.  It is the exact
// nine-DWORD dispatch table consumed by the following 0x1F24 routine:
//   0x20DD,0x20DD,0x20DD, 0x210F,0x210F,0x210F,
//   0x213D,0x2161,0x217C.
static constexpr std::array<std::uint32_t, 9> kCharacterTripleDispatch1F00{
    0x20DDu, 0x20DDu, 0x20DDu,
    0x210Fu, 0x210Fu, 0x210Fu,
    0x213Du, 0x2161u, 0x217Cu
};
static_assert(kCharacterTripleDispatch1F00.size() == 9);

} // namespace ravenloft::game
