#include "rloft/combat_attack_target.hpp"

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <vector>

namespace {

rloft::ObjectCatalog fixture() {
    std::vector<std::uint8_t> bytes(4U+4U*0x41U,0U);
    bytes[0]=4U;
    bytes[4U+1U*0x41U+0x35U]=0U;
    bytes[4U+2U*0x41U+0x35U]=2U;
    bytes[4U+3U*0x41U+0x35U]=3U;
    return rloft::ObjectCatalog::parse(bytes);
}

} // namespace

int main() {
    using rloft::CombatAttackTargetModel;
    const auto objects=fixture();
    std::vector<std::uint8_t> searches;
    const auto scan=[&](std::uint8_t group)->std::int32_t {
        searches.push_back(group);
        return group==0U ? 7 : 9;
    };

    // Explicit target does NOT trigger DB94 or reorder target.
    for(std::size_t actor=0U;actor<4U;++actor) {
        searches.clear();
        auto p=CombatAttackTargetModel::choose(
            objects,actor,1,42,0U,scan);
        assert(p.target_id==42);
        assert(!p.used_db94_search);
        assert(!p.blocked_by_rear_party_rule);
        assert(searches.empty());
    }

    // No explicit target triggers DB94 with actor's LOW BIT.
    for(std::size_t actor=0U;actor<4U;++actor) {
        searches.clear();
        auto p=CombatAttackTargetModel::choose(
            objects,actor,1,-1,0U,scan);
        assert(p.target_id==(actor%2U==0U?7:9));
        assert(p.used_db94_search);
        assert(searches.size()==1U && searches[0]==actor%2U);
    }

    // E5F0 special mode FFh rejects targets only for actors2/3,
    // when the current attacking object's +35h low2 !=2 or -1.
    // Actors0/1 have no equivalent late target suppression.
    for(std::size_t actor=0U;actor<2U;++actor) {
        auto p=CombatAttackTargetModel::choose(
            objects,actor,1,14,0xFFU,scan);
        assert(p.target_id==14 && !p.blocked_by_rear_party_rule);
    }
    for(std::size_t actor=2U;actor<4U;++actor) {
        auto p=CombatAttackTargetModel::choose(
            objects,actor,1,14,0xFFU,scan);
        assert(p.target_id==-1 && p.blocked_by_rear_party_rule);
        p=CombatAttackTargetModel::choose(
            objects,actor,-1,14,0xFFU,scan);
        assert(p.target_id==-1 && p.blocked_by_rear_party_rule);
        p=CombatAttackTargetModel::choose(
            objects,actor,3,14,0xFFU,scan);
        assert(p.target_id==-1 && p.blocked_by_rear_party_rule);
        p=CombatAttackTargetModel::choose(
            objects,actor,2,14,0xFFU,scan);
        assert(p.target_id==14 && !p.blocked_by_rear_party_rule);
        p=CombatAttackTargetModel::choose(
            objects,actor,1,14,0U,scan);
        assert(p.target_id==14 && !p.blocked_by_rear_party_rule);
    }

    // The same rule follows a DB94-found target, never a missing
    // target. A missing target does not create false rejection.
    searches.clear();
    auto p=CombatAttackTargetModel::choose(
        objects,2U,1,-1,0xFFU,scan);
    assert(p.used_db94_search && searches.size()==1U);
    assert(p.target_id==-1 && p.blocked_by_rear_party_rule);
    const auto no_target=[](std::uint8_t)->std::int32_t {
        return -1;
    };
    p=CombatAttackTargetModel::choose(
        objects,2U,1,-1,0xFFU,no_target);
    assert(p.target_id==-1 && !p.blocked_by_rear_party_rule);

    bool rejected=false;
    try {
        (void)CombatAttackTargetModel::choose(
            objects,4U,1,0,0U,scan);
    } catch(const std::out_of_range&) {rejected=true;}
    assert(rejected);
    rejected=false;
    try {
        (void)CombatAttackTargetModel::choose(
            objects,0U,-2,0,0U,scan);
    } catch(const std::out_of_range&) {rejected=true;}
    assert(rejected);
    rejected=false;
    try {
        (void)CombatAttackTargetModel::choose(
            objects,0U,1,-2,0U,scan);
    } catch(const std::out_of_range&) {rejected=true;}
    assert(rejected);
}
