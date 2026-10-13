#pragma once

#include <array>
#include <cstdint>
#include <string>

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
    std::string name;
};

class CharacterCreationFlow final {
public:
    [[nodiscard]] CharacterCreationState state() const noexcept { return state_; }
    [[nodiscard]] const CharacterDraft& draft() const noexcept { return draft_; }
    [[nodiscard]] bool aborted() const noexcept { return aborted_; }
    [[nodiscard]] bool completed() const noexcept { return completed_; }
    [[nodiscard]] int class_stage() const noexcept { return class_stage_; }
    [[nodiscard]] int portrait_page() const noexcept { return portrait_page_; }
    [[nodiscard]] bool actor_present(int actor) const noexcept {
        return actor >= 0 && actor < 2 &&
               actor_present_[static_cast<std::size_t>(actor)];
    }
    [[nodiscard]] const CharacterDraft& actor(int actor) const {
        return actors_.at(static_cast<std::size_t>(actor));
    }
    [[nodiscard]] bool both_present() const noexcept {
        return actor_present_[0] && actor_present_[1];
    }

    [[nodiscard]] std::array<bool, 6> available_choices() const noexcept;

    // CODE.1 035C hit-test result. Coordinate order follows the engine's
    // 320x400 logical surface: row (0..399), column (0..319).
    [[nodiscard]] static int hit_test(
        CharacterCreationState state, int row, int column) noexcept;

    // Consume an already-resolved CODE.1 UI event (0 cancel, 1..6 choices).
    // Returns true when the state/draft changed.
    bool apply_event(int event);

    // function_3138 completion/deletion followed by mode-7 -> mode-0 wrap.
    void finish_current_actor(bool accepted, std::string name = {});

    // function_1BD4 existing-actor keep/delete submode.
    void resolve_existing_actor(int actor, bool keep);

private:
    CharacterCreationState state_{CharacterCreationState::ActorChoice};
    CharacterDraft draft_{};
    std::array<CharacterDraft, 2> actors_{};
    std::array<bool, 2> actor_present_{{false, false}};
    bool aborted_{};
    bool completed_{};
    int class_stage_{};
    int portrait_page_{};
};

}  // namespace rloft
