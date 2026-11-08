#ifdef NDEBUG
#undef NDEBUG
#endif
#include "rloft/original_mist_staging.hpp"
#include <cassert>
#include <cstdint>
#include <stdexcept>

namespace {
std::uint32_t fixed(unsigned cell,unsigned fraction=0x8000U) {
    return (cell<<16U)|fraction;
}
void reached(const rloft::MistStageTrace& t) {
    assert(t.final_pose.coordinate_a==fixed(23));
    assert(t.final_pose.coordinate_b==fixed(7));
    assert(t.final_pose.facing==213U);
    for(const auto& f:t.draws) {
        assert(f.pose.facing<360U);
        assert(f.draw_call_offset!=0U);
        assert(f.present_call_offset!=0U);
    }
}
}
int main() {
    using rloft::OriginalMistStagingModel;
    using rloft::MistStagePhase;
    const auto fresh=OriginalMistStagingModel::trace(
        {fixed(4),fixed(4),270U});
    reached(fresh);
    // Original Level22 fresh pose: 19 + 27 + 3 + 14 draw pairs.
    assert(fresh.draws.size()==63U);
    assert(fresh.draws.front().phase==MistStagePhase::MoveColumn);
    assert(fresh.draws.front().pose.coordinate_a==fixed(5));
    assert(fresh.draws.at(18U).pose.coordinate_a==fixed(23));
    assert(fresh.draws.at(19U).pose.facing==260U);
    assert(fresh.draws.at(45U).pose.facing==0U);
    assert(fresh.draws.at(46U).pose.coordinate_b==fixed(5));
    assert(fresh.draws.back().phase==MistStagePhase::FinalTurn);

    const auto centered=OriginalMistStagingModel::trace(
        {fixed(23,0x1234),fixed(7,0x4567),213U});
    reached(centered);
    // Equal-column/row branch STILL turns 270->0->213.
    assert(centered.draws.size()==48U);
    assert(centered.draws.front().pose.facing==223U);
    assert(centered.draws.at(6U).pose.facing==270U);

    const auto upper=OriginalMistStagingModel::trace(
        {fixed(29,0),fixed(14,0),180U});
    reached(upper);
    unsigned dx=0U,dy=0U;
    bool col_turn=false,row_turn=false;
    for(const auto& f:upper.draws) {
        if(f.phase==MistStagePhase::TurnColumn) col_turn=true;
        if(f.phase==MistStagePhase::TurnRow) row_turn=true;
        if(f.phase==MistStagePhase::MoveColumn) ++dx;
        if(f.phase==MistStagePhase::MoveRow) ++dy;
    }
    assert(dx==6U && dy==7U && col_turn && row_turn);

    // CODE.2 22090 chooses 275 immediately when heading<90.
    const auto asymmetric=OriginalMistStagingModel::trace(
        {fixed(23),fixed(7),1U});
    reached(asymmetric);
    assert(asymmetric.draws.front().pose.facing==275U);

    for(unsigned h=0U;h<360U;h+=7U) {
        reached(OriginalMistStagingModel::trace(
            {fixed(0,0x1111),fixed(0,0x2222),h}));
        reached(OriginalMistStagingModel::trace(
            {fixed(29,0x3333),fixed(14,0x4444),h}));
    }
    bool rejected=false;
    try { (void)OriginalMistStagingModel::trace(
        {fixed(30),fixed(7),270U}); }
    catch (const std::out_of_range&) { rejected=true; }
    assert(rejected);
    rejected=false;
    try { (void)OriginalMistStagingModel::trace(
        {fixed(22),fixed(15),270U}); }
    catch (const std::out_of_range&) { rejected=true; }
    assert(rejected);
    rejected=false;
    try { (void)OriginalMistStagingModel::trace(
        {fixed(22),fixed(7),360U}); }
    catch (const std::out_of_range&) { rejected=true; }
    assert(rejected);
}
