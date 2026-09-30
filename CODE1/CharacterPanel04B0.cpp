#include "CharacterPanel04B0.h"

#include <cstddef>
#include <cstdint>

namespace ravenloft::game {

void CharacterPanel04B0::drawThroughStage(
    std::int32_t stage,
    const CharacterPanelState04B0& state,
    const CharacterPanelOps04B0& ops) {
    // 0x04B4..0x04E4. DS:04CC is multiplied by 0x248 before the 680B load;
    // state.field680B is therefore the selected record's unsigned byte.
    if (stage >= 1) {
        ops.draw.call6EB0(0, 0x114, 3,
                          static_cast<std::int32_t>(state.field680B) + 0x0C,
                          7, 0x5FF8);
    }

    // 0x04E7..0x0514: unsigned byte at selected-record +0x14.
    if (stage >= 2) {
        ops.draw.call6EB0(0, 0x114, 0x38,
                          static_cast<std::int32_t>(state.field680C),
                          7, 0x5FF8);
    }

    // 0x0517..0x0568: EBX counts 2,1,0 and MOVSX reads signed bytes
    // 680F,680E,680D.  -1 suppresses that entry.
    if (stage >= 3) {
        for (std::int32_t slot = 2; slot >= 0; --slot) {
            const std::int32_t value = static_cast<std::int32_t>(
                state.fields680D[static_cast<std::size_t>(slot)]);
            if (value != -1) {
                ops.draw.call6EB0(0,
                                  0x114 - slot * 0x14,
                                  0x6D - slot,
                                  value + 6,
                                  7,
                                  0x5FF8);
            }
        }
    }

    // 0x0568..0x059B: unsigned selected-record byte 6812.
    if (stage >= 4) {
        ops.draw.call6EB0(0, 0x114, 0xA2,
                          static_cast<std::int32_t>(state.field6812) + 0x0E,
                          7, 0x5FF8);
    }

    if (stage >= 5) {
        // 0x05A8..0x05BD.
        ops.draw.call6C60(0, 0x114, 0xD7, 0x14, 7);

        // 0x05C0..0x05F7. IMUL/ADD are 32-bit. Use unsigned intermediates
        // so overflow remains the original modulo-2^32 result instead of C++
        // signed-overflow UB; 680A is sign-extended by MOVSX.
        std::uint32_t computedBits =
            static_cast<std::uint32_t>(state.field680B) * 0x17u;
        computedBits += static_cast<std::uint32_t>(state.global713C);
        computedBits += static_cast<std::uint32_t>(
            static_cast<std::int32_t>(state.field680A));
        ops.draw.call6C60(0, 0x129, 0xD7,
                          static_cast<std::int32_t>(computedBits), 1);

        // 0x05FA..0x0628 deliberately tests BYTE [DS:04C4 + DS:04CC],
        // i.e. the selected index in the four-byte flag array, while the text
        // pointer itself is 0x67F8 + (DS:04CC * 0x248).  state.flag4C4 is the
        // already-indexed byte and recordBase67F8 the already-indexed pointer.
        if (state.flag4C4) {
            ops.draw.call7D80(0x116, 0xD7, 0x30, state.recordBase67F8, 0);
        }
    }

    // 0x062B..0x0640.
    if (stage >= 6) {
        ops.call38A4(0x114, 0x10C);
    }
}

} // namespace ravenloft::game
