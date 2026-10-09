#ifdef NDEBUG
#undef NDEBUG
#endif
#include "rloft/inventory_world_drop.hpp"
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <vector>

namespace {
rloft::ObjectCatalog catalog() {
    constexpr std::size_t count=160U;
    std::vector<std::uint8_t> body(4U+65U*count,0U);
    body[0]=static_cast<std::uint8_t>(count);
    return rloft::ObjectCatalog::parse(body);
}
rloft::ExplorationSessionState session() {
    rloft::ExplorationSessionState s{};
    s.coordinate_a=0x00048000U;
    s.coordinate_b=0x00048000U;
    s.facing=350U; // seed128 -> 350-(-32/3) = 360 -> 0
    s.level.width=30;
    s.level.height=15;
    s.level.cell_words.assign(450U,0U);
    return s;
}
}
int main() {
    rloft::RendererTables tables{};
    tables.sine_q16_signmag.fill(0U);
    tables.sine_q16_signmag[0]=0x00010000U; // at 0 degrees
    tables.sine_q16_signmag[90]=0U;
    auto s=session();
    auto definitions=catalog();
    rloft::LevelWorldState world{};
    rloft::InventoryRuntimeState inventory{};

    // Empty cursor does not emit phantom family-C records.
    auto result=rloft::InventoryWorldDropModel::drop(
        world,s,inventory,definitions,tables,{});
    assert(result.status==rloft::InventoryWorldDropStatus::NothingHeld);
    assert(world.loose_items.empty());

    inventory.held={50,1U};
    result=rloft::InventoryWorldDropModel::drop(
        world,s,inventory,definitions,tables,
        [](std::uint32_t y,std::uint32_t x,std::uint32_t radius) {
            assert(radius==0x190U);
            assert(y==0x00048000U);
            assert(x==0x00030000U);
            return true;
        });
    assert(result.status==rloft::InventoryWorldDropStatus::Dropped);
    assert(world.loose_items.size()==1U);
    assert(inventory.held.object_id==-1);
    assert(inventory.held.known==0U);
    assert(world.loose_items[0].object_type_id==50U);
    assert(world.loose_items[0].x_position_16_16==0x00030000U);
    assert(world.loose_items[0].y_position_16_16==0x00048000U);
    assert(world.loose_items[0].cached_cell_index==123U);
    assert(result.cached_cell_index==123U);
    assert(world.loose_items[0].packed_runtime_state==0x0400U);

    // Failed original 2D5D0 placement reverts to the actual player
    // coordinates BEFORE computing cached cell index.
    inventory.held={51,0U};
    result=rloft::InventoryWorldDropModel::drop(
        world,s,inventory,definitions,tables,
        [](std::uint32_t,std::uint32_t,std::uint32_t r) {
            assert(r==0x190U);return false;
        });
    assert(result.status==rloft::InventoryWorldDropStatus::Dropped);
    assert(world.loose_items.size()==2U);
    assert(world.loose_items.back().x_position_16_16==0x00048000U);
    assert(world.loose_items.back().y_position_16_16==0x00048000U);
    assert(world.loose_items.back().cached_cell_index==124U);
    assert(world.loose_items.back().packed_runtime_state==0U);

    // Original C02F grid-center mode coerces only the low Q16 words;
    // it does not round, clamp or move the actor's fallback position.
    s.coordinate_a=0x00041234U;
    s.coordinate_b=0x00045678U;
    inventory.held={52,1U};
    result=rloft::InventoryWorldDropModel::drop(
        world,s,inventory,definitions,tables,
        [](std::uint32_t y,std::uint32_t x,std::uint32_t) {
            assert((y&0xFFFFU)==0x8000U);
            assert((x&0xFFFFU)==0x8000U);
            return false;
        },true);
    assert(result.status==rloft::InventoryWorldDropStatus::Dropped);
    assert(world.loose_items.back().x_position_16_16==s.coordinate_a);
    assert(world.loose_items.back().y_position_16_16==s.coordinate_b);

    // 2195C discards the first family-C record at the exact 300
    // record ceiling, not at 299, and appends the new record last.
    world.loose_items.assign(300U,rloft::LiveObjectRecord{});
    world.loose_items.front().object_type_id=65U;
    world.loose_items[1U].object_type_id=66U;
    inventory.held={53,1U};
    result=rloft::InventoryWorldDropModel::drop(
        world,s,inventory,definitions,tables,
        [](std::uint32_t,std::uint32_t,std::uint32_t){return false;});
    assert(result.status==rloft::InventoryWorldDropStatus::Dropped);
    assert(world.loose_items.size()==300U);
    assert(world.loose_items.front().object_type_id==66U);
    assert(world.loose_items.back().object_type_id==53U);
    assert(result.loose_item_index==299U);

    // Original item 0x89 is a special-script trigger inside 2195C;
    // it cannot be silently dropped without the associated callback.
    inventory.held={0x89,1U};
    const auto before_world=world.loose_items;
    result=rloft::InventoryWorldDropModel::drop(
        world,s,inventory,definitions,tables,{});
    assert(result.status==
        rloft::InventoryWorldDropStatus::SourceSpecialActionPending);
    assert(world.loose_items.size()==before_world.size());
    assert(inventory.held.object_id==0x89);

    // Invalid/outside-level and unknown objects must be rejected without
    // consuming the cursor or discarding the oldest world object.
    inventory.held={310,1U};
    result=rloft::InventoryWorldDropModel::drop(
        world,s,inventory,definitions,tables,{});
    assert(result.status==
        rloft::InventoryWorldDropStatus::InvalidInventoryObject);
    assert(world.loose_items.size()==300U);
    s.coordinate_a=0x00000000U;
    s.coordinate_b=0x00000000U;
    inventory.held={54,0U};
    result=rloft::InventoryWorldDropModel::drop(
        world,s,inventory,definitions,tables,{});
    assert(result.status==
        rloft::InventoryWorldDropStatus::InvalidWorldPosition);
    assert(inventory.held.object_id==54);
    assert(world.loose_items.size()==300U);
}
