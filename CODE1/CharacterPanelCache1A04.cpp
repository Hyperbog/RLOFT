#include "CharacterPanelCache1A04.h"

#include <cstdint>

namespace ravenloft::game {
namespace {

// Shared source-level spelling of the two duplicated binary branches.  The
// calls and arithmetic below retain the exact order and widths at 0x1A71..
// 0x1AF5 and 0x1B12..0x1B96.
void drawPanel(std::uint8_t live,
               const char* record,
               std::int8_t signedField,
               std::uint8_t unsignedField,
               std::int32_t globalBase,
               const CharacterPanelCacheOps1A04& ops) {
    if (live == 0) {
        // PUSH 0; PUSH DS:55F8; CALL 0x004C.
        ops.call004C(ops.textBuffer55F8, 0);

        // Original 0x00EC stack contract:
        // x=0, y=0, width=0x7A, textWidth=0x30, text=DS:55F8,
        // mode=0, surface/resource=DS:5FF8.
        ops.call00EC(0, 0, 0x7A, 0x30, ops.textBuffer55F8, 0, 0x5FF8);
        return;
    }

    // 0x1AA7..0x1AB6 / 0x1B48..0x1B57: base portrait resource.
    ops.draw.call6C60(0, 0, 0, 0x14, 7);

    // MOVSX signed byte; ADD DWORD DS:713C; MOVZX unsigned byte;
    // IMUL by 0x17; ADD.  The x86 ADD/IMUL result is modulo 2^32.
    const std::uint32_t signedPart =
        static_cast<std::uint32_t>(static_cast<std::int32_t>(signedField));
    const std::uint32_t base = static_cast<std::uint32_t>(globalBase);
    const std::uint32_t stride =
        static_cast<std::uint32_t>(unsignedField) * 0x17u;
    const std::int32_t resource = static_cast<std::int32_t>(base + signedPart + stride);
    ops.draw.call6C60(0, 0x15, 0, resource, 1);

    // The original passes the address of the character record itself.
    ops.draw.call7D80(2, 0, 0x30, record, 0);
}

} // namespace

void CharacterPanelCache1A04::rebuild(
    CharacterPanelCacheState1A04& state,
    const CharacterPanelCacheOps1A04& ops) {
    // 0x1A08..0x1A47: release old cached handles independently and zero
    // each slot immediately after its corresponding release.
    if (state.handle95C != 0) {
        ops.call7830(state.handle95C);
        state.handle95C = 0;
    }
    if (state.handle960 != 0) {
        ops.call7830(state.handle960);
        state.handle960 = 0;
    }

    // 0x1A4A uses MOVZX BYTE [7138], not a DWORD read.
    ops.call6588(static_cast<std::uint8_t>(state.buffer7138));

    // 0x1A5A..0x1A6F: temporary snapshot handle, retained in 32-bit EBX.
    const std::uint32_t temporary = ops.call7868(0, 0, 0x7A, 0x30, 0xFF);

    // 0x1A71..0x1AF5.
    drawPanel(state.flag4C4, state.record67F8,
              state.field680A, state.field680B,
              state.global713C, ops);

    // 0x1AF8..0x1B0D: first persistent cache snapshot.
    state.handle95C = ops.call7868(0, 0, 0x7A, 0x30, 0xFF);

    // 0x1B12..0x1B96.
    drawPanel(state.flag4C5, state.record6A40,
              state.field6A52, state.field6A53,
              state.global713C, ops);

    // 0x1B99..0x1BAE: second persistent cache snapshot.
    state.handle960 = ops.call7868(0, 0, 0x7A, 0x30, 0xFF);

    // 0x1BB3..0x1BC8: restore the temporary snapshot, then free it.
    ops.call67C8(0, 0, 0, temporary);
    ops.call7830(temporary);

    // 0x1BCB..0x1BD0 is the epilogue/RET.
    // 0x1BD1..0x1BD3 is LEA EAX,[EAX+0] alignment padding.
}

} // namespace ravenloft::game
