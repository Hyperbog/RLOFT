#ifdef NDEBUG
#undef NDEBUG
#endif
#include "rloft/combat_progression_thresholds.hpp"

#include <cassert>
#include <cstdint>
#include <cstdlib>
#include <fstream>
#include <iterator>
#include <vector>

namespace {
void set16(rloft::PartyImage& p,unsigned actor,unsigned offset,
           std::uint16_t value) {
    const auto at=actor*rloft::PartyImage::kActorSize+offset;
    p.bytes[at]=static_cast<std::uint8_t>(value);
    p.bytes[at+1U]=static_cast<std::uint8_t>(value>>8U);
}
void set32(rloft::PartyImage& p,unsigned actor,unsigned offset,
           std::uint32_t value) {
    const auto at=actor*rloft::PartyImage::kActorSize+offset;
    for (unsigned i=0U;i<4U;++i)
        p.bytes[at+i]=static_cast<std::uint8_t>(value>>(8U*i));
}
}

int main() {
    using rloft::CombatAttackTargetModel;
    using rloft::CombatProgressionTables;
    using rloft::CombatXpDistributionModel;
    using rloft::CombatXpLevelModel;
    using rloft::PartyImage;
    CombatProgressionTables tables{};
    for (auto& cls:tables.thresholds_in_thousands) {
        for (std::size_t i=0U;i<cls.size();++i)
            cls[i]=static_cast<std::uint16_t>(i*2U);
    }
    tables.attack_numerator={{1U,1U,1U,2U,1U,1U}};
    tables.attack_denominator={{1U,1U,1U,3U,3U,2U}};

    PartyImage party{};
    for (unsigned a=0;a<4U;++a) {
        set16(party,a,0x10U,0x8000U);
        party.bytes[a*PartyImage::kActorSize+0x16U]=0xFFU;
        party.bytes[a*PartyImage::kActorSize+0x17U]=0xFFU;
    }
    set16(party,0U,0x10U,0U);
    set16(party,0U,0x20U,1U);
    set32(party,0U,0x26U,1999U);
    party.bytes[0x15U]=0U;
    auto xp=CombatXpDistributionModel::inspect(party,1U);
    assert(xp && xp->eligible_party_members==1U);
    auto plan=CombatXpLevelModel::inspect(party,*xp,tables);
    assert(plan && plan->has_unexecuted_level_change_services);
    const auto& lane0=plan->actors[0][0];
    assert(lane0.visited && lane0.class_code==0U);
    assert(lane0.original_level==1);
    assert(lane0.projected_experience==2000U);
    assert(lane0.table_level==2U && lane0.requires_original_level_change_services);
    assert(!plan->actors[0][1].visited && !plan->actors[1][0].visited);
    assert(party.bytes[0x20U]==1U); // read-only; original state intact

    set16(party,0U,0x20U,2U);
    xp=CombatXpDistributionModel::inspect(party,1U);
    plan=CombatXpLevelModel::inspect(party,*xp,tables);
    assert(plan && !plan->has_unexecuted_level_change_services);
    assert(!plan->actors[0][0].requires_original_level_change_services);

    // Original signed class byte FF means undefined table access.
    party.bytes[0x15U]=0xFFU;
    assert(!CombatXpLevelModel::inspect(party,*xp,tables));
    party.bytes[0x15U]=0U;

    // Multiclass count is contiguous 0..count-1, even when the
    // actual third class is set while the second is absent.
    party.bytes[0x17U]=3U;
    xp=CombatXpDistributionModel::inspect(party,1U);
    assert(xp && xp->actors[0].class_track_count==2U);
    assert(!CombatXpLevelModel::inspect(party,*xp,tables));
    party.bytes[0x16U]=3U;
    party.bytes[0x17U]=0xFFU;
    set16(party,0U,0x22U,9U);
    set32(party,0U,0x2AU,3999U);
    xp=CombatXpDistributionModel::inspect(party,1U);
    plan=CombatXpLevelModel::inspect(party,*xp,tables);
    assert(plan && plan->actors[0][1].visited);
    assert(plan->actors[0][1].projected_experience==4000U);
    assert(plan->actors[0][1].table_level==3U);

    // Reject an XP plan obtained from a different party snapshot.
    set16(party,0U,0x10U,0x4000U);
    assert(!CombatXpLevelModel::inspect(party,*xp,tables));
    set16(party,0U,0x10U,0U);

    // Original DBD4 selects strictly highest signed level; a tie
    // retains first track, even if the second class has a different
    // attack-target coefficient.
    set16(party,0U,0x20U,11U);
    set16(party,0U,0x22U,11U);
    auto target=CombatAttackTargetModel::inspect(party,0U,tables);
    assert(target && target->winning_class_track==0U);
    assert(target->winning_class_level==11);
    assert(target->stored_target_byte==9U);
    set16(party,0U,0x20U,10U);
    target=CombatAttackTargetModel::inspect(party,0U,tables);
    assert(target && target->winning_class_track==1U);
    assert(target->class_code==3U && target->stored_target_byte==14U);

    // Source uses signed 16-bit levels and returns an 8-bit
    // truncated value; it does not saturate at a modern cap.
    set16(party,0U,0x22U,0U);
    target=CombatAttackTargetModel::inspect(party,0U,tables);
    assert(target && target->stored_target_byte==10U);
    set16(party,0U,0x20U,0x7FFFU);
    target=CombatAttackTargetModel::inspect(party,0U,tables);
    assert(target && target->stored_target_byte==21U);
    set16(party,0U,0x20U,0U);
    assert(!CombatAttackTargetModel::inspect(party,0U,tables));
    assert(!CombatAttackTargetModel::inspect(party,4U,tables));

    // Caller can optionally verify the exact user-owned original
    // CODE.2 data via RLOFT_TEST_CODE2. Tests never bundle game bytes.
    const auto* original=std::getenv("RLOFT_TEST_CODE2");
    if (original!=nullptr && *original!='\0') {
        std::ifstream input(original,std::ios::binary);
        assert(input.good());
        const std::vector<std::uint8_t> code2(
            (std::istreambuf_iterator<char>(input)),
            std::istreambuf_iterator<char>());
        const auto decoded=CombatProgressionTables::from_code2(code2);
        assert(decoded);
        assert(decoded->thresholds_in_thousands[0][0]==0U);
        assert(decoded->thresholds_in_thousands[0][7]==125U);
        assert(decoded->thresholds_in_thousands[0][19]==3000U);
        assert(decoded->thresholds_in_thousands[3][1]==1U);
        assert(decoded->attack_numerator[3]==2U);
        assert(decoded->attack_denominator[3]==3U);
        auto changed=code2;
        changed[0x75F24U+0x7D9CU+2U]^=1U;
        assert(!CombatProgressionTables::from_code2(changed));
    }
    assert(!CombatProgressionTables::from_code2({}));
}
