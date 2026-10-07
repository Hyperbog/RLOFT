#pragma once

#include <array>
#include <cstdint>

namespace rloft {

enum class CharacterCreationState : std::uint8_t {
    ActorChoice = 0,
    SexChoice = 1,
    RaceChoice = 2,
};

struct CharacterDraft {
    int actor{-1};
    int sex{-1};
    int race{-1};
};

class CharacterCreationFlow final {
public:
    [[nodiscard]] CharacterCreationState state() const noexcept { return state_; }
    [[nodiscard]] const CharacterDraft& draft() const noexcept { return draft_; }

    // CODE.1 035C hit-test result. Coordinate order follows the engine's
    // 320x400 logical surface: row (0..399), column (0..319).
    [[nodiscard]] static int hit_test(
        CharacterCreationState state, int row, int column) noexcept;

    // Consume an already-resolved CODE.1 UI event (0 cancel, 1..6 choices).
    // Returns true when the state/draft changed.
    bool apply_event(int event);

private:
    CharacterCreationState state_{CharacterCreationState::ActorChoice};
    CharacterDraft draft_{};
};

}  // namespace rloft
