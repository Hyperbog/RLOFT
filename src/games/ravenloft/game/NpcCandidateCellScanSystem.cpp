#include "games/ravenloft/game/NpcCandidateCellScanSystem.h"
#include "games/ravenloft/game/NpcCellInteractionSystem.h"
#include <stdexcept>

namespace ravenloft::game {
namespace {
int cellAfterWrappedOffset(std::int32_t fixed, std::int32_t offset) {
    // Original code performs a wrapping 32-bit ADD, then MOVSX of the high
    // 16-bit word. Preserve that rather than relying on implementation-defined
    // signed right shift behavior.
    const auto wrapped = static_cast<std::uint32_t>(fixed) +
                         static_cast<std::uint32_t>(offset);
    return static_cast<std::int16_t>(wrapped >> 16);
}
}

NpcCandidateCellScanResult NpcCandidateCellScanSystem::evaluate(
    std::int32_t candidateYFixed,
    std::int32_t candidateXFixed,
    int moverId,
    std::uint8_t moverNpcTraits39,
    bool scanSuppressedF048,
    const CellInteractionCallbacks& interactionCallbacks) const {

    if (moverId < 0 || moverId >= 0x190)
        throw std::invalid_argument("NpcCandidateCellScanSystem requires ordinary NPC mover id 0..399");
    if (work_.width != interactions_.width() || work_.height != interactions_.height() ||
        work_.cells.size() != static_cast<std::size_t>(work_.width) * work_.height)
        throw std::runtime_error("Candidate cell scan level dimensions do not match interaction index");

    NpcCandidateCellScanResult out;

    // 0x2D5DC..0x2D629: candidate footprint is +/-0x5800 in each 16.16 axis.
    const int minY = cellAfterWrappedOffset(candidateYFixed, -0x5800);
    const int minX = cellAfterWrappedOffset(candidateXFixed, -0x5800);
    const int maxY = cellAfterWrappedOffset(candidateYFixed,  0x5800);
    const int maxX = cellAfterWrappedOffset(candidateXFixed,  0x5800);

    for (int x = minX; x <= maxX; ++x) {
        for (int y = minY; y <= maxY; ++y) {
            // DS:F048 skips the complete per-cell scan body.
            if (scanSuppressedF048) continue;
            if (x < 0 || y < 0 || x >= work_.width || y >= work_.height) continue;

            const auto cellIndex = static_cast<std::size_t>(y) * work_.width +
                                   static_cast<std::size_t>(x);
            auto* interaction = interactions_.find(x, y); // CODE.2 0x1E8C8
            const auto cellWord = work_.cells[cellIndex];

            // 0x2D6DC..0x2D811: interaction-coded cell handling.
            if ((cellWord & 0x0080u) != 0) {
                const auto interactionCode = interactions_.dynamicInteractionCode(
                    static_cast<std::int32_t>(cellIndex)); // CODE.2 0x1E930
                if (interactionCode != 0) {
                    const auto q = NpcCellInteractionSystem::evaluate(
                        interactionCode, interaction, moverNpcTraits39,
                        interactionCallbacks);
                    out.blocked = out.blocked || q.blocked;
                    out.transitionTriggered = out.transitionTriggered || q.transitionTriggered;
                }
            }

            // 0x2D811: low six bits zero means no further cell-geometry test.
            if ((cellWord & 0x003Fu) == 0) continue;

            // 0x2D823..0x2D82F: bit 0x0800 blocks every ordinary NPC mover.
            if ((cellWord & 0x0800u) != 0) {
                out.blocked = true;
                continue;
            }

            // 0x2D835..0x2D87F: without bit 0x1000 only the player can enter
            // the 0x2D004 helper; ordinary NPCs are blocked.
            if ((cellWord & 0x1000u) == 0) {
                out.blocked = true;
                continue;
            }

            // 0x2D884..0x2D92B: if no interaction record exists this branch
            // does not add a block. Type 1 and type 8 are also passable for an
            // ordinary NPC (mover 0x190 and player -1 have separate behavior).
            if (!interaction) continue;
            if (interaction->type == 1 || interaction->type == 8) continue;
            out.blocked = true;
        }
    }

    return out;
}

} // namespace ravenloft::game
