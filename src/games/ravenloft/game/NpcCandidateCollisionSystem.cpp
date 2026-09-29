#include "games/ravenloft/game/NpcCandidateCollisionSystem.h"
#include <stdexcept>

namespace ravenloft::game {

int NpcCandidateCollisionSystem::evaluate(
    std::int32_t candidateYFixed,
    std::int32_t candidateXFixed,
    int moverId,
    std::int32_t partyXFixed,
    std::int32_t partyYFixed,
    bool scanSuppressedF048,
    const CellInteractionCallbacks& interactionCallbacks,
    const SceneryCollisionCallbacks& sceneryCallbacks) const {

    if (moverId < 0 || moverId >= 0x190 ||
        static_cast<std::size_t>(moverId) >= work_.npcs.size())
        throw std::invalid_argument("NpcCandidateCollisionSystem requires a live ordinary NPC mover");

    const auto type = work_.npcs[static_cast<std::size_t>(moverId)].type;
    if (type >= npcs_.count())
        throw std::runtime_error("Runtime NPC type outside NPCS.DAT");
    const auto traits39 = npcs_.at(type).traits39;

    const auto cell = cellScan_.evaluate(candidateYFixed, candidateXFixed,
                                         moverId, traits39,
                                         scanSuppressedF048,
                                         interactionCallbacks);

    // For ordinary NPCs the scenery helper cannot return the player-only
    // Special=2 case, so the complete 0x2D5D0 result is 0 or 1 here.
    return tail_.evaluate(candidateYFixed, candidateXFixed,
                          moverId, traits39,
                          /*currentLevel supplied only for player specials*/ 0,
                          partyXFixed, partyYFixed,
                          cell.blocked, sceneryCallbacks);
}

} // namespace ravenloft::game
