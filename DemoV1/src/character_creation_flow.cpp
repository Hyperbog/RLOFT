#include "rloft/character_creation_flow.hpp"

#include <array>

namespace rloft {
namespace {

constexpr std::array<int, 9> kRow{
    13, 67, 91, 91, 67, 13, 91, 91, 91
};
constexpr std::array<int, 9> kColumn{
    3, 56, 109, 162, 215, 268, 83, 136, 189
};

}  // namespace

int CharacterCreationFlow::hit_test(
    CharacterCreationState state, int row, int column) noexcept {
    // CODE.1 035C scans slots 0..5 for modes 0..5. Each slot is the
    // engine's 0x7A x 0x30 row/column rectangle.
    for (int i = 0; i < 6; ++i) {
        if (row >= kRow[i] && row < kRow[i] + 0x7A &&
            column >= kColumn[i] && column < kColumn[i] + 0x30) {
            const int event = i + 1;

            // State 0 only exposes the two actor cards (events 3/4).
            if (state == CharacterCreationState::ActorChoice &&
                event != 3 && event != 4) {
                continue;
            }

            // State 1 only exposes sex choices (events 3/4).
            if (state == CharacterCreationState::SexChoice &&
                event != 3 && event != 4) {
                continue;
            }

            return event;
        }
    }

    // CODE.1 035C's fixed cancel/back rectangle.
    if (row >= 0x114 && row < 0x18E &&
        column >= 0x10C && column < 0x13C) {
        return 0;
    }

    return -1;
}

bool CharacterCreationFlow::apply_event(int event) {
    switch (state_) {
        case CharacterCreationState::ActorChoice:
            if (event == 0) {
                draft_.actor = -1;
                return true;
            }
            if (event == 3 || event == 4) {
                draft_.actor = event - 3;
                draft_.sex = -1;
                draft_.race = -1;
                state_ = CharacterCreationState::SexChoice;
                return true;
            }
            return false;

        case CharacterCreationState::SexChoice:
            if (event == 0) {
                draft_.sex = -1;
                state_ = CharacterCreationState::ActorChoice;
                return true;
            }
            if (event == 3 || event == 4) {
                draft_.sex = event - 3;
                draft_.race = -1;
                state_ = CharacterCreationState::RaceChoice;
                return true;
            }
            return false;

        case CharacterCreationState::RaceChoice:
            if (event == 0) {
                draft_.race = -1;
                state_ = CharacterCreationState::SexChoice;
                return true;
            }
            if (event >= 1 && event <= 6) {
                draft_.race = event - 1;
                // CODE.1 advances 04D0 here; Demo v1 deliberately stops at
                // the next unimplemented controller state rather than invent it.
                return true;
            }
            return false;
    }
    return false;
}

}  // namespace rloft
