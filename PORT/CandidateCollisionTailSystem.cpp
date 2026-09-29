#include "games/ravenloft/game/CandidateCollisionTailSystem.h"

namespace ravenloft::game {

int CandidateCollisionTailSystem::evaluate(
    std::int32_t candidateYFixed,
    std::int32_t candidateXFixed,
    int moverId,
    std::uint8_t moverNpcTraits39,
    std::uint16_t currentLevel,
    std::int32_t partyXFixed,
    std::int32_t partyYFixed,
    bool blockedByEarlierCellScan,
    const SceneryCollisionCallbacks& sceneryCallbacks) const {

    // 0x2D944: mover -2 bypasses NPC/scenery/party tail checks entirely.
    if (moverId != -2) {
        if (npcOverlap_.overlaps(candidateYFixed, candidateXFixed, moverId))
            return 0;

        // Mover id 0x190 skips scenery collision but still reaches the party
        // condition below only if nonnegative, matching 0x2D972..0x2D9B0.
        if (moverId != 0x190) {
            const auto s = scenery_.query(candidateYFixed,
                                          candidateXFixed,
                                          moverId,
                                          moverNpcTraits39,
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

    return blockedByEarlierCellScan ? 0 : 1;
}

} // namespace ravenloft::game
