#ifdef NDEBUG
#undef NDEBUG
#endif

#include "rloft/level22_mist_timeline.hpp"

#include <array>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <vector>

int main() {
    using rloft::Level22MistTimeline;
    using Op=rloft::Level22MistOpcode;
    const auto actions=Level22MistTimeline::commands();

    static_assert(Level22MistTimeline::kFrameCount==22);
    static_assert(Level22MistTimeline::kLoadFrame==20);
    static_assert(Level22MistTimeline::kFrameResourceOffset==0x67);
    // 7 prelude, (22 * 7) frame calls, 1 level load, 5 epilogue.
    assert(actions.size()==167U);
    const std::array<Op,7> prelude{{
        Op::ResetInput_19CCC, Op::InitialDraw_1F1C8,
        Op::InitialPresent_1EA78,Op::SetEventLock_BF30,
        Op::StageOriginalPose_21F9D, Op::StartEffect_1CD48,
        Op::WaitEffectState_31F94,
    }};
    for(std::size_t i=0;i<prelude.size();++i) {
        assert(actions[i].opcode==prelude[i]);
        assert(actions[i].frame_index==-1);
    }
    const std::array<Op,7> original_frame{{
        Op::FramePrepare_2E608,Op::SelectPage_291B0,
        Op::DrawResource_2E510,Op::ComposeFrame_1EA04,
        Op::PresentFrame_1EA78,Op::PostFrame_28168,
        Op::FrameAction_2E614,
    }};

    std::size_t cursor=prelude.size();
    int loaded_count=0, frame_count=0;
    for(int i=0;i<Level22MistTimeline::kFrameCount;++i) {
        for(const auto expected:original_frame) {
            assert(cursor<actions.size());
            const auto& action=actions.at(cursor++);
            assert(action.opcode==expected);
            assert(Level22MistTimeline::is_frame_operation(action.opcode));
            assert(action.frame_index==i);
            if(expected==Op::DrawResource_2E510) {
                assert(action.source_offset==0x2233EU);
                assert(action.operand==
                       static_cast<std::uint32_t>(0x67+i));
                ++frame_count;
            }
            if(expected==Op::FrameAction_2E614)
                assert(action.operand==9U);
        }
        if(i==20) {
            assert(actions.at(cursor).opcode==Op::EnterLevel_181B4);
            assert(actions.at(cursor).source_offset==0x22386U);
            assert(actions.at(cursor).frame_index==20);
            assert(actions.at(cursor).operand==0x17U);
            assert(!Level22MistTimeline::is_frame_operation(
                actions.at(cursor).opcode));
            ++cursor;
            ++loaded_count;
        }
    }
    assert(loaded_count==1);
    assert(frame_count==22);

    const std::array<Op,5> original_tail{{
        Op::RemoveObject_F540,Op::RemoveObject_F540,
        Op::RemoveObject_F540,Op::FinalizeEvent_19CF4,
        Op::ClearEventLock_BF30,
    }};
    for(const auto expected:original_tail) {
        assert(cursor<actions.size());
        assert(actions.at(cursor).opcode==expected);
        assert(actions.at(cursor).frame_index==-1);
        ++cursor;
    }
    assert(cursor==actions.size());
    assert(actions.at(actions.size()-5U).operand==0x87U);
    assert(actions.at(actions.size()-4U).operand==0x88U);
    assert(actions.at(actions.size()-3U).operand==0x74U);
    assert(actions.at(actions.size()-2U).source_offset==0x223D9U);
    assert(actions.back().source_offset==0x223DEU);
    const auto end=rloft::AssassinRelicTransition::kDestinationCoordinateB;
    assert(end==0x003A8000U);
    // This is a SOURCE TRACE, not fake playback or a mini-game.
}
