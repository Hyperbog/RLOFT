#ifdef NDEBUG
#undef NDEBUG
#endif
#include "rloft/combat_level_spell_tables.hpp"

#include <array>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <fstream>
#include <iterator>
#include <vector>

namespace {
void verify_expected(
    const rloft::PartyImage& input,
    const rloft::CombatLevelSpellProjection& observed,
    const std::array<std::uint8_t,8>& selected,
    const std::array<std::int32_t,8>& counts,
    int level, bool class3ea4) {
    auto reference=input;
    const auto base=observed.actor*rloft::PartyImage::kActorSize;
    if(class3ea4) {
        reference.bytes[base+0xC1U]=0x11U;
        if(level>=4) reference.bytes[base+0xC2U]=0x11U;
    } else reference.bytes[base+0xC2U]=0x11U;
    std::uint32_t writes=0U;
    for(std::size_t tier=0U;tier<8U;++tier) {
        for(std::int32_t slot=0;slot<counts[tier];++slot) {
            const auto at=base+0xBAU+tier*10U+
                static_cast<std::size_t>(slot);
            const auto before=reference.bytes[at];
            if(selected[tier]!=0U && (before&0xFU)==0xFU) {
                reference.bytes[at]=0U; ++writes;
            } else if(selected[tier]==0U && (before&0xFU)!=0xFU) {
                reference.bytes[at]=0xFFU; ++writes;
            }
        }
    }
    assert(observed.projected_party.bytes==reference.bytes);
    assert(observed.spell_bytes_written==writes);
}
} // namespace

int main() {
    using rloft::CombatLevelSpellTables;
    using rloft::CombatLevelSpellRoutine;
    using rloft::CombatLevelSpellModel;
    rloft::PartyImage initial{};
    CombatLevelSpellTables synthetic{};
    synthetic.group_entry_counts={{7,6,5,4,4,2,0,0}};
    synthetic.class_3de4_level_groups[0][0]=1U;
    synthetic.class_3ea4_level_groups[8][0]=1U;
    // High bits of the low-nibble-F sentinel must be treated
    // identically to 0Fh, not required to equal FFh.
    for(std::size_t group=0U;group<8U;++group)
        for(std::size_t slot=0U;slot<10U;++slot) {
            const auto at=0xBAU+group*10U+slot;
            initial.bytes[at]=(slot%3U==0U)?0x2FU:
                (slot%3U==1U)?0xFEU:0x00U;
        }
    initial.bytes[0xC1U]=0xA1U;
    initial.bytes[0xC2U]=0xA2U;

    auto result=CombatLevelSpellModel::inspect(
        initial,0U,1,CombatLevelSpellRoutine::Original3DE4,synthetic);
    assert(result);
    assert(result->projected_party.bytes[0xC2U]==0x11U);
    assert(result->projected_party.bytes[0xC1U]==0xA1U);
    assert(result->projected_party.bytes[0xBAU]==0x00U);
    assert(result->projected_party.bytes[0xBBU]==0xFEU);
    assert(result->projected_party.bytes[0xBCU]==0x00U);
    assert(result->projected_party.bytes[0xBDU]==0x00U);
    assert(result->projected_party.bytes[0xBEU]==0xFEU);
    assert(result->projected_party.bytes[0xC4U]==0xFFU);
    assert(result->projected_party.bytes[0xC5U]==0xFEU);
    assert(result->projected_party.bytes[0xC1U]==0xA1U);
    assert(initial.bytes[0xBAU]==0x2FU); // no host mutation
    verify_expected(initial,*result,synthetic.class_3de4_level_groups[0],
                    synthetic.group_entry_counts,1,false);

    result=CombatLevelSpellModel::inspect(
        initial,0U,1,CombatLevelSpellRoutine::Original3EA4,synthetic);
    assert(result && result->projected_party.bytes[0xC1U]==0x11U);
    assert(result->projected_party.bytes[0xC2U]==0xA2U);
    verify_expected(initial,*result,synthetic.class_3ea4_level_groups[0],
                    synthetic.group_entry_counts,1,true);
    result=CombatLevelSpellModel::inspect(
        initial,0U,4,CombatLevelSpellRoutine::Original3EA4,synthetic);
    assert(result && result->projected_party.bytes[0xC2U]==0x11U);
    result=CombatLevelSpellModel::inspect(
        initial,0U,9,CombatLevelSpellRoutine::Original3EA4,synthetic);
    assert(result && result->projected_party.bytes[0xBAU]==0x00U);
    assert(!CombatLevelSpellModel::inspect(
        initial,0U,0,CombatLevelSpellRoutine::Original3EA4,synthetic));
    assert(!CombatLevelSpellModel::inspect(
        initial,0U,21,CombatLevelSpellRoutine::Original3EA4,synthetic));
    assert(!CombatLevelSpellModel::inspect(
        initial,4U,1,CombatLevelSpellRoutine::Original3DE4,synthetic));
    synthetic.group_entry_counts[0]=11;
    assert(!CombatLevelSpellModel::inspect(
        initial,0U,1,CombatLevelSpellRoutine::Original3DE4,synthetic));
    assert(!CombatLevelSpellTables::from_code2({}));

    // Extended exhaustive regression gated by user-owned CODE.2.
    // The expected values are read DIRECTLY from the original table
    // spans, not from the decoded class_3de4/3ea4 model data.
    const auto* owned=std::getenv("RLOFT_TEST_CODE2");
    if(owned && *owned!='\0') {
        std::ifstream f(owned,std::ios::binary);
        assert(f.good());
        const std::vector<std::uint8_t> code(
            (std::istreambuf_iterator<char>(f)),
            std::istreambuf_iterator<char>());
        const auto tables=CombatLevelSpellTables::from_code2(code);
        assert(tables);
        assert((tables->group_entry_counts==
            std::array<std::int32_t,8>{{7,6,5,4,4,2,0,0}}));
        assert(tables->class_3de4_level_groups[0][0]==1U);
        assert(tables->class_3ea4_level_groups[0][0]==0U);
        assert(tables->class_3ea4_level_groups[8][0]==1U);
        std::uint32_t cases=0U;
        for(std::size_t actor=0U;actor<4U;++actor)
        for(int level=1;level<=20;++level)
        for(int kind=0;kind<2;++kind)
        for(int pattern=0;pattern<6;++pattern) {
            rloft::PartyImage p{};
            for(std::size_t a=0;a<p.bytes.size();++a)
                p.bytes[a]=static_cast<std::uint8_t>(
                    (a*13U+static_cast<std::size_t>(pattern)*31U)&0xFFU);
            const auto base=actor*rloft::PartyImage::kActorSize;
            for(std::size_t i=0;i<8U;++i)
                for(std::size_t j=0;j<10U;++j) {
                    const auto at=base+0xBAU+i*10U+j;
                    p.bytes[at]=((j+static_cast<std::size_t>(pattern))%3U==0U)
                        ? 0x2FU : (((j+static_cast<std::size_t>(pattern))%3U==1U)
                            ? 0xFEU:0x00U);
                }
            const bool ea4=(kind==1);
            const auto routine=ea4 ? CombatLevelSpellRoutine::Original3EA4:
                CombatLevelSpellRoutine::Original3DE4;
            const auto projection=CombatLevelSpellModel::inspect(
                p,actor,level,routine,*tables);
            assert(projection);
            std::array<std::uint8_t,8> raw{};
            const auto levelOffset=(ea4?0x6350U:0x62B0U)+
                static_cast<std::size_t>(level-1)*8U;
            for(std::size_t tier=0;tier<8;++tier)
                raw[tier]=code[0x75F24U+levelOffset+tier];
            std::array<std::int32_t,8> counts{};
            for(std::size_t tier=0U;tier<8U;++tier) {
                const auto at=0x75F24U+0x6420U+tier*4U;
                counts[tier]=static_cast<std::int32_t>(
                    static_cast<std::uint32_t>(code[at]) |
                    (static_cast<std::uint32_t>(code[at+1U])<<8U) |
                    (static_cast<std::uint32_t>(code[at+2U])<<16U) |
                    (static_cast<std::uint32_t>(code[at+3U])<<24U));
            }
            verify_expected(p,*projection,raw,counts,level,ea4);
            assert(p.bytes!=projection->projected_party.bytes);
            ++cases;
        }
        assert(cases==960U);
        auto corrupt=code;
        corrupt[0x75F24U+0x6350U+8U]^=0x01U;
        assert(!CombatLevelSpellTables::from_code2(corrupt));
    }
}
