#pragma once
#include "games/ravenloft/game/OpeningSequence.h"
#include "games/ravenloft/game/OriginalWorldClock1E75C.h"
#include <cstdint>

namespace ravenloft::game {

// Faithful state-machine form of CODE.2 0x21F70. One tick corresponds to one
// original render/update iteration where the routine visibly changes heading or
// position. The 0x222C0 wait is driven by the reconstructed DS:CF4C world clock.
class MistTransition {
public:
    enum class Phase {
        TurnForX,
        MoveX,
        TurnForY,
        MoveY,
        TurnIntoMist,
        StartMistEffect,
        WaitMistEffect,
        TransferFrames,
        Complete
    };

    MistTransition(PartyState& party, OriginalWorldClock1E75C& worldClock);

    // Returns true once the transition has completed. CODE.2 0x222C0..0x222E5
    // reads and advances the real DS:CF4C world clock; there is no forced-ready
    // parameter in the reconstructed production path.
    bool tick();

    Phase phase() const { return phase_; }
    int transferFrame() const { return transferFrame_; }
    bool levelLoaded() const { return levelLoaded_; }

private:
    PartyState& party_;
    OriginalWorldClock1E75C& worldClock_;
    Phase phase_ = Phase::TurnForX;
    int xTargetHeading_ = 0;
    int yTargetHeading_ = 0;
    int transferFrame_ = 0;
    bool levelLoaded_ = false;

    static int integerPart(std::int32_t fixed);
    static std::int32_t withIntegerHalf(int integer);
    static bool stepTurnX(int& heading, int target);
    static bool stepTurnY(int& heading, int target);
    static bool stepTurnFinal(int& heading);
    void loadSvalichWoods();
    void finalizeInventory();
};

} // namespace ravenloft::game
