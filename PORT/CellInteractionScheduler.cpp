#include "games/ravenloft/game/CellInteractionScheduler.h"

namespace ravenloft::game {

void CellInteractionScheduler::tick(std::vector<std::vector<CellInteractionRecord>>& columns,
                                    std::uint8_t& activeTransitionCount) {
    // CODE.2 0x20B46..0x20B4E: zero active transitions means no scan at all.
    if (activeTransitionCount == 0) return;

    auto remaining = activeTransitionCount;

    // First pass: advance all records with nonzero signed +7 delta.
    for (auto& column : columns) {
        for (auto& rec : column) {
            if (rec.type == 0) break;
            if (rec.frameDelta == 0) continue;

            // 8-bit addition is intentional and matches BYTE arithmetic.
            rec.frame = static_cast<std::uint8_t>(
                static_cast<unsigned>(rec.frame) +
                static_cast<unsigned>(static_cast<std::uint8_t>(rec.frameDelta)));

            // 0x20B88..0x20B99: only frames 0 and 8 terminate a transition.
            if (rec.frame == 0 || rec.frame == 8) {
                remaining = static_cast<std::uint8_t>(remaining - 1u);
                rec.frameDelta = 0;
            }
        }
    }

    // Once the last transition completes, the original performs a second pass
    // and canonicalizes every non-8 frame to zero.
    if (remaining == 0) {
        for (auto& column : columns) {
            for (auto& rec : column) {
                if (rec.type == 0) break;
                if (rec.frame != 8) rec.frame = 0;
            }
        }
    }

    activeTransitionCount = remaining;
}

} // namespace ravenloft::game
