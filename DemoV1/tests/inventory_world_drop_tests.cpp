#ifdef NDEBUG
#undef NDEBUG
#endif
#include "rloft/inventory_world_drop.hpp"
#include <array>
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


    // Binary-verified 2D5D0 mode 0x190: the last argument is
    // an actor selector, not a Q16 radius. The exact sweep bounds
    // are +/-0x5800 Q16. Coordinate 3.E000 covers cells 3 and 4.
    auto live_npcs=rloft::NpcCatalog::parse({0U,0U,0U,0U});
    const auto& lev=s.level;
    (void)lev;
    assert(rloft::InventoryWorldDropModel::placement_passable_2d5d0(
        s,world,live_npcs,0x00048000U,0x0003E000U,0x190U));
    assert(!rloft::InventoryWorldDropModel::placement_passable_2d5d0(
        s,world,live_npcs,0x00048000U,0x0003E000U,0U));

    // Two adjacent original cells participate near the Q16 seam.
    s.level.cell_words[4U*30U+4U]=0x0001U;
    assert(!rloft::InventoryWorldDropModel::placement_passable_2d5d0(
        s,world,live_npcs,0x00048000U,0x0003E000U,0x190U));
    s.level.cell_words[4U*30U+4U]=0x0801U;
    assert(!rloft::InventoryWorldDropModel::placement_passable_2d5d0(
        s,world,live_npcs,0x00048000U,0x0003E000U,0x190U));

    // A 0x1000 low-six wall defers to original 1E8C8 type
    // control: type1 bypasses, type8 blocks, no record bypasses.
    s.level.cell_words[4U*30U+4U]=0x1001U;
    assert(rloft::InventoryWorldDropModel::placement_passable_2d5d0(
        s,world,live_npcs,0x00048000U,0x0003E000U,0x190U));
    rloft::CellControlRecord ctrl{};
    ctrl.raw[0]=1U;ctrl.raw[4]=4U;ctrl.raw[5]=4U;
    s.level.group2.push_back(ctrl);
    assert(rloft::InventoryWorldDropModel::placement_passable_2d5d0(
        s,world,live_npcs,0x00048000U,0x0003E000U,0x190U));
    s.level.group2[0].raw[0]=8U;
    assert(!rloft::InventoryWorldDropModel::placement_passable_2d5d0(
        s,world,live_npcs,0x00048000U,0x0003E000U,0x190U));
    s.level.group2[0].raw[0]=1U;
    s.level.cell_words[4U*30U+4U]=0x0080U;
    // 1E930 position byte 0 passes dynamic cell.
    assert(rloft::InventoryWorldDropModel::placement_passable_2d5d0(
        s,world,live_npcs,0x00048000U,0x0003E000U,0x190U));
    // Position byte 8 generates 0x8000 and mode 0x190 blocks.
    s.level.group2[0].raw[6]=8U;
    assert(!rloft::InventoryWorldDropModel::placement_passable_2d5d0(
        s,world,live_npcs,0x00048000U,0x0003E000U,0x190U));
    s.level.group2.clear();
    s.level.cell_words[4U*30U+4U]=0U;

    // Original 2C800: NPCS +0x16 plus 0x3E8, excluding
    // NPC runtime flag bit 02; the item is never consumed here.
    std::vector<std::uint8_t> npc_bytes(4U+0x45U,0U);
    npc_bytes[0]=1U;
    npc_bytes[4U+0x16U]=0x00U;
    npc_bytes[4U+0x17U]=0x02U; // 0x0200 Q16 half extent
    live_npcs=rloft::NpcCatalog::parse(npc_bytes);
    rloft::LiveNpcRecord npc{};
    npc.npc_type_id=0U;
    npc.y_position_16_16=0x00048000U;
    npc.x_position_16_16=0x0003E000U;
    world.npcs.push_back(npc);
    assert(!rloft::InventoryWorldDropModel::placement_passable_2d5d0(
        s,world,live_npcs,0x00048000U,0x0003E000U,0x190U));
    world.npcs[0].runtime_flags=0x02000000U;
    assert(rloft::InventoryWorldDropModel::placement_passable_2d5d0(
        s,world,live_npcs,0x00048000U,0x0003E000U,0x190U));
    world.npcs.clear();

    // 2C9DC player proximity is a distinct +/-0x8FA0 test.
    assert(!rloft::InventoryWorldDropModel::placement_passable_2d5d0(
        s,world,live_npcs,s.coordinate_b,s.coordinate_a,0x190U));

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

    // CODE.2 21880 only enters the special 4-site check on
    // Level16. It returns immediately elsewhere; non-completing
    // Level16 drops are also ordinary, while the fourth seed
    // needs 13EF8(8) and 15050(0x73) effects.
    s.level_id=22U;
    inventory.held={0x89,1U};
    result=rloft::InventoryWorldDropModel::drop(
        world,s,inventory,definitions,tables,
        [](std::uint32_t,std::uint32_t,std::uint32_t){return false;});
    assert(result.status==rloft::InventoryWorldDropStatus::Dropped);
    assert(world.loose_items.back().object_type_id==0x89);
    assert(inventory.held.object_id==-1);
    s.level_id=16U;
    inventory.held={0x89,1U};
    const auto before_special=world.loose_items.size();
    result=rloft::InventoryWorldDropModel::drop(
        world,s,inventory,definitions,tables,
        [](std::uint32_t,std::uint32_t,std::uint32_t){
            return false;});
    assert(result.status==rloft::InventoryWorldDropStatus::Dropped);
    assert(world.loose_items.size()==before_special);
    assert(world.loose_items.back().object_type_id==0x89U);
    assert(inventory.held.object_id==-1);

    // Original object-1 logical 0x207E0 8-byte source table:
    // 0D 13, 0B 15, 0D 17, 0F 15. 218DB checks high
    // 16 bits of each family-C object's X then Y, and
    // requires all FOUR distinct source locations.
    auto ritual_session=session();
    ritual_session.level_id=16U;
    ritual_session.level.width=32;
    ritual_session.level.height=32;
    ritual_session.level.cell_words.assign(1024U,0U);
    ritual_session.persistent_event_flags.fill(0U);
    rloft::LevelWorldState ritual_world{};
    const auto seed_at=[](std::uint16_t x,std::uint16_t y,
                          std::uint16_t id=0x89U,
                          std::uint16_t low_x=0U,
                          std::uint16_t low_y=0U) {
        rloft::LiveObjectRecord obj{};
        obj.object_type_id=id;
        obj.x_position_16_16=
            (static_cast<std::uint32_t>(x)<<16U)|low_x;
        obj.y_position_16_16=
            (static_cast<std::uint32_t>(y)<<16U)|low_y;
        return obj;
    };
    using SeedStatus=rloft::Level16SeedRitualStatus;
    using SeedModel=rloft::Level16SeedRitualModel;
    auto ritual=SeedModel::evaluate_21880(
        ritual_session,ritual_world);
    assert(ritual.status==SeedStatus::MissingSeed);
    assert(ritual.present==
        (std::array<bool,4>{{false,false,false,false}}));
    // Fractions are irrelevant. Wrong object class and duplicate
    // seed at a single site cannot satisfy a different site.
    ritual_world.loose_items.push_back(
        seed_at(13U,19U,0x89U,0xBAADU,0xC0DEU));
    ritual_world.loose_items.push_back(seed_at(13U,19U));
    ritual_world.loose_items.push_back(seed_at(11U,21U,0x8AU));
    ritual=SeedModel::evaluate_21880(
        ritual_session,ritual_world);
    assert(ritual.status==SeedStatus::MissingSeed);
    assert(ritual.present==
        (std::array<bool,4>{{true,false,false,false}}));
    ritual_world.loose_items.push_back(seed_at(11U,21U));
    ritual_world.loose_items.push_back(seed_at(13U,23U));
    ritual=SeedModel::evaluate_21880(
        ritual_session,ritual_world);
    assert(ritual.status==SeedStatus::MissingSeed);
    assert(ritual.present==
        (std::array<bool,4>{{true,true,true,false}}));
    const auto final_seed=seed_at(15U,21U,0x89U,0x8000U,0x1234U);
    ritual=SeedModel::evaluate_21880(
        ritual_session,ritual_world,&final_seed);
    assert(ritual.status==SeedStatus::ReadyForSourceActions);
    assert(ritual.present==
        (std::array<bool,4>{{true,true,true,true}}));
    assert(ritual.kEventBit==0x44U);
    assert(ritual.kFirstFollowOnArg==8U);
    assert(ritual.kSecondFollowOnArg==0x73U);
    // The predicate alone NEVER changes the 0x44 story flag.
    assert(ritual_session.persistent_event_flags[0x44U/8U]==0U);

    // 2195C must avoid committing the fourth seed because its
    // cinematic/level-effect dispatcher is not implemented yet.
    // The first three source sites already contain seeds.
    ritual_session.coordinate_a=0x000F8000U;
    ritual_session.coordinate_b=0x00158000U;
    rloft::InventoryRuntimeState ritual_inv{};
    ritual_inv.held={0x89,1U};
    const auto before_ritual=ritual_world.loose_items.size();
    result=rloft::InventoryWorldDropModel::drop(
        ritual_world,ritual_session,ritual_inv,definitions,tables,
        [](std::uint32_t,std::uint32_t,std::uint32_t){
            return false;});
    assert(result.status==
        rloft::InventoryWorldDropStatus::SourceSpecialActionPending);
    assert(ritual_world.loose_items.size()==before_ritual);
    assert(ritual_inv.held.object_id==0x89);
    assert(ritual_inv.held.known==1U);
    // Setting bit 0x44 reproduces original 2189F early
    // return: an ordinary fourth seed drop is then allowed.
    ritual_session.persistent_event_flags[0x44U/8U]=
        static_cast<std::uint8_t>(1U<<(0x44U%8U));
    ritual=SeedModel::evaluate_21880(
        ritual_session,ritual_world,&final_seed);
    assert(ritual.status==SeedStatus::AlreadyFlagged);
    result=rloft::InventoryWorldDropModel::drop(
        ritual_world,ritual_session,ritual_inv,definitions,tables,
        [](std::uint32_t,std::uint32_t,std::uint32_t){
            return false;});
    assert(result.status==rloft::InventoryWorldDropStatus::Dropped);
    assert(ritual_world.loose_items.size()==before_ritual+1U);
    assert(ritual_inv.held.object_id==-1);

    // 2195C evicts the earliest loose-object record when full
    // BEFORE calling 21880. The pure candidate audit models
    // that behavior rather than counting an evicted seed.
    ritual_session.persistent_event_flags.fill(0U);
    rloft::LevelWorldState crowded{};
    crowded.loose_items.push_back(seed_at(13U,19U));
    for(std::size_t i=1U;i<0x12CU;++i)
        crowded.loose_items.push_back(seed_at(4U,4U,0x20U));
    crowded.loose_items[1U]=seed_at(11U,21U);
    crowded.loose_items[2U]=seed_at(13U,23U);
    ritual=SeedModel::evaluate_21880(
        ritual_session,crowded,&final_seed,true);
    assert(ritual.status==SeedStatus::MissingSeed);
    assert(!ritual.present[0U]);
    ritual=SeedModel::evaluate_21880(
        ritual_session,crowded,&final_seed,false);
    assert(ritual.status==SeedStatus::ReadyForSourceActions);
    ritual_session.level_id=22U;
    ritual=SeedModel::evaluate_21880(
        ritual_session,crowded,&final_seed);
    assert(ritual.status==SeedStatus::NotLevel16);

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
