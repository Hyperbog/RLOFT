#ifdef NDEBUG
#undef NDEBUG
#endif
#include "rloft/inventory_party_order.hpp"

#include <array>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <stdexcept>

int main() {
    rloft::PartyImage party{};
    for(std::size_t index=0U;index<party.bytes.size();++index)
        party.bytes[index]=static_cast<std::uint8_t>(
            (index*17U+13U)&255U);
    const auto original=party.bytes;
    rloft::LevelWorldState world{};
    world.npcs.resize(3U);
    world.loose_items.resize(4U);
    world.placed_objects.resize(1U);
    world.npcs[0].packed_runtime_state=0x0001U;
    world.npcs[1].packed_runtime_state=0x0802U; // actor1
    world.npcs[2].packed_runtime_state=0x1003U; // actor2
    world.loose_items[0].packed_runtime_state=0x0009U;
    world.loose_items[1].packed_runtime_state=0x080AU;
    world.loose_items[2].packed_runtime_state=0x100BU;
    world.loose_items[3].packed_runtime_state=0x180CU; // actor3
    world.placed_objects[0].packed_runtime_state=0x100DU;
    rloft::InventoryRuntimeState inventory{};
    inventory.open_container=rloft::OpenInventoryContainer{
        1U,0U,2,1U
    };
    // Original CODE.2 12470 swaps complete 6FA0 4-byte
    // actor menu contexts, not merely the selected viewer.
    inventory.party_container_ui_contexts[1].raw=
        {{3U,1U,3U,2U}};
    inventory.party_container_ui_contexts[2].raw=
        {{0xFFU,0xFFU,0U,0xFFU}};
    const auto context_one=
        inventory.party_container_ui_contexts[1].raw;
    const auto context_two=
        inventory.party_container_ui_contexts[2].raw;
    std::array<bool,4> modes{{false,true,false,true}};
    const auto result=rloft::InventoryPartyOrderModel::exchange(
        party,world,inventory,modes,1U,2U);
    assert(result.changed);
    assert(result.npc_references_updated==2U);
    assert(result.loose_references_updated==2U);
    for(std::size_t i=0U;i<rloft::PartyImage::kActorSize;++i) {
        assert(party.bytes[rloft::PartyImage::kActorSize+i]==
               original[2U*rloft::PartyImage::kActorSize+i]);
        assert(party.bytes[2U*rloft::PartyImage::kActorSize+i]==
               original[rloft::PartyImage::kActorSize+i]);
    }
    assert(world.npcs[1].packed_runtime_state==0x1002U);
    assert(world.npcs[2].packed_runtime_state==0x0803U);
    assert(world.npcs[0].packed_runtime_state==0x0001U);
    assert(world.loose_items[1].packed_runtime_state==0x100AU);
    assert(world.loose_items[2].packed_runtime_state==0x080BU);
    assert(world.loose_items[0].packed_runtime_state==0x0009U);
    assert(world.loose_items[3].packed_runtime_state==0x180CU);
    assert(world.placed_objects[0].packed_runtime_state==0x100DU);
    assert(inventory.open_container->actor==2U);
    assert(inventory.party_container_ui_contexts[1].raw==context_two);
    assert(inventory.party_container_ui_contexts[2].raw==context_one);
    assert(modes[1]==false && modes[2]==true);
    const auto undone=rloft::InventoryPartyOrderModel::exchange(
        party,world,inventory,modes,1U,2U);
    assert(undone.changed);
    assert(party.bytes==original);
    assert(inventory.open_container->actor==1U);
    assert(inventory.party_container_ui_contexts[1].raw==context_one);
    assert(inventory.party_container_ui_contexts[2].raw==context_two);
    assert(modes[1]==true && modes[2]==false);
    assert(world.npcs[1].packed_runtime_state==0x0802U);
    assert(world.loose_items[2].packed_runtime_state==0x100BU);

    const auto no_op=rloft::InventoryPartyOrderModel::exchange(
        party,world,inventory,modes,2U,2U);
    assert(!no_op.changed);
    bool failed=false;
    try {
        (void)rloft::InventoryPartyOrderModel::exchange(
            party,world,inventory,modes,0U,4U);
    }catch(const std::out_of_range&){failed=true;}
    assert(failed);
    assert(party.bytes==original);
}
