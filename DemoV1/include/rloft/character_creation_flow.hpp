#pragma once

#include <array>
#include <cstdint>

namespace rloft {

enum class CharacterCreationState : std::uint8_t {
    ActorChoice = 0,
    SexChoice = 1,
    RaceChoice = 2,
    ClassChoice = 3,
    OptionChoice = 4,
    PortraitChoice = 5,
    StatsReview = 6,
};

struct CharacterDraft {
    int actor{-1};
    int sex{-1};
    int race{-1};
    std::array<int, 3> classes{{-1, -1, -1}};
    int option{-1};
    int face{-1};
};

class CharacterCreationFlow final {
public:
    [[nodiscard]] CharacterCreationState state() const noexcept { return state_; }
    [[nodiscard]] const CharacterDraft& draft() const noexcept { return draft_; }
    [[nodiscard]] bool aborted() const noexcept { return aborted_; }
    [[nodiscard]] int class_stage() const noexcept { return class_stage_; }
    [[nodiscard]] int portrait_page() const noexcept { return portrait_page_; }

    [[nodiscard]] std::array<bool, 6> available_choices() const noexcept;

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
    bool aborted_{};
    int class_stage_{};
    int portrait_page_{};
};

}  // namespace rloft
