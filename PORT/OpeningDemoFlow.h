#pragma once
#include "games/ravenloft/data/TextDatabase.h"
#include "games/ravenloft/game/CinematicSession.h"
#include "games/ravenloft/game/OpeningDemoSession.h"
#include <filesystem>
#include <memory>

namespace ravenloft::game {
class OpeningDemoFlow {
public:
    enum class Phase { OpeningCinematic, ForgottenRealmsForest, MistTransfer, DemoComplete };
    OpeningDemoFlow(const std::filesystem::path& rloftRoot,const std::filesystem::path& cinematicRoot);
    Phase phase() const {return phase_;}
    CinematicSession& cinematic(){return *cinematic_;}
    OpeningDemoSession& game(){return game_;}
    void advanceCinematicFrame();
    void skipOpeningCinematic();
    MistTransitionResult acquireGroundObject(std::uint16_t objectId);
    void tickMist();
private:
    data::TextDatabase text_;
    OpeningDemoSession game_;
    std::unique_ptr<CinematicSession> cinematic_;
    Phase phase_=Phase::OpeningCinematic;
};
}
