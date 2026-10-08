#ifdef NDEBUG
#undef NDEBUG  // Assertions run in Release CTest as well.
#endif
#include "rloft/combat_level_up_hp.hpp"
#include "rloft/original_random.hpp"

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <fstream>
#include <iterator>
#include <vector>

namespace {
void put16(rloft::PartyImage& party,unsigned at,std::uint16_t value) {
    party.bytes[at]=static_cast<std::uint8_t>(value);
    party.bytes[at+1U]=static_cast<std::uint8_t>(value>>8U);
}
}

int main() {
    using rloft::CombatLevelUpHpTables;
    using rloft::CombatLevelUpHpModel;
    using rloft::OriginalRandom;
    rloft::PartyImage party{};
    CombatLevelUpHpTables tables{};
    tables.die_sides={{10U,10U,10U,8U,4U,6U}};
    for(auto& group:tables.constitution_bonus) group.fill(0);
    // These values are verified in the original DGROUP:67FA table.
    tables.constitution_bonus[0][15]=1;
    tables.constitution_bonus[0][1]=-3;
    tables.constitution_bonus[1][15]=1;
    tables.constitution_bonus[1][1]=-3;
    party.bytes[0x37U]=15U;
    party.bytes[0x15U]=0U;
    party.bytes[0x16U]=0xFFU;
    party.bytes[0x17U]=0xFFU;
    put16(party,0x1CU,0xFFFD);
    put16(party,0x1EU,0xFFFF);

    const auto hp=[&](int level) {
        return CombatLevelUpHpModel::inspect(party,0U,0U,level,1U,tables);
    };
    // Original first 30CA2 draw is 16838: 1d10 = 6.
    auto outcome=hp(9);
    assert(outcome && outcome->original_die_roll==6U);
    assert(outcome->final_hp_increase==7);
    assert(outcome->current_hp_after==4U);
    assert(outcome->max_hp_after==6U);
    OriginalRandom control(1U);
    (void)control.next15();
    assert(outcome->rng_state_after==control.state());
    // Level 10 removes the die from the selected class group,
    // BUT the draw already occurred before that branch.
    outcome=hp(10);
    assert(outcome && outcome->final_hp_increase==4);
    assert(outcome->rng_state_after==control.state());
    assert(party.bytes[0x1CU]==0xFDU); // source remains unchanged

    // Same jump-table destination for class 1 and 2.
    for(std::uint8_t class_id:{1U,2U}) {
        party.bytes[0x15U]=class_id;
        outcome=hp(9);
        assert(outcome && outcome->final_hp_increase==7);
        outcome=hp(10);
        assert(outcome && outcome->final_hp_increase==4);
    }

    // Class 3: d8 below level 10, con+2 from level 10.
    party.bytes[0x15U]=3U;
    outcome=hp(9);
    assert(outcome && outcome->original_die_roll==5U);
    assert(outcome->final_hp_increase==6);
    outcome=hp(10);
    assert(outcome && outcome->final_hp_increase==3);

    // Class 4: d4 until level 10, con+1 beginning level 11.
    party.bytes[0x15U]=4U;
    outcome=hp(10);
    assert(outcome && outcome->original_die_roll==3U);
    assert(outcome->final_hp_increase==4);
    outcome=hp(11);
    assert(outcome && outcome->final_hp_increase==2);

    // Class 5: d6 until level 10, con+2 beginning level 11.
    party.bytes[0x15U]=5U;
    outcome=hp(10);
    assert(outcome && outcome->original_die_roll==4U);
    assert(outcome->final_hp_increase==5);
    outcome=hp(11);
    assert(outcome && outcome->final_hp_increase==3);

    // Multiclass divisor occurs after min-one; for three classes,
    // the final HP increase can be ZERO (do not modernize to >=1).
    party.bytes[0x15U]=4U;
    party.bytes[0x37U]=1U;
    party.bytes[0x16U]=1U;
    party.bytes[0x17U]=0xFFU;
    outcome=hp(9);
    assert(outcome && outcome->final_hp_increase==0);
    party.bytes[0x37U]=15U;
    party.bytes[0x15U]=0U;
    outcome=hp(9);
    assert(outcome && outcome->final_hp_increase==3);
    party.bytes[0x17U]=2U;
    outcome=hp(9);
    assert(outcome && outcome->final_hp_increase==2);
    // Original tests second class byte first, so third without
    // second is not a 2-class divisor.
    party.bytes[0x16U]=0xFFU;
    outcome=hp(9);
    assert(outcome && outcome->final_hp_increase==7);

    // Unsupported original-memory indexes must never synthesize HP.
    party.bytes[0x15U]=0xFFU;
    assert(!hp(9));
    party.bytes[0x15U]=0U;
    party.bytes[0x37U]=19U;
    assert(!hp(9));
    party.bytes[0x37U]=15U;
    assert(!CombatLevelUpHpModel::inspect(party,4U,0U,9,1U,tables));
    assert(!CombatLevelUpHpModel::inspect(party,0U,3U,9,1U,tables));
    tables.die_sides[0U]=0U;
    assert(!hp(9));

    // Optional exact original-file data check; never ship DOS assets.
    const auto* owned=std::getenv("RLOFT_TEST_CODE2");
    if(owned!=nullptr && *owned!='\0') {
        std::ifstream f(owned,std::ios::binary);
        assert(f.good());
        const std::vector<std::uint8_t> code(
            (std::istreambuf_iterator<char>(f)),
             std::istreambuf_iterator<char>());
        const auto loaded=CombatLevelUpHpTables::from_code2(code);
        assert(loaded);
        assert(loaded->die_sides[0]==10U);
        assert(loaded->die_sides[3]==8U);
        assert(loaded->die_sides[4]==4U);
        assert(loaded->die_sides[5]==6U);
        assert(loaded->constitution_bonus[0][15]==1);
        assert(loaded->constitution_bonus[1][15]==1);
        auto altered=code;
        altered[0x75F24U+0x67FAU+15U]^=1U;
        assert(!CombatLevelUpHpTables::from_code2(altered));
        // Byte-driven differential regression. The expected branch comes
        // from the ORIGINAL six-way CS jump table at logical DC64,
        // while the C++ model uses its independently audited dispatch.
        // This runs only when the caller provides owned CODE.2 bytes.
        const auto cs_jump=[&](int cls) {
            const auto at=0x3AF24U+0xDC64U+
                static_cast<std::size_t>(cls)*4U;
            return static_cast<std::uint32_t>(code[at]) |
                (static_cast<std::uint32_t>(code[at+1U])<<8U) |
                (static_cast<std::uint32_t>(code[at+2U])<<16U) |
                (static_cast<std::uint32_t>(code[at+3U])<<24U);
        };
        std::uint32_t matrix_cases=0U;
        for (int actor:{0,3})
        for (int cls=0;cls<6;++cls)
        for (int con=0;con<19;++con)
        for (int level:{0,1,9,10,11,20})
        for (int multiclass=0;multiclass<3;++multiclass)
        for (std::uint32_t seed:{1U,42U,0U,0xFFFFFFFFU}) {
            rloft::PartyImage snap{};
            const auto base=static_cast<std::size_t>(actor)*
                rloft::PartyImage::kActorSize;
            snap.bytes[base+0x15U]=static_cast<std::uint8_t>(cls);
            snap.bytes[base+0x16U]=multiclass>=1?1U:0xFFU;
            snap.bytes[base+0x17U]=multiclass==2?2U:0xFFU;
            snap.bytes[base+0x37U]=static_cast<std::uint8_t>(con);
            snap.bytes[base+0x1CU]=0xFDU;
            snap.bytes[base+0x1DU]=0xFFU;
            snap.bytes[base+0x1EU]=0xFFU;
            snap.bytes[base+0x1FU]=0xFFU;
            const auto result=CombatLevelUpHpModel::inspect(
                snap,static_cast<std::size_t>(actor),0U,
                level,seed,*loaded);
            assert(result);
            const auto next=seed*0x41C64E6DU+0x3039U;
            const auto random15=(next>>16U)&0x7FFFU;
            const auto original_sides=code[
                0x75F24U+0x6821U+static_cast<std::size_t>(cls)];
            const auto die=static_cast<std::int32_t>(
                (random15*original_sides)>>15U)+1;
            const auto con_raw=code[0x75F24U+0x67FAU+
                static_cast<std::size_t>(cls/3*19+con)];
            const auto bonus=static_cast<std::int32_t>(
                con_raw<=127U ? static_cast<int>(con_raw):
                                static_cast<int>(con_raw)-256);
            std::int32_t gain=0;
            const auto jump=cs_jump(cls);
            if (jump==0xDD06U) gain=level<10?die+bonus:bonus+3;
            else if (jump==0xDD27U) gain=level<10?die+bonus:bonus+2;
            else if (jump==0xDD17U) gain=level<11?die+bonus:bonus+1;
            else if (jump==0xDD32U) gain=level<11?die+bonus:bonus+2;
            else assert(false && "unexpected original CS jump target");
            if (gain<1) gain=1;
            if (multiclass!=0) gain/=multiclass==1?2:3;
            assert(result->original_die_roll==
                   static_cast<std::uint16_t>(die));
            assert(result->final_hp_increase==gain);
            assert(result->current_hp_after==
                static_cast<std::uint16_t>(0xFFFDU+gain));
            assert(result->max_hp_after==
                static_cast<std::uint16_t>(0xFFFFU+gain));
            assert(result->rng_state_after==next);
            ++matrix_cases;
        }
        assert(matrix_cases==16416U);
    }
    assert(!CombatLevelUpHpTables::from_code2({}));
}
