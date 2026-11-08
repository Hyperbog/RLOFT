#include "rloft/inventory_party_order.hpp"

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <utility>

namespace rloft {
namespace {
constexpr std::uint16_t kActorBits=0x3800U;
constexpr unsigned kActorShift=11U;

bool swap_reference(
    std::uint16_t& state,std::size_t first,std::size_t second) {
    const auto current=static_cast<std::size_t>(
        (state&kActorBits)>>kActorShift);
    if(current!=first && current!=second)return false;
    const auto replacement=current==first?second:first;
    state=static_cast<std::uint16_t>(
        (state&static_cast<std::uint16_t>(~kActorBits)) |
        static_cast<std::uint16_t>((replacement&7U)<<kActorShift));
    return true;
}
} // namespace

InventoryPartyOrderResult InventoryPartyOrderModel::exchange(
    PartyImage& party,LevelWorldState& world,
    InventoryRuntimeState& inventory,
    std::array<bool,4>& panel_modes,
    std::size_t first,std::size_t second) {
    if(first>=PartyImage::kActorCount ||
       second>=PartyImage::kActorCount)
        throw std::out_of_range("CODE.2 party-order actor index");
    if(first==second)return {};
    // Always relocate *complete* actor images: item ID/known bits,
    // names, spellbooks, stats, statuses, effects and all untouched
    // original 0x248-byte state survive byte-for-byte.
    const auto left=first*PartyImage::kActorSize;
    const auto right=second*PartyImage::kActorSize;
    for(std::size_t i=0U;i<PartyImage::kActorSize;++i) {
        std::swap(party.bytes[left+i],party.bytes[right+i]);
    }
    // Original CODE.2 12470 swaps the FULL four-byte 6FA0
    // actor records together with their associated party order.
    // Never leave an active container view attached to the
    // character lane that formerly occupied the index.
    std::swap(inventory.party_container_ui_contexts[first],
              inventory.party_container_ui_contexts[second]);
    std::swap(panel_modes[first],panel_modes[second]);
    if(inventory.open_container) {
        auto& owner=inventory.open_container->actor;
        if(owner==first)owner=second;
        else if(owner==second)owner=first;
    }
    InventoryPartyOrderResult result{};
    result.changed=true;
    for(auto& npc:world.npcs) {
        if(swap_reference(
            npc.packed_runtime_state,first,second))
            ++result.npc_references_updated;
    }
    for(auto& item:world.loose_items) {
        if(swap_reference(
            item.packed_runtime_state,first,second))
            ++result.loose_references_updated;
    }
    // The original CODE.2 12470 does NOT modify family B placed
    // object actor fields here; those bytes remain untouched.
    return result;
}

} // namespace rloft
