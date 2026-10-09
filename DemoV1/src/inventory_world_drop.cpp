#include "rloft/inventory_world_drop.hpp"
#include "rloft/exploration_motion.hpp"

#include <cstddef>
#include <cstdint>
#include <limits>
#include <stdexcept>

namespace rloft {

InventoryWorldDropResult InventoryWorldDropModel::drop(
    LevelWorldState& world,
    const ExplorationSessionState& session,
    InventoryRuntimeState& inventory,
    const ObjectCatalog& objects,
    const RendererTables& tables,
    const PlacementProbe& can_place,
    bool center_fraction) {
    InventoryWorldDropResult result{};
    const auto held=inventory.held;
    if(held.object_id==-1)return result;
    if(held.object_id<0 ||
       static_cast<std::size_t>(held.object_id)>=objects.size()) {
        result.status=InventoryWorldDropStatus::InvalidInventoryObject;
        return result;
    }
    // Original 2195C calls 21880 for item 0x89 after spawning.
    // Until that state-specific side effect is modeled, reject
    // without dropping or losing the object in the cursor.
    if(held.object_id==0x89) {
        result.status=InventoryWorldDropStatus::SourceSpecialActionPending;
        return result;
    }
    if(session.level.width<=0 || session.level.height<=0 ||
       session.level.cell_words.empty() ||
       world.loose_items.size()>0x12CU) {
        result.status=InventoryWorldDropStatus::InvalidWorldPosition;
        return result;
    }

    // Original CODE.2 2195C: screen seed 0x80, subtract 0xA0,
    // IDIV 3 truncating toward zero, then facing minus quotient,
    // normalized in original modulo-360 degree units.
    constexpr std::int32_t screen_seed=0x80;
    const auto correction=(screen_seed-0xA0)/3;
    const auto angle=ExplorationMotionModel::normalize_heading(
        static_cast<std::int32_t>(session.facing)-correction);
    const auto delta=ExplorationMotionModel::direction_vector(
        0x00018000U,angle,false,tables);
    // The original inventory caller pushes DGROUP:255D4 as the
    // fifth coordinate (Y) and 255D0 as the fourth coordinate (X).
    // Preserve the actual CALL operand order.
    auto x=session.coordinate_a+delta.delta_x;
    auto y=session.coordinate_b+delta.delta_y;
    // Original C02F source mode forces both Q16 low words to
    // 0x8000 before passing the 2D5D0 placement predicate.
    if(center_fraction) {
        y=(y&0xFFFF0000U)|0x8000U;
        x=(x&0xFFFF0000U)|0x8000U;
    }
    if(can_place && !can_place(y,x,0x190U)) {
        x=session.coordinate_a;
        y=session.coordinate_b;
    }
    const auto cell_y=static_cast<std::int16_t>(y>>16U);
    const auto cell_x=static_cast<std::int16_t>(x>>16U);
    const auto cell=static_cast<std::int64_t>(cell_y)*
        session.level.width+cell_x;
    if(cell_y<0 || cell_x<0 ||
       cell_y>=session.level.height ||
       cell_x>=session.level.width ||
       cell<0 ||
       static_cast<std::uint64_t>(cell)>=
           session.level.cell_words.size() ||
       cell>std::numeric_limits<std::uint16_t>::max()) {
        result.status=InventoryWorldDropStatus::InvalidWorldPosition;
        return result;
    }

    // Model 2195C append/evict first, then clear the exact
    // original global cursor object/known byte. The new record
    // has no reused slot's unknown high packed bits.
    if(world.loose_items.size()==0x12CU) {
        world.loose_items.erase(world.loose_items.begin());
    }
    LiveObjectRecord next{};
    next.y_position_16_16=y;
    next.x_position_16_16=x;
    next.cached_cell_index=static_cast<std::uint16_t>(cell);
    next.object_type_id=static_cast<std::uint16_t>(held.object_id);
    // Original 2195C clears 0x05F8 and uses (known&1)<<10.
    next.packed_runtime_state=static_cast<std::uint16_t>(
        (held.known&1U)<<10U);
    world.loose_items.push_back(next);
    inventory.held={-1,0U};
    result.status=InventoryWorldDropStatus::Dropped;
    result.loose_item_index=world.loose_items.size()-1U;
    result.cached_cell_index=next.cached_cell_index;
    return result;
}

} // namespace rloft
