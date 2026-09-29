#include "games/ravenloft/game/CandidateCollisionSystem2D5D0.h"
#include "games/ravenloft/game/NpcCellInteractionSystem.h"
#include "games/ravenloft/game/PartyOverlapSystem.h"
#include <stdexcept>

namespace ravenloft::game {
namespace {
int cellAfterWrappedOffset(std::int32_t fixed, std::int32_t offset) {
    const auto wrapped = static_cast<std::uint32_t>(fixed) +
                         static_cast<std::uint32_t>(offset);
    return static_cast<std::int16_t>(wrapped >> 16);
}
}

int CandidateCollisionSystem2D5D0::evaluate(
    std::int32_t candidateYFixed,
    std::int32_t candidateXFixed,
    int moverId,
    std::int32_t partyXFixed,
    std::int32_t partyYFixed,
    std::uint16_t currentLevel,
    std::uint32_t& scanSuppressedF048,
    const CellInteractionCallbacks& interactionCallbacks,
    const CandidateCollisionPlayerCallbacks& playerCallbacks,
    const SceneryCollisionCallbacks& sceneryCallbacks) const {

    if (work_.width != interactions_.width() ||
        work_.height != interactions_.height() ||
        work_.cells.size() != static_cast<std::size_t>(work_.width) * work_.height)
        throw std::runtime_error("CODE.2 0x2D5D0 level/interaction dimensions disagree");

    std::uint8_t moverTraits39 = 0;
    const bool ordinaryNpc = moverId >= 0 && moverId < 0x190;
    if (ordinaryNpc) {
        if (static_cast<std::size_t>(moverId) >= work_.npcs.size())
            throw std::invalid_argument("CODE.2 0x2D5D0 ordinary NPC mover outside runtime array");
        const auto type = work_.npcs[static_cast<std::size_t>(moverId)].type;
        if (type >= npcs_.count())
            throw std::runtime_error("CODE.2 0x2D5D0 runtime NPC type outside NPCS.DAT");
        moverTraits39 = npcs_.at(type).traits39;
    }

    bool blocked = false; // local [ebp-0x30]

    // 0x2D5DC..0x2D629: 16.16 candidate footprint +/- 0x5800.
    const int minY = cellAfterWrappedOffset(candidateYFixed, -0x5800);
    const int minX = cellAfterWrappedOffset(candidateXFixed, -0x5800);
    const int maxY = cellAfterWrappedOffset(candidateYFixed,  0x5800);
    const int maxX = cellAfterWrappedOffset(candidateXFixed,  0x5800);

    for (int x = minX; x <= maxX; ++x) {
        for (int y = minY; y <= maxY; ++y) {
            // 0x2D677: DS:F048 skips the complete per-cell body.
            if (scanSuppressedF048 != 0) continue;
            if (x < 0 || y < 0 || x >= work_.width || y >= work_.height) continue;

            const auto cellIndex = static_cast<std::size_t>(y) * work_.width +
                                   static_cast<std::size_t>(x);
            auto* interaction = interactions_.find(x, y); // 0x1E8C8
            const auto cellWord = work_.cells[cellIndex];

            // 0x2D6DC..0x2D811: runtime interaction-coded cell branch.
            if ((cellWord & 0x0080u) != 0) {
                const auto interactionCode = interactions_.dynamicInteractionCode(
                    static_cast<std::int32_t>(cellIndex)); // 0x1E930
                if (interactionCode != 0 && interaction != nullptr && interaction->type != 8) {
                    if (interactionCode == 0x8000u) {
                        // 0x2D72D..0x2D7D3.
                        if ((interaction->flags & 0x04u) != 0 &&
                            (interaction->flags & 0x40u) == 0 &&
                            ordinaryNpc && (moverTraits39 & 0x40u) != 0) {
                            if (interaction->frameDelta == 0) {
                                const auto originalBit04 = static_cast<std::uint8_t>(interaction->flags & 0x04u);
                                interaction->flags = static_cast<std::uint8_t>(interaction->flags | 0x04u);
                                (void)CellInteractionSystem::activate(x, y, *interaction, interactionCallbacks);
                                interaction->flags = static_cast<std::uint8_t>(
                                    (interaction->flags & ~0x04u) | originalBit04);
                            } else {
                                blocked = true;
                            }
                        } else {
                            // Mover -2 is the sole non-NPC exception here.
                            if (moverId != -2) blocked = true;
                        }
                    } else {
                        // 0x2D7D5..0x2D811. Ordinary NPC trait 0x80 grants
                        // pass-through; all nonordinary mover ids are blocked.
                        if (ordinaryNpc) {
                            if ((moverTraits39 & 0x80u) == 0) blocked = true;
                        } else {
                            blocked = true;
                        }
                    }
                }
            }

            // 0x2D811: no low-six geometry code, no remaining cell work.
            const auto cellCode = static_cast<std::uint8_t>(cellWord & 0x003Fu);
            if (cellCode == 0) continue;

            // 0x2D823..0x2D92D. Bit 0x0800 blocks every mover except player -1.
            if ((cellWord & 0x0800u) != 0) {
                if (moverId != -1) blocked = true;
                continue;
            }

            if ((cellWord & 0x1000u) == 0) {
                // 0x2D843..0x2D87F. Only player -1 invokes 0x2D004.
                if (moverId == -1 && playerCallbacks.trigger2D004) {
                    if (playerCallbacks.trigger2D004(cellCode) != 0)
                        return 1;
                }
                blocked = true;
                continue;
            }

            // 0x2D884..0x2D92B. Missing runtime interaction adds no block.
            if (!interaction) continue;

            // Special mover -2 is blocked before the type dispatch.
            if (moverId == -2) {
                blocked = true;
                continue;
            }

            if (interaction->type == 1) {
                continue;
            }
            if (interaction->type == 8) {
                if (moverId == 0x190) {
                    blocked = true;
                    continue;
                }
                if (moverId == -1) {
                    if (playerCallbacks.portal2D3D8) {
                        playerCallbacks.portal2D3D8(
                            x, y,
                            static_cast<std::uint8_t>(interaction->frameDelta),
                            interaction->opaque8,
                            interaction->opaque9,
                            interaction->frame);
                    }
                    scanSuppressedF048 = 1;
                    return 1;
                }
                // Ordinary NPCs pass type 8 here.
                continue;
            }

            blocked = true;
        }
    }

    // 0x2D944..0x2D9F8: exact late collision tail.
    if (moverId != -2) {
        if (npcOverlap_.overlaps(candidateYFixed, candidateXFixed, moverId))
            return 0;

        if (moverId != 0x190) {
            const auto s = scenery_.query(candidateYFixed,
                                          candidateXFixed,
                                          moverId,
                                          moverTraits39,
                                          currentLevel,
                                          sceneryCallbacks);
            if (s == SceneryCollisionResult::Special) return -1;
            if (s == SceneryCollisionResult::Blocked) return 0;
        }

        if (moverId >= 0 &&
            PartyOverlapSystem::overlaps(candidateYFixed,
                                         candidateXFixed,
                                         partyXFixed,
                                         partyYFixed))
            return 0;
    }

    return blocked ? 0 : 1;
}

} // namespace ravenloft::game
