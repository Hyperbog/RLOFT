#pragma once
// Bounded C++ reconstruction of the observed R.EXE / M.EXE / SP.EXE
// CODE.2 -> CODE.1 DOS child-status loop and file cleanup protocol.
// Actions describe original behavior; a native game need not call DOS/4GW.
#include "rloft/game_profile.hpp"
#include <cstdint>
#include <stdexcept>
#include <vector>

namespace rloft {

enum class LauncherModule : std::uint8_t { NotStarted, Code2, Code1, Stopped };
enum class LauncherAction : std::uint8_t {
    CreateWork, CreateAutomaps, CreateGsr, DeleteChars,
    RemoveWork, RemoveAutomaps, RemoveGsr,
    StartCode2, StartCode1, Stop
};
struct LauncherStep final {
    LauncherModule next{LauncherModule::NotStarted};
    std::vector<LauncherAction> actions;
};

// Source bytes: R.EXE physical 286e..28db;
// M.EXE physical 288e..2912; SP.EXE physical 2975..2a17.
// The CODE child return values 10/12 are shared; only M/SP handle 13.
[[nodiscard]] inline LauncherStep original_launcher_transition(
    GameId game, LauncherModule returning, int status) {
    if (returning != LauncherModule::Code2 && returning != LauncherModule::Code1)
        throw std::invalid_argument("not a returning CODE child");
    const bool menzo = game == GameId::Menzoberranzan;
    const bool stone = game == GameId::StoneProphet;
    if (!menzo && !stone && game != GameId::StrahdsPossession)
        throw std::invalid_argument("unknown game profile");
    LauncherStep step;
    // Stone SP.EXE explicitly unlinks CHARS after *every* CODE.2 process.
    if (stone && returning == LauncherModule::Code2)
        step.actions.push_back(LauncherAction::DeleteChars);
    if (returning == LauncherModule::Code2 && status == 10) {
        step.next = LauncherModule::Code1;
        step.actions.push_back(LauncherAction::StartCode1);
        return step;
    }
    if (returning == LauncherModule::Code2 && status == 12) {
        step.next = LauncherModule::Code2;
        // For Stone this is an additional delete, beyond the unconditional one.
        step.actions.push_back(LauncherAction::DeleteChars);
        step.actions.push_back(LauncherAction::StartCode2);
        return step;
    }
    if (returning == LauncherModule::Code2 && status == 13 && (menzo || stone)) {
        step.next = LauncherModule::Code2;
        step.actions.push_back(LauncherAction::StartCode2);
        return step;
    }
    if (returning == LauncherModule::Code1 && status == 10) {
        step.next = LauncherModule::Code2;
        step.actions.push_back(LauncherAction::StartCode2);
        return step;
    }
    step.next = LauncherModule::Stopped;
    step.actions.push_back(LauncherAction::DeleteChars);
    if (menzo || stone) step.actions.push_back(LauncherAction::RemoveWork);
    if (stone) {
        step.actions.push_back(LauncherAction::RemoveAutomaps);
        step.actions.push_back(LauncherAction::RemoveGsr);
    }
    step.actions.push_back(LauncherAction::Stop);
    return step;
}

class OriginalLauncher final {
public:
    explicit OriginalLauncher(GameId game) : game_(game) {}
    [[nodiscard]] LauncherModule next_module() const noexcept { return next_; }
    [[nodiscard]] LauncherStep begin() {
        if (next_ != LauncherModule::NotStarted)
            throw std::logic_error("launcher already started");
        if (!game_profile(game_)) throw std::invalid_argument("unknown game");
        next_ = LauncherModule::Code2;
        LauncherStep step{next_, {LauncherAction::CreateWork,
                                  LauncherAction::CreateAutomaps}};
        if (game_ != GameId::StrahdsPossession)
            step.actions.push_back(LauncherAction::CreateGsr);
        if (game_ == GameId::StoneProphet)
            step.actions.push_back(LauncherAction::DeleteChars);
        step.actions.push_back(LauncherAction::StartCode2);
        return step;
    }
    [[nodiscard]] LauncherStep complete(LauncherModule returned, int status) {
        if (returned != next_ || next_ == LauncherModule::NotStarted ||
            next_ == LauncherModule::Stopped)
            throw std::logic_error("out-of-order CODE child completion");
        auto step = original_launcher_transition(game_, returned, status);
        next_ = step.next;
        return step;
    }
private:
    GameId game_;
    LauncherModule next_{LauncherModule::NotStarted};
};

} // namespace rloft
