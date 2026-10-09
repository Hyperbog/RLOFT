#include "rloft/inventory_world_drop.hpp"
#include "rloft/exploration_motion.hpp"

#include <cstddef>
#include <cstdint>
#include <limits>
#include <stdexcept>

namespace rloft {

namespace {

// 1E8C8 prefers an active group-1 record; otherwise it scans
// group-2. Its group-1 sentinel can be type zero with bit 0x10
// set, while group-2 terminates at the first zero type.
const CellControlRecord* source_control_at(
    const LevelState& level,int row,int col) noexcept {
    for(const auto& rec:level.group1) {
        if(rec.kind()==0U && (rec.control()&0x10U)==0U)break;
        if(rec.row()==row && rec.column()==col)return &rec;
    }
    for(const auto& rec:level.group2) {
        if(rec.kind()==0U)break;
        if(rec.row()==row && rec.column()==col)return &rec;
    }
    return nullptr;
}

// Original 1E930 independently reads the group-2 position byte
// +6, returning 0x8000 for a missing record.
std::uint32_t source_dynamic_position(
    const LevelState& level,int row,int col) noexcept {
    for(const auto& rec:level.group2) {
        if(rec.kind()==0U)break;
        if(rec.row()==row && rec.column()==col)
            return static_cast<std::uint32_t>(rec.raw[6])<<12U;
    }
    return 0x8000U;
}

bool source_unsigned_overlap(
    std::uint32_t coordinate,std::uint32_t center,
    std::uint32_t half_extent) noexcept {
    return coordinate>=center-half_extent &&
           coordinate<=center+half_extent;
}
} // namespace

bool InventoryWorldDropModel::placement_passable_2d5d0(
    const ExplorationSessionState& session,
    const LevelWorldState& world,
    const NpcCatalog& npcs,
    std::uint32_t y,std::uint32_t x,
    std::uint32_t actor_mode) {
    // Original CODE.2 2D5D0 uses 0x190 as an ACTOR INDEX / MODE.
    // Its rectangle is fixed at +/-0x5800 Q16 (2D5E2..2D629),
    // regardless of the actor index. This is the mode used by 2195C.
    if(actor_mode!=0x190U || session.level.width<=0 ||
       session.level.height<=0 ||
       session.level.cell_words.empty())return false;
    constexpr std::uint32_t kHalfExtent=0x5800U;
    const auto signed_cell=[](std::uint32_t coord)->int {
        const auto hi=static_cast<std::uint16_t>(coord>>16U);
        return hi<=0x7FFFU ? static_cast<int>(hi) :
               static_cast<int>(hi)-0x10000;
    };
    const int low_row=signed_cell(y-kHalfExtent);
    const int high_row=signed_cell(y+kHalfExtent);
    const int low_col=signed_cell(x-kHalfExtent);
    const int high_col=signed_cell(x+kHalfExtent);
    const auto& level=session.level;

    // 2D647..2D93F: scan inclusive square; off-map cells are
    // skipped by the original instructions, not treated as walls.
    // F048 scripted early-exit is not active in a fresh game and
    // remains unmodeled for mutable save/script state.
    for(int col=low_col;col<=high_col;++col) {
        for(int row=low_row;row<=high_row;++row) {
            if(row<0 || col<0 || row>=level.height ||
               col>=level.width)continue;
            const auto index=static_cast<std::size_t>(row)*
                static_cast<std::size_t>(level.width)+
                static_cast<std::size_t>(col);
            if(index>=level.cell_words.size())return false;
            const auto word=level.cell_words[index];
            const CellControlRecord* record=nullptr;
            if((word&0x0080U)!=0U || (word&0x1000U)!=0U)
                record=source_control_at(level,row,col);

            // 2D6DC..2D811: dynamic record, its group-2
            // position/status, and the mode-0x190 conservative
            // blocked branch; type-8 record is explicitly skipped.
            if((word&0x0080U)!=0U &&
               source_dynamic_position(level,row,col)!=0U &&
               record!=nullptr && record->kind()!=8U)
                return false;

            // 2D811..2D93A: bit0x0800 blocks mode 0x190;
            // bit0x1000 allows missing/type1 controls, blocks
            // type8 and other records. Otherwise all low-six
            // nonzero geometry codes block.
            if((word&0x003FU)!=0U) {
                if((word&0x0800U)!=0U)return false;
                if((word&0x1000U)!=0U) {
                    if(record && record->kind()!=1U)return false;
                } else return false;
            }
        }
    }

    // 2D94E..2D971 calls 2C800 even for actor mode 0x190.
    // Exclude source NPC +0x0B bit 02; compare each original
    // unsigned Q16 rectangle with NPCS +0x16 plus 0x3E8.
    for(const auto& npc:world.npcs) {
        if((npc.runtime_flags&0x02000000U)!=0U)continue;
        const auto extent=static_cast<std::uint32_t>(
            npcs.at(npc.npc_type_id).collision_half_extent)+0x3E8U;
        if(source_unsigned_overlap(x,npc.x_position_16_16,extent) &&
           source_unsigned_overlap(y,npc.y_position_16_16,extent))
            return false;
    }

    // 2D972 skips placed-object 2CC88 only in mode 0x190.
    // 2D9B0..2D9D5 still invokes 2C9DC: the player's own
    // +/-0x8FA0 Q16 collision square independently vetoes
    // a forward position, causing 2195C to fall back to
    // the actual player coordinates.
    if(source_unsigned_overlap(x,session.coordinate_a,0x8FA0U) &&
       source_unsigned_overlap(y,session.coordinate_b,0x8FA0U))
        return false;
    return true;
}


Level16SeedRitualCheck Level16SeedRitualModel::evaluate_21880(
    const ExplorationSessionState& session,
    const LevelWorldState& world,
    const LiveObjectRecord* candidate,
    bool evict_oldest) noexcept {
    Level16SeedRitualCheck result{};
    if(session.level_id!=16U) {
        result.status=Level16SeedRitualStatus::NotLevel16;
        return result;
    }
    // Original 2189F calls 1C1DC(0x44) and returns early
    // whenever that bit is already set in persistent 0x22200.
    constexpr auto byte=Level16SeedRitualCheck::kEventBit/8U;
    constexpr auto mask=static_cast<std::uint8_t>(
        1U<<(Level16SeedRitualCheck::kEventBit%8U));
    if((session.persistent_event_flags[byte]&mask)!=0U) {
        result.status=Level16SeedRitualStatus::AlreadyFlagged;
        return result;
    }
    const auto add=[&](const LiveObjectRecord& record) {
        if(record.object_type_id!=kSeedObjectId)return;
        // 218DB..218F7: cmp source unsigned XY byte against
        // WORD at live 0x17618+6 (x high word) and +2 (y high
        // word). Low fixed-point fractions do NOT matter.
        const auto x=static_cast<std::uint16_t>(
            record.x_position_16_16>>16U);
        const auto y=static_cast<std::uint16_t>(
            record.y_position_16_16>>16U);
        for(std::size_t i=0U;i<kSourceCells.size();++i)
            if(x==kSourceCells[i][0] &&
               y==kSourceCells[i][1])
                result.present[i]=true;
    };
    // 2195C evicts the oldest record before appending when
    // the source array reaches 0x12C entries. The pure check
    // must audit the POST-eviction state, not the old source.
    for(std::size_t i=evict_oldest?1U:0U;
        i<world.loose_items.size();++i)
        add(world.loose_items[i]);
    if(candidate)add(*candidate);
    bool complete=true;
    for(const bool present:result.present)
        if(!present)complete=false;
    result.status=complete
        ? Level16SeedRitualStatus::ReadyForSourceActions
        : Level16SeedRitualStatus::MissingSeed;
    return result;
}

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
    LiveObjectRecord next{};
    next.y_position_16_16=y;
    next.x_position_16_16=x;
    next.cached_cell_index=static_cast<std::uint16_t>(cell);
    next.object_type_id=static_cast<std::uint16_t>(held.object_id);
    // Original 2195C clears 0x05F8 and uses (known&1)<<10.
    next.packed_runtime_state=static_cast<std::uint16_t>(
        (held.known&1U)<<10U);
    // 2195C invokes 21880 only AFTER creating item 0x89.
    // Unfinished source follow-on calls 13EF8(8) and
    // 15050(0x73) must not be skipped when the fourth site
    // becomes occupied. Preflight the would-be appended
    // record; permit the other Level16 seed drops normally.
    if(held.object_id==Level16SeedRitualModel::kSeedObjectId) {
        const auto ritual=Level16SeedRitualModel::evaluate_21880(
            session,world,&next,
            world.loose_items.size()==0x12CU);
        if(ritual.status==
           Level16SeedRitualStatus::ReadyForSourceActions) {
            result.status=
                InventoryWorldDropStatus::SourceSpecialActionPending;
            return result;
        }
    }
    if(world.loose_items.size()==0x12CU)
        world.loose_items.erase(world.loose_items.begin());
    world.loose_items.push_back(next);
    inventory.held={-1,0U};
    result.status=InventoryWorldDropStatus::Dropped;
    result.loose_item_index=world.loose_items.size()-1U;
    result.cached_cell_index=next.cached_cell_index;
    return result;
}

} // namespace rloft
