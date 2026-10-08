#include "rloft/combat_visible_target.hpp"
#include "rloft/combat_target_priority.hpp"

#include <cassert>
#include <cstdint>
#include <stdexcept>
#include <vector>

int main() {
    using rloft::CombatProjectedTarget;
    using rloft::CombatVisibleTargetModel;
    using rloft::CombatTargetPriorityModel;

    // Original DACC searches draw RANK from count-1 to 0, not
    // input vector order. Actor 1 is rank 1 despite being first.
    std::vector<CombatProjectedTarget> scene{
        {0x80U,0x7ED8U,1U,0U,1U},
        {0x70U,0x7ED8U,0U,0U,2U},
        {0x81U,0x7ED8U,2U,0U,0U},
    };
    const std::vector<std::uint8_t> flags{0x10U,0x10U,0x10U};
    assert(CombatVisibleTargetModel::scan(0U,scene,flags)==0);
    assert(CombatVisibleTargetModel::scan(1U,scene,flags)==2);

    // A higher-rank actor blocked at record+8 cannot suppress the
    // lower-ranked valid target.
    scene[1].blocked_marker=1U;
    assert(CombatVisibleTargetModel::scan(0U,scene,flags)==1);
    scene[1].blocked_marker=0U;

    // CODE.2 group partition is inclusive at exactly horizontal 128.
    scene[0].horizontal=0x81U;
    assert(CombatVisibleTargetModel::scan(0U,scene,flags)==0);
    assert(CombatVisibleTargetModel::scan(1U,scene,flags)==1);
    scene[0].horizontal=0x80U;

    // Actor bit02 rejects otherwise visible ranked targets.
    auto filtered=flags;
    filtered[0]=0x12U;
    assert(CombatVisibleTargetModel::scan(0U,scene,filtered)==1);
    filtered[1]=0U; // lack of bit10 also prevents acceptance
    assert(CombatVisibleTargetModel::scan(0U,scene,filtered)==-1);

    // Vertical projection threshold (7D00h + 0x3B*8) is exact.
    scene[1].vertical=0x7ED7U;
    assert(CombatVisibleTargetModel::scan(0U,scene,flags)==1);
    scene[0].vertical=0x7ED7U;
    assert(CombatVisibleTargetModel::scan(0U,scene,flags)==-1);
    scene[0].vertical=0x7ED8U;
    scene[1].vertical=0x7ED8U;

    // DB94 is separate: it first asks preferred group, then asks
    // the other group only when that returned -1.
    assert(CombatTargetPriorityModel::choose(
        0U,[&](std::uint8_t group) {
            return CombatVisibleTargetModel::scan(group,scene,flags);
        })==0);
    assert(CombatTargetPriorityModel::choose(
        1U,[&](std::uint8_t group) {
            return CombatVisibleTargetModel::scan(group,scene,flags);
        })==2);

    bool bad=false;
    try {(void)CombatVisibleTargetModel::scan(2U,scene,flags);}
    catch(const std::out_of_range&) {bad=true;}
    assert(bad);
    auto missing_rank=scene;
    missing_rank[1].draw_rank=5U;
    bad=false;
    try {(void)CombatVisibleTargetModel::scan(0U,missing_rank,flags);}
    catch(const std::invalid_argument&) {bad=true;}
    assert(bad);
    bad=false;
    try {
        (void)CombatVisibleTargetModel::scan(
            0U,scene,std::vector<std::uint8_t>{});
    } catch(const std::out_of_range&) {bad=true;}
    assert(bad);
}
