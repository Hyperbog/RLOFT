#include "rloft/level22_mist_timeline.hpp"

namespace rloft {

std::vector<Level22MistCommand> Level22MistTimeline::commands() {
    using Op = Level22MistOpcode;
    std::vector<Level22MistCommand> sequence{};
    sequence.reserve(4U + 3U + 22U * 7U + 1U + 3U + 2U);

    // 21F70..21F9D: original reset, initial draw, first presentation,
    // BF30 event lock, and original CODE.2 staging path. The staging
    // path contains many successive calls to 1F1C8 and 1EA78 while
    // the source moves to (23.5,7.5) at facing 0xD5. It must be
    // lifted individually before this can be live playback.
    sequence.push_back({Op::ResetInput_19CCC,0x21F78U,-1,0U});
    sequence.push_back({Op::InitialDraw_1F1C8,0x21F82U,-1,0U});
    sequence.push_back({Op::InitialPresent_1EA78,0x21F87U,-1,0U});
    sequence.push_back({Op::SetEventLock_BF30,0x21F93U,-1,1U});
    sequence.push_back({Op::StageOriginalPose_21F9D,0x21F9DU,-1,0U});

    // 222A6..222E5: 1CD48(2,0x21,7, six -1 operands) and the
    // active effect-state wait. The number of actual iterations
    // depends on original CF4C/CF4E time/state, not a fixed timer.
    sequence.push_back({Op::StartEffect_1CD48,0x222A6U,-1,
                        AssassinRelicTransition::kEffectResource});
    sequence.push_back({Op::WaitEffectState_31F94,0x222C0U,-1,2U});

    // CODE.2 222F0..223AF: call order is source-critical.
    // 2E510(4,0x20, original_258DC+0x67+i,0), then four more
    // update/presentation calls. At i=20, 181B4(0x17) happens
    // AFTER 2E614 and BEFORE the next iteration/frame i=21.
    for (int i=0;i<kFrameCount;++i) {
        sequence.push_back({Op::FramePrepare_2E608,0x22318U,i,0U});
        sequence.push_back({Op::SelectPage_291B0,0x22325U,i,0U});
        sequence.push_back({
            Op::DrawResource_2E510,0x2233EU,i,
            static_cast<std::uint32_t>(
                kFrameResourceOffset+i)});
        sequence.push_back({Op::ComposeFrame_1EA04,0x22346U,i,0U});
        sequence.push_back({Op::PresentFrame_1EA78,0x2234BU,i,0U});
        sequence.push_back({Op::PostFrame_28168,0x22350U,i,0U});
        sequence.push_back({Op::FrameAction_2E614,0x22359U,i,9U});
        if(i==kLoadFrame) {
            sequence.push_back({
                Op::EnterLevel_181B4,0x22386U,i,
                AssassinRelicTransition::kDestinationLevel});
        }
    }

    // Original F540 order, deliberately after ALL 22 frames.
    // The Assassin's Parchment E9 is NOT consumed.
    sequence.push_back({Op::RemoveObject_F540,0x223B5U,-1,0x87U});
    sequence.push_back({Op::RemoveObject_F540,0x223C2U,-1,0x88U});
    sequence.push_back({Op::RemoveObject_F540,0x223CFU,-1,0x74U});
    sequence.push_back({Op::FinalizeEvent_19CF4,0x223D9U,-1,0U});
    sequence.push_back({Op::ClearEventLock_BF30,0x223DEU,-1,0U});
    return sequence;
}

bool Level22MistTimeline::is_frame_operation(
    Level22MistOpcode opcode) noexcept {
    return opcode>=Level22MistOpcode::FramePrepare_2E608 &&
           opcode<=Level22MistOpcode::FrameAction_2E614;
}

} // namespace rloft
