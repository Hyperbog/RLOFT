#include "rloft/inventory_encumbrance.hpp"

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <stdexcept>

namespace rloft {
namespace {
constexpr std::size_t kDataAt=0x75F24U+0x7E8CU;
constexpr std::size_t kRows=21U;
constexpr std::size_t kEntries=5U;

std::uint16_t u16(const std::uint8_t* p) noexcept {
    return static_cast<std::uint16_t>(p[0]) |
        (static_cast<std::uint16_t>(p[1])<<8U);
}
} // namespace

InventoryEncumbranceModel InventoryEncumbranceModel::from_code2(
    const std::vector<std::uint8_t>& original_code2) {
    if(original_code2.size()<kDataAt+kRows*kEntries*2U)
        throw std::runtime_error(
            "original CODE.2 encumbrance 7E8C table truncated");
    InventoryEncumbranceModel result{};
    for(std::size_t row=0U;row<kRows;++row)
        for(std::size_t n=0U;n<kEntries;++n)
            result.thresholds_[row][n]=u16(
                original_code2.data()+kDataAt+row*10U+n*2U);
    return result;
}

std::uint8_t InventoryEncumbranceModel::effective_strength(
    std::uint8_t strength,std::uint8_t exceptional) noexcept {
    // CODE.2 0FE44 changes ranks for *current* Strength 18 only.
    // Max burden 0FDE4 applies exceptional bands to base Strength
    // unconditionally, but a legal character only has exceptional
    // strength at 18. Use the original rank branch here.
    unsigned effective=strength;
    if(strength==18U) {
        if(exceptional>=1U && exceptional<=50U)++effective;
        else if(exceptional<=75U && exceptional>=51U)effective+=2U;
        else if(exceptional<=90U && exceptional>=76U)effective+=3U;
        else if(exceptional<=99U && exceptional>=91U)effective+=4U;
        else if(exceptional==100U)effective+=5U;
    }
    return static_cast<std::uint8_t>(effective);
}

std::array<std::uint16_t,5>
InventoryEncumbranceModel::thresholds(
    std::uint8_t strength) const {
    if(strength<3U || strength>23U)
        throw std::out_of_range(
            "original CODE.2 7E8C encumbrance strength out of range");
    return thresholds_[strength-3U];
}

InventoryEncumbranceSnapshot InventoryEncumbranceModel::actor(
    const PartyImage& party,
    const InventoryRuntimeState& inventory,
    const ObjectCatalog& objects,
    std::size_t actor_index) const {
    if(actor_index>=PartyImage::kActorCount)
        throw std::out_of_range("original actor burden index");
    if(!party.active(actor_index))
        return {};
    const auto base=actor_index*PartyImage::kActorSize;
    const auto str_current=party.bytes[base+0x33U];
    const auto exceptional_current=party.bytes[base+0x3FU];
    const auto str_base=party.bytes[base+0x32U];
    const auto exceptional_base=party.bytes[base+0x3EU];
    const auto current_effective=effective_strength(
        str_current,exceptional_current);
    const auto maximum_effective=effective_strength(
        str_base,exceptional_base);
    auto values=thresholds(current_effective);
    const auto maxvalues=thresholds(maximum_effective);
    const auto half=inventory.actor_weight_half_units(
        party,objects,actor_index);
    // CODE.2 0FD84 returns integer pounds rounded toward zero.
    const auto pounds=half/2U;
    std::uint8_t rank=4U;
    for(std::uint8_t i=0U;i<5U;++i) {
        if(pounds<=values[i]) {rank=i;break;}
    }
    return InventoryEncumbranceSnapshot{
        pounds,half,maxvalues[4],values,rank,current_effective
    };
}

} // namespace rloft
