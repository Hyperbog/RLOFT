#include "games/ravenloft/game/PlayerCollisionTrigger2D004.h"

namespace ravenloft::game {
namespace {
bool specialLevel(std::uint16_t level) {
    switch (level) {
    case 0x08: case 0x12: case 0x17: case 0x18:
    case 0x19: case 0x1A: case 0x1F: return true;
    default: return false;
    }
}
void renderTrapFrame(PlayerCollisionState2D004& state,
                     const PlayerCollisionTrigger2D004Callbacks& cb) {
    if (cb.refresh1F1C8) cb.refresh1F1C8();
    if (cb.draw2E510) {
        const auto base = cb.resourceBase258EC ? cb.resourceBase258EC() : 0;
        cb.draw2E510(4, 0x20, base, 9);
    }
    if (cb.refresh1EA04) cb.refresh1EA04();
    if (cb.refresh1EA78) cb.refresh1EA78();
}
}

int PlayerCollisionTrigger2D004::evaluate(
    std::uint8_t cellCode,
    PlayerCollisionState2D004& state,
    const PlayerCollisionTrigger2D004Callbacks& cb) {

    const auto mapped = cb.mappedCellObjectId ? cb.mappedCellObjectId(cellCode) : std::int16_t{-1};
    if (mapped < 0x242 || mapped > 0x247) return 0;

    const bool flag3E = cb.queryFlag1C1DC ? cb.queryFlag1C1DC(0x3E) : false;
    if (flag3E) {
        if (specialLevel(state.currentLevel)) {
            if (cb.sequence2CEB8) cb.sequence2CEB8();
            return 1;
        }
        if (state.currentLevel == 0x10 || state.currentLevel == 0x11) {
            state.partyXFixed = static_cast<std::int32_t>(0x00218000u); // 33.5
            state.partyYFixed = static_cast<std::int32_t>(0x00028000u); // 2.5
            state.facingDegrees = 0;
            if (cb.loadLevel181B4) cb.loadLevel181B4(0x1F);
            state.currentLevel = 0x1F;
            return 1;
        }
        return 0;
    }

    // 0x2D12D..0x2D14C: collision probe coordinates become the live party
    // coordinates before the scripted damage/turn sequence.
    state.partyXFixed = state.savedXFixed;
    state.partyYFixed = state.savedYFixed;
    if (cb.refresh1F1C8) cb.refresh1F1C8();
    if (cb.refresh1EA78) cb.refresh1EA78();

    // 0x2D15B..0x2D1A4: every living party slot (<0x4000 state word)
    // takes an independent 1d2 roll through the normal damage routine.
    for (std::size_t i=0;i<4;++i) {
        const auto status = cb.partyStatusWord ? cb.partyStatusWord(i) : std::uint16_t{0x4000};
        if (status >= 0x4000u) continue;
        const int damage = cb.roll1d2 ? cb.roll1d2() : 1;
        if (cb.damageCharacterCD44) cb.damageCharacterCD44(i, damage);
    }

    renderTrapFrame(state, cb);
    state.facingDegrees += 180;
    state.facingDegrees %= 360;
    if (state.facingDegrees < 0) state.facingDegrees += 360;
    renderTrapFrame(state, cb);
    state.stateBF44 = 0x2E;
    return 1;
}

} // namespace ravenloft::game
