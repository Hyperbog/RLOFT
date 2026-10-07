#include "rloft/character_creation_flow.hpp"

#include <algorithm>
#include <array>

namespace rloft {
namespace {

constexpr std::array<int, 9> kRow{
    13, 67, 91, 91, 67, 13, 91, 91, 91
};
constexpr std::array<int, 9> kColumn{
    3, 56, 109, 162, 215, 268, 83, 136, 189
};

// CODE.1 DGROUP 06D8: first class availability by race.
constexpr bool kFirstClass[6][6] = {
    {1,1,1,1,1,1},
    {1,0,1,1,1,1},
    {1,0,0,1,0,1},
    {1,0,0,1,0,1},
    {1,0,0,1,0,1},
    {1,0,1,1,1,1},
};

// CODE.1 DGROUP 06FC: second class availability by race, then
// already-selected first class, then candidate class.
constexpr bool kSecondClass[6][6][6] = {
    {
        {0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},
        {0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0},
    },
    {
        {0,0,0,1,1,1},{0,0,0,0,0,0},{0,0,0,1,0,0},
        {1,0,1,0,1,0},{1,0,0,1,0,1},{1,0,0,0,1,0},
    },
    {
        {0,0,0,0,0,1},{0,0,0,0,0,0},{0,0,0,0,0,0},
        {0,0,0,0,0,0},{0,0,0,0,0,0},{1,0,0,0,0,0},
    },
    {
        {0,0,0,1,0,1},{0,0,0,0,0,0},{0,0,0,0,0,0},
        {1,0,0,0,0,1},{0,0,0,0,0,0},{1,0,0,1,0,0},
    },
    {
        {0,0,0,1,0,1},{0,0,0,0,0,0},{0,0,0,0,0,0},
        {1,0,0,0,0,0},{0,0,0,0,0,0},{1,0,0,0,0,0},
    },
    {
        {0,0,0,0,1,1},{0,0,0,0,0,0},{0,0,0,0,0,0},
        {0,0,0,0,0,0},{1,0,0,0,0,1},{1,0,0,0,1,0},
    },
};

std::array<bool, 6> third_class(
    int race, int first, int second) noexcept {
    std::array<bool, 6> out{};

    // CODE.1 function_2198 mode 2. Only races 1 and 5 expose
    // third-class combinations in the shipped rules.
    if (race == 5) {
        if (first == 0) {
            if (second == 4) out[5] = true;
            if (second == 5) out[4] = true;
        }
        if (first == 4) {
            if (second == 0) out[5] = true;
            if (second == 5) out[0] = true;
        }
        if (first == 5 && second == 4) out[0] = true;
    } else if (race == 1) {
        if (first == 0) {
            if (second == 4) {
                out[5] = true;
                out[3] = true;
            }
            if (second == 5 || second == 3) out[4] = true;
        }
        if (first == 4) {
            if (second == 0) {
                out[5] = true;
                out[3] = true;
            }
            if (second == 5 || second == 3) out[0] = true;
        }
        if (first == 5) {
            if (second == 4) out[0] = true;
            if (second == 0) {
                out[4] = true;
                out[3] = true;
            }
        }
        if (first == 3) {
            if (second == 4) out[0] = true;
            if (second == 0) out[4] = true;
        }
    }

    return out;
}

void normalize_classes(CharacterDraft& draft) noexcept {
    auto& p = draft.classes;
    int key = p[0];

    if (p[1] == -1) {
        key = p[0];
    } else if (p[2] == -1) {
        const int a = p[0];
        const int b = p[1];

        if (a == 0) {
            if (b == 5) key = 6;
            if (b == 3) key = 7;
            if (b == 4) key = 8;
        }
        if (a == 5) {
            if (b == 0) key = 6;
            if (b == 3) key = 9;
            if (b == 4) key = 12;
        }
        if (a == 2) key = 10;
        if (a == 3) {
            if (b == 0) key = 7;
            if (b == 5) key = 9;
            if (b == 4) key = 11;
            if (b == 2) key = 10;
        }
        if (a == 4) {
            if (b == 0) key = 8;
            if (b == 3) key = 11;
            if (b == 5) key = 12;
        }
    } else {
        key = (p[0] == 5 || p[1] == 5 || p[2] == 5) ? 13 : 14;
    }

    switch (key - 6) {
        case 0:
        case 1:
        case 2:
            if (p[0] != 0) std::swap(p[0], p[1]);
            break;

        case 3:
        case 4:
        case 5:
            if (p[0] != 3) std::swap(p[0], p[1]);
            break;

        case 6:
            if (p[0] != 4) std::swap(p[0], p[1]);
            break;

        case 7:
            p = {{0, 4, 5}};
            break;

        case 8:
            p = {{0, 4, 3}};
            break;

        default:
            break;
    }
}

std::array<bool, 6> option_availability(
    const CharacterDraft& draft) noexcept {
    std::array<bool, 6> out{{true, true, true, true, true, true}};

    if (draft.classes[0] == 1) {
        out = {{true, false, false, false, false, false}};
        return out;
    }

    if (std::find(
            draft.classes.begin(),
            draft.classes.end(),
            5) != draft.classes.end()) {
        out[0] = false;
    }

    if (draft.classes[0] == 2 || draft.classes[1] == 2) {
        out[1] = false;
        out[3] = false;
        out[5] = false;
    }

    return out;
}

}  // namespace

int CharacterCreationFlow::hit_test(
    CharacterCreationState state, int row, int column) noexcept {
    int result = -1;
    const int first =
        state == CharacterCreationState::StatsReview ? 6 : 0;
    const int limit =
        state == CharacterCreationState::StatsReview ? 9 : 6;

    for (int i = first; i < limit; ++i) {
        if (row >= kRow[i] && row < kRow[i] + 0x7A &&
            column >= kColumn[i] && column < kColumn[i] + 0x30) {
            // CODE.1 035C does not break on overlap; the last match wins.
            result = i + 1;
        }
    }

    // CODE.1 035C's fixed cancel/back rectangle.
    if (row >= 0x114 && row < 0x18E &&
        column >= 0x10C && column < 0x13C) {
        result = 0;
    }

    // Modes 3 and 5 apply a second special rectangle after cancel. In mode 3
    // this is "finish class choices"; in mode 5 it advances portrait pages.
    if ((state == CharacterCreationState::ClassChoice ||
         state == CharacterCreationState::PortraitChoice) &&
        row >= 0x114 && row < 0x18E &&
        column >= 0x0D7 && column < 0x107) {
        result = 0xFF;
    }

    return result;
}

std::array<bool, 6> CharacterCreationFlow::available_choices() const noexcept {
    std::array<bool, 6> out{};

    switch (state_) {
        case CharacterCreationState::ActorChoice:
        case CharacterCreationState::SexChoice:
            out[2] = true;
            out[3] = true;
            return out;

        case CharacterCreationState::RaceChoice:
            out.fill(true);
            return out;

        case CharacterCreationState::ClassChoice:
            if (draft_.race < 0 || draft_.race >= 6) return out;
            if (class_stage_ == 0) {
                for (int i = 0; i < 6; ++i) {
                    out[i] = kFirstClass[draft_.race][i];
                }
                return out;
            }
            if (class_stage_ == 1 &&
                draft_.classes[0] >= 0 &&
                draft_.classes[0] < 6) {
                for (int i = 0; i < 6; ++i) {
                    out[i] =
                        kSecondClass[draft_.race][draft_.classes[0]][i];
                }
                return out;
            }
            if (class_stage_ == 2) {
                return third_class(
                    draft_.race,
                    draft_.classes[0],
                    draft_.classes[1]);
            }
            return out;

        case CharacterCreationState::OptionChoice:
            return option_availability(draft_);

        case CharacterCreationState::PortraitChoice: {
            out.fill(true);
            for (int i = 0; i < 6; ++i) {
                if (portrait_page_ + i == 17) out[i] = false;
            }
            return out;
        }

        case CharacterCreationState::StatsReview:
            return out;
    }

    return out;
}

bool CharacterCreationFlow::apply_event(int event) {
    switch (state_) {
        case CharacterCreationState::ActorChoice:
            if (event == 0) {
                draft_.actor = -1;
                aborted_ = true;
                return true;
            }
            if (event == 3 || event == 4) {
                const int actor = event - 3;
                if (actor_present_[static_cast<std::size_t>(actor)]) {
                    // function_1BD4 existing-actor keep/delete modal is
                    // presented by the host; do not overwrite the actor here.
                    return false;
                }

                draft_ = {};
                draft_.actor = actor;
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
                draft_.classes = {{-1, -1, -1}};
                draft_.option = -1;
                draft_.face = -1;
                class_stage_ = 0;
                state_ = CharacterCreationState::ClassChoice;
                return true;
            }
            return false;

        case CharacterCreationState::ClassChoice: {
            if (event == 0) {
                if (class_stage_ == 0) {
                    draft_.classes = {{-1, -1, -1}};
                    state_ = CharacterCreationState::RaceChoice;
                } else {
                    // CODE.1 clears all three class bytes and restarts stage 0.
                    draft_.classes = {{-1, -1, -1}};
                    class_stage_ = 0;
                }
                return true;
            }

            if (event == 0xFF) {
                if (class_stage_ == 0) return false;
                normalize_classes(draft_);
                state_ = CharacterCreationState::OptionChoice;
                return true;
            }

            if (event < 1 || event > 6) return false;
            const auto available = available_choices();
            if (!available[static_cast<std::size_t>(event - 1)]) return false;

            draft_.classes[static_cast<std::size_t>(class_stage_)] = event - 1;
            ++class_stage_;

            if (class_stage_ >= 3) {
                normalize_classes(draft_);
                state_ = CharacterCreationState::OptionChoice;
                return true;
            }

            const auto next = available_choices();
            if (std::none_of(next.begin(), next.end(), [](bool v) { return v; })) {
                normalize_classes(draft_);
                state_ = CharacterCreationState::OptionChoice;
            }
            return true;
        }

        case CharacterCreationState::OptionChoice:
            if (event == 0) {
                draft_.option = -1;
                class_stage_ = 0;
                draft_.classes = {{-1, -1, -1}};
                state_ = CharacterCreationState::ClassChoice;
                return true;
            }
            if (event >= 1 && event <= 6) {
                const auto available = available_choices();
                if (!available[static_cast<std::size_t>(event - 1)]) return false;
                draft_.option = event - 1;
                portrait_page_ = 0;
                state_ = CharacterCreationState::PortraitChoice;
                return true;
            }
            return false;

        case CharacterCreationState::PortraitChoice:
            if (event == 0) {
                draft_.face = -1;
                state_ = CharacterCreationState::OptionChoice;
                return true;
            }
            if (event == 0xFF) {
                portrait_page_ = (portrait_page_ + 6) % 18;
                return true;
            }
            if (event >= 1 && event <= 6) {
                const auto available = available_choices();
                if (!available[static_cast<std::size_t>(event - 1)]) return false;
                draft_.face = portrait_page_ + event - 1;
                state_ = CharacterCreationState::StatsReview;
                return true;
            }
            return false;

        case CharacterCreationState::StatsReview:
            if (event == 0) {
                draft_.face = -1;
                state_ = CharacterCreationState::PortraitChoice;
                return true;
            }
            // Events 7/8/9 are owned by function_40FC's mode-6 controller,
            // not the linear mode-transition state machine.
            return false;
    }

    return false;
}

void CharacterCreationFlow::finish_current_actor(
    bool accepted, std::string name) {
    const int actor = draft_.actor;
    if (actor < 0 || actor >= 2) {
        throw std::logic_error(
            "CODE.1 final decision has no selected actor");
    }

    if (accepted) {
        draft_.name = std::move(name);
        actors_[static_cast<std::size_t>(actor)] = draft_;
        actor_present_[static_cast<std::size_t>(actor)] = true;
    } else {
        actors_[static_cast<std::size_t>(actor)] = {};
        actor_present_[static_cast<std::size_t>(actor)] = false;
    }

    // function_40FC increments mode to 7; function_4978 immediately maps 7
    // back to 0 and re-enters actor choice.
    draft_ = {};
    state_ = CharacterCreationState::ActorChoice;
    class_stage_ = 0;
    portrait_page_ = 0;
    aborted_ = false;
}

}  // namespace rloft
