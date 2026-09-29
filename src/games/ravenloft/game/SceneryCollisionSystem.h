#pragma once

#include "games/ravenloft/data/InitDatabase.h"
#include "games/ravenloft/data/WorkLevel.h"
#include <cstdint>
#include <functional>

namespace ravenloft::game {

enum class SceneryCollisionResult : int {
    Clear = 0,
    Blocked = 1,
    Special = 2
};

struct SceneryCollisionPlayerState {
    std::int32_t partyXFixed = 0;
    std::int32_t partyYFixed = 0;
    std::int32_t facingDegrees = 0;
    std::uint16_t currentLevel = 0;
};

struct SceneryCollisionCallbacks {
    // Player-only side effects from CODE.2 0x2CC88. The collision routine owns
    // the decision to invoke them; their broader event/teleport implementations
    // remain separate reconstruction dependencies.
    // CODE.2 0x2CD99..0x2CE34 performs these state mutations inline before
    // returning Special=2. When supplied, playerState is mutated exactly.
    SceneryCollisionPlayerState* playerState = nullptr;
    std::function<void(std::int32_t facing)> setFacing1C0C0;
    std::function<void(std::uint16_t level)> loadLevel181B4;
    std::function<void(std::uint16_t oldLevel)> onType54Special;
    std::function<void()> onType7ASpecial;
};

class SceneryCollisionSystem {
public:
    SceneryCollisionSystem(const data::WorkLevelSnapshot& work,
                           const data::InitDatabase& init)
        : work_(work), init_(init) {}

    // Direct reconstruction of CODE.2 0x2CC88.
    // Argument order mirrors the original routine: candidate Y, candidate X,
    // then mover id (-1 player, 0..399 ordinary NPC).
    SceneryCollisionResult query(std::int32_t candidateYFixed,
                                 std::int32_t candidateXFixed,
                                 int moverId,
                                 std::uint8_t moverNpcTraits39,
                                 std::uint16_t currentLevel,
                                 const SceneryCollisionCallbacks& callbacks = {}) const;

private:
    const data::WorkLevelSnapshot& work_;
    const data::InitDatabase& init_;
};

} // namespace ravenloft::game
