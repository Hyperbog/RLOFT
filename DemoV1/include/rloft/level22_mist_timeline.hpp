#pragma once

#include <cstddef>
#include <cstdint>
#include <vector>

#include "rloft/assassin_relic_transition.hpp"

namespace rloft {

// Exact CODE.2 21F70..223F0 event COMMAND order, distinct from any
// modern playback engine. These records do NOT perform rendering or
// complete the original 21F70 staging loops by themselves.
enum class Level22MistOpcode : std::uint8_t {
    ResetInput_19CCC,
    InitialDraw_1F1C8,
    InitialPresent_1EA78,
    SetEventLock_BF30,
    StageOriginalPose_21F9D,
    StartEffect_1CD48,
    WaitEffectState_31F94,
    FramePrepare_2E608,
    SelectPage_291B0,
    DrawResource_2E510,
    ComposeFrame_1EA04,
    PresentFrame_1EA78,
    PostFrame_28168,
    FrameAction_2E614,
    EnterLevel_181B4,
    RemoveObject_F540,
    FinalizeEvent_19CF4,
    ClearEventLock_BF30,
};

struct Level22MistCommand final {
    Level22MistOpcode opcode{};
    // Original CODE.2 object-1 logical instruction start.
    std::uint32_t source_offset{};
    // -1 is outside the frame loop. Frame 0..21 otherwise.
    int frame_index{-1};
    // For DrawResource: signed relative to ORIGINAL mutable 258DC RES
    // frame base, not an absolute index from guessed archive metadata.
    // For EnterLevel: target level; for RemoveObject: target item ID.
    std::uint32_t operand{};
};

// SOURCE-ORDER timeline data for a future live original effect,
// compositor, audio, sprite and level-entry dispatcher. It cannot be
// substituted for original timing or scene animation.
class Level22MistTimeline final {
public:
    static constexpr int kFrameCount =
        static_cast<int>(AssassinRelicTransition::kTransitionFrameCount);
    static constexpr int kLoadFrame =
        static_cast<int>(AssassinRelicTransition::kLevelChangeFrame);
    static constexpr int kFrameResourceOffset =
        AssassinRelicTransition::kFirstFrameResourceOffset;

    [[nodiscard]] static std::vector<Level22MistCommand> commands();
    [[nodiscard]] static bool is_frame_operation(
        Level22MistOpcode opcode) noexcept;
};

} // namespace rloft
