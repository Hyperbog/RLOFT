#ifdef NDEBUG
#undef NDEBUG
#endif
#include "rloft/inventory_encumbrance.hpp"

#include <array>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <vector>

namespace {
void word(std::vector<std::uint8_t>& v,std::size_t offset,std::uint16_t w) {
    v[offset]=static_cast<std::uint8_t>(w);
    v[offset+1U]=static_cast<std::uint8_t>(w>>8U);
}
}
int main() {
    constexpr std::size_t at=0x75F24U+0x7E8CU;
    std::vector<std::uint8_t> code2(at+21U*10U,0U);
    const std::array<std::array<std::uint16_t,5>,21> original{{
        {{5,6,7,9,10}},{{10,13,16,19,25}},
        {{10,13,16,19,25}},{{20,29,38,46,55}},
        {{20,29,38,46,55}},{{35,50,65,80,90}},
        {{35,50,65,80,90}},{{40,58,76,96,110}},
        {{40,58,76,96,110}},{{45,69,93,117,140}},
        {{45,69,93,117,140}},{{55,85,115,145,170}},
        {{55,85,115,145,170}},{{70,100,130,160,195}},
        {{85,121,157,193,220}},{{110,149,188,227,255}},
        {{135,174,213,252,280}},{{160,199,238,277,305}},
        {{185,224,263,302,330}},{{235,274,313,352,380}},
        {{335,374,413,452,480}}
    }};
    for(std::size_t i=0;i<21U;++i)for(std::size_t j=0;j<5U;++j)
        word(code2,at+i*10U+j*2U,original[i][j]);
    const auto rules=rloft::InventoryEncumbranceModel::from_code2(code2);
    for(std::uint8_t str=3U;str<=23U;++str)
        assert(rules.thresholds(str)==original[str-3U]);
    assert(rules.thresholds(12U)==
           (std::array<std::uint16_t,5>{{45,69,93,117,140}}));
    assert(rloft::InventoryEncumbranceModel::effective_strength(18,0)==18);
    assert(rloft::InventoryEncumbranceModel::effective_strength(18,50)==19);
    assert(rloft::InventoryEncumbranceModel::effective_strength(18,51)==20);
    assert(rloft::InventoryEncumbranceModel::effective_strength(18,75)==20);
    assert(rloft::InventoryEncumbranceModel::effective_strength(18,76)==21);
    assert(rloft::InventoryEncumbranceModel::effective_strength(18,90)==21);
    assert(rloft::InventoryEncumbranceModel::effective_strength(18,91)==22);
    assert(rloft::InventoryEncumbranceModel::effective_strength(18,99)==22);
    assert(rloft::InventoryEncumbranceModel::effective_strength(18,100)==23);
    assert(rloft::InventoryEncumbranceModel::effective_strength(12,100)==12);

    std::vector<std::uint8_t> object_bytes(4U+2U*65U,0U);
    object_bytes[0]=2U;
    object_bytes[4U+0x30U]=34U; // held items, 17 lb
    const auto objects=rloft::ObjectCatalog::parse(object_bytes);
    rloft::PartyImage party{};
    party.bytes.fill(0xFFU);
    party.bytes[0x10U]=0U;party.bytes[0x11U]=0U;
    party.bytes[0x32U]=12U;party.bytes[0x33U]=12U;
    party.bytes[0x3EU]=0U;party.bytes[0x3FU]=0U;
    rloft::InventoryRuntimeState inv{};
    const auto snapEmpty=rules.actor(party,inv,objects,0U);
    assert(snapEmpty.maximum_pounds==140U);
    assert(snapEmpty.carried_pounds==0U);
    assert(snapEmpty.burden_rank==0U);
    party.bytes[0x40U]=0U;
    party.bytes[0x41U]=0U; // original item 0, weight34 half
    const auto screenshot=rules.actor(party,inv,objects,0U);
    assert(screenshot.maximum_pounds==140U);
    assert(screenshot.carried_pounds==17U);
    assert(screenshot.carried_half_pounds==34U);
    assert(screenshot.burden_rank==0U);
    assert(rules.actor(party,inv,objects,2U).maximum_pounds==0U);
    for(std::uint8_t strength=3U;strength<=23U;++strength) {
        party.bytes[0x32U]=strength;
        party.bytes[0x33U]=strength;
        assert(rules.actor(party,inv,objects,0U).maximum_pounds==
               rules.thresholds(strength)[4]);
    }
    bool fail=false;
    try{(void)rloft::InventoryEncumbranceModel::from_code2(
        std::vector<std::uint8_t>(at+4U,0U));}
    catch(const std::runtime_error&){fail=true;}
    assert(fail);
}
