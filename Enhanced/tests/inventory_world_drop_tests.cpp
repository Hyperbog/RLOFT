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
    // Original SINTAB fixed point is a 16-bit fractional
    // magnitude in the LOW WORD; 0x00010000 has magnitude
    // zero and cannot represent sine/cosine 1. Use the
    // largest representable positive source fraction.
    tables.sine_q16_signmag[0]=0x0000FFFFU; // near 1 at 0 degrees
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
            assert(x==0x00030002U);
            return true;
        });
    assert(result.status==rloft::InventoryWorldDropStatus::Dropped);
    assert(world.loose_items.size()==1U);
    assert(inventory.held.object_id==-1);
    assert(inventory.held.known==0U);
    assert(world.loose_items[0].object_type_id==50U);
    assert(world.loose_items[0].x_position_16_16==0x00030002U);
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
    assert(ritual.kNumberedEventId==8U);
    assert(ritual.kNumberedEventFilename=="CINE08.ANM");
    assert(ritual.kGrantedObjectId==115U);
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
    // The flag-set branch above deliberately inserted a fourth
    // seed. Remove that test-only instance before the separate
    // ritual-completion test starts: CODE.2 must begin with
    // exactly THREE original occupied sites, not a leftover
    // fourth that invalidates the incomplete-site regression.
    ritual_world.loose_items.pop_back();
    assert(ritual_world.loose_items.size()==before_ritual);

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

    const auto reject_forward=[](
        std::uint32_t,std::uint32_t,std::uint32_t) {
        return false;
    };

    // SOURCE-ORDER complete 21880:
    // 2195C appends fourth Seed and clears the 0x89 cursor,
    // 1C124 sets flag 44h, 13EF8 runs numbered CINE08,
    // then 15050(73h) grants exactly one reward.
    // No callback may see the Holy Symbol before the event.
    ritual_session.level_id=16U;
    ritual_session.persistent_event_flags.fill(0U);
    ritual_session.coordinate_a=0x000F8000U;
    ritual_session.coordinate_b=0x00158000U;
    using Completion=rloft::Level16RitualCompletionStatus;
    using Playback=rloft::Level16RitualPlayback;
    const auto unchanged_ritual_world=ritual_world.loose_items.size();
    rloft::InventoryRuntimeState completed_inventory{};
    completed_inventory.held={0x89,1U};
    bool event_seen=false;
    const auto completed=rloft::Level16RitualCompletionModel::
        complete_21880(
            ritual_session,ritual_world,completed_inventory,
            definitions,tables,reject_forward,
            [&]() {
                event_seen=true;
                assert(ritual_world.loose_items.size()==
                    unchanged_ritual_world+1U);
                assert(ritual_world.loose_items.back().object_type_id==
                    0x89U);
                assert(ritual_session.persistent_event_flags[0x44U/8U]&
                    (1U<<(0x44U%8U)));
                assert(completed_inventory.held.object_id==-1);
                return Playback::Completed;
            });
    assert(event_seen);
    assert(completed.status==Completion::Completed);
    assert(completed.reward.status==
        rloft::HolySymbolGrantStatus::GivenToCursor);
    assert(completed_inventory.held.object_id==0x73);
    assert(completed_inventory.held.known==0U);
    assert(ritual_world.loose_items.size()==
        unchanged_ritual_world+1U);
    assert(ritual_session.persistent_event_flags[0x44U/8U]&
        (1U<<(0x44U%8U)));

    // Already completed: another identical event must NOT play
    // and cannot grant a second Holy Symbol.
    completed_inventory.held={0x89,1U};
    int events_called=0;
    const auto once=rloft::Level16RitualCompletionModel::
        complete_21880(
            ritual_session,ritual_world,completed_inventory,
            definitions,tables,reject_forward,
            [&]() {++events_called;return Playback::Completed;});
    assert(once.status==Completion::NotReady);
    assert(events_called==0);
    assert(completed_inventory.held.object_id==0x89);

    // Native Escape/quit safety: restore original source world,
    // cursor and event flags atomically, with no hidden reward.
    ritual_world.loose_items.pop_back();
    ritual_session.persistent_event_flags.fill(0U);
    const auto before_cancel=ritual_world.loose_items.size();
    for(const auto event_result:{Playback::Escaped,Playback::Quit}) {
        completed_inventory.held={0x89,1U};
        int calls=0;
        const auto cancelled=rloft::Level16RitualCompletionModel::
            complete_21880(
                ritual_session,ritual_world,completed_inventory,
                definitions,tables,reject_forward,
                [&]() {
                    ++calls;
                    assert(ritual_world.loose_items.size()==
                        before_cancel+1U);
                    assert(completed_inventory.held.object_id==-1);
                    return event_result;
                });
        assert(calls==1);
        assert(cancelled.status==
            (event_result==Playback::Quit
             ? Completion::Quit : Completion::Escaped));
        assert(ritual_world.loose_items.size()==before_cancel);
        assert(ritual_session.persistent_event_flags[0x44U/8U]==0U);
        assert(completed_inventory.held.object_id==0x89);
        assert(completed_inventory.held.known==1U);
    }

    // An event resource or rendering exception must NOT leave the
    // quest flagged, the seed missing, or a reward half-granted.
    bool event_threw=false;
    try {
        (void)rloft::Level16RitualCompletionModel::complete_21880(
            ritual_session,ritual_world,completed_inventory,
            definitions,tables,reject_forward,
            []() -> Playback {
                throw std::runtime_error("corrupt event 8 resource");
            });
    } catch(const std::runtime_error&) { event_threw=true; }
    assert(event_threw);
    assert(ritual_world.loose_items.size()==before_cancel);
    assert(ritual_session.persistent_event_flags[0x44U/8U]==0U);
    assert(completed_inventory.held.object_id==0x89);

    // No event callback is not equivalent to Completed.
    bool invalid_presenter=false;
    try {
        (void)rloft::Level16RitualCompletionModel::complete_21880(
            ritual_session,ritual_world,completed_inventory,
            definitions,tables,reject_forward,{});
    } catch(const std::invalid_argument&) {
        invalid_presenter=true;
    }
    assert(invalid_presenter);

    // Three original sites are required BEFORE the candidate
    // fourth Seed; an incomplete map is not permission to play
    // CINE08, set 44h or preemptively grant a Holy Symbol.
    auto missing_site_world=ritual_world;
    missing_site_world.loose_items.pop_back(); // site (13,23)
    const auto missing_size=missing_site_world.loose_items.size();
    int incorrect_callbacks=0;
    completed_inventory.held={0x89,1U};
    const auto incomplete=rloft::Level16RitualCompletionModel::
        complete_21880(
            ritual_session,missing_site_world,completed_inventory,
            definitions,tables,reject_forward,
            [&]() {
                ++incorrect_callbacks;
                return Playback::Completed;
            });
    assert(incomplete.status==Completion::NotReady);
    assert(incorrect_callbacks==0);
    assert(missing_site_world.loose_items.size()==missing_size);
    assert(completed_inventory.held.object_id==0x89);
    assert(ritual_session.persistent_event_flags[0x44U/8U]==0U);

    // Invalid original map/cell geometry must be rejected
    // before the first event frame and without losing the Seed.
    auto invalid_ritual_session=ritual_session;
    invalid_ritual_session.level.cell_words.clear();
    const auto invalid=rloft::Level16RitualCompletionModel::
        complete_21880(
            invalid_ritual_session,ritual_world,
            completed_inventory,definitions,tables,reject_forward,
            [&]() {
                ++incorrect_callbacks;
                return Playback::Completed;
            });
    assert(invalid.status==Completion::InvalidPlacement);
    assert(incorrect_callbacks==0);
    assert(completed_inventory.held.object_id==0x89);
    assert(ritual_world.loose_items.size()==before_cancel);

    // Original flag + event branch is specific to Level16.
    auto wrong_level_session=ritual_session;
    wrong_level_session.level_id=22U;
    const auto other_level=rloft::Level16RitualCompletionModel::
        complete_21880(
            wrong_level_session,ritual_world,
            completed_inventory,definitions,tables,reject_forward,
            [&]() {
                ++incorrect_callbacks;
                return Playback::Completed;
            });
    assert(other_level.status==Completion::NotReady);
    assert(incorrect_callbacks==0);
    assert(completed_inventory.held.object_id==0x89);

    // Bounded 15050(0x73) source reward routing: after the
    // numbered event, the HOLY SYMBOL OF RAVENKIND is put on
    // an empty cursor; otherwise the first empty ordinary
    // slot in active actor order; otherwise 2195C world-spawn
    // is requested and NO unbound world action is fabricated.
    rloft::PartyImage reward_party{};
    reward_party.bytes.fill(0xFFU);
    reward_party.bytes[0x10U]=0U;
    reward_party.bytes[0x11U]=0U; // actor0 active
    rloft::InventoryRuntimeState reward_inv{};
    reward_inv.held={-1,1U}; // known must be reset to zero
    using RewardStatus=rloft::HolySymbolGrantStatus;
    auto grant=rloft::HolySymbolRewardModel::grant_15050(
        reward_party,reward_inv);
    assert(grant.status==RewardStatus::GivenToCursor);
    assert(reward_inv.held.object_id==0x73);
    assert(reward_inv.held.known==0U);
    assert(reward_party.bytes[0x40U]==0xFFU);

    const auto put_item=[](rloft::PartyImage& party,
                           std::size_t actor,std::size_t slot,
                           std::int16_t id) {
        const auto at=actor*rloft::PartyImage::kActorSize+
            0x40U+2U*slot;
        const auto raw=static_cast<std::uint16_t>(id);
        party.bytes[at]=static_cast<std::uint8_t>(raw);
        party.bytes[at+1U]=static_cast<std::uint8_t>(raw>>8U);
    };
    reward_inv.held={7,1U};
    reward_party.bytes[0x12BU]=0xFFU;
    grant=rloft::HolySymbolRewardModel::grant_15050(
        reward_party,reward_inv);
    assert(grant.status==RewardStatus::GivenToPartySlot);
    assert(grant.actor==0U && grant.slot==0U);
    assert(reward_party.bytes[0x40U]==0x73U);
    assert(reward_party.bytes[0x41U]==0U);
    assert(reward_party.bytes[0x12BU]==0xFEU);
    assert(reward_inv.held.object_id==7);
    assert(reward_inv.held.known==1U);

    // CODE.2 150C9..1511D skips actors whose status word
    // contains 8000h. Actor0 has all 12 ordinary slots
    // occupied; actor1 remains inactive. Actor2 is active
    // and the first free slot is slot5. Actor3 is inactive.
    for(std::size_t slot=0U;slot<12U;++slot)
        put_item(reward_party,0U,slot,7);
    const auto actor2=2U*rloft::PartyImage::kActorSize;
    reward_party.bytes[actor2+0x10U]=0U;
    reward_party.bytes[actor2+0x11U]=0U;
    for(std::size_t slot=0U;slot<12U;++slot)
        put_item(reward_party,2U,slot,7);
    put_item(reward_party,2U,5U,-1);
    reward_party.bytes[actor2+0x12BU]=0xFFU;
    grant=rloft::HolySymbolRewardModel::grant_15050(
        reward_party,reward_inv);
    assert(grant.status==RewardStatus::GivenToPartySlot);
    assert(grant.actor==2U && grant.slot==5U);
    assert(reward_party.bytes[actor2+0x40U+5U*2U]==0x73U);
    assert(reward_party.bytes[actor2+0x12BU]==0xDFU);
    assert(reward_inv.held.object_id==7);

    // Even if the actor inventory is completely full, the
    // unimplemented 2195C world fallback cannot lose the
    // pending reward or overwrite the held item.
    for(std::size_t slot=0U;slot<12U;++slot)
        put_item(reward_party,2U,slot,7);
    const auto party_before_reward=reward_party.bytes;
    const auto cursor_before_reward=reward_inv.held;
    grant=rloft::HolySymbolRewardModel::grant_15050(
        reward_party,reward_inv);
    assert(grant.status==RewardStatus::NeedsWorldSpawn);
    assert(reward_party.bytes==party_before_reward);
    assert(reward_inv.held.object_id==
        cursor_before_reward.object_id);
    assert(reward_inv.held.known==cursor_before_reward.known);

    // Original 15050 full-inventory fallback calls
    // 2195C(0x80,0x73,0,playerX,playerY,1,0).
    // Distinguish its final clear-held=0 from ordinary inventory
    // D drops (clear-held=1). The emitted reward is a known=0
    // family-C loose item, with the original placement fallback.
    auto reward_session=session();
    rloft::LevelWorldState reward_world{};
    const auto actual_held_before=reward_inv.held;

    auto spawned=rloft::InventoryWorldDropModel::
        spawn_holy_symbol_reward_15050(
            reward_world,reward_session,definitions,tables,
            reject_forward);
    assert(spawned.status==rloft::InventoryWorldDropStatus::Dropped);
    assert(spawned.loose_item_index==0U);
    assert(reward_world.loose_items.size()==1U);
    assert(reward_world.loose_items.back().object_type_id==0x73U);
    assert(reward_world.loose_items.back().x_position_16_16==
        reward_session.coordinate_a);
    assert(reward_world.loose_items.back().y_position_16_16==
        reward_session.coordinate_b);
    assert(reward_world.loose_items.back().packed_runtime_state==
        0U); // 0x73 is emitted unidentified.
    assert(reward_inv.held.object_id==
        actual_held_before.object_id);
    assert(reward_inv.held.known==actual_held_before.known);
    // Same source 0x12C capacity/evict-first behavior as drops:
    // the oldest family-C record disappears but the reward and
    // held cursor remain unambiguously intact.
    reward_world.loose_items.clear();
    for(std::size_t i=0;i<0x12CU;++i) {
        rloft::LiveObjectRecord record{};
        record.object_type_id=static_cast<std::uint16_t>(
            i+1U);
        reward_world.loose_items.push_back(record);
    }
    spawned=rloft::InventoryWorldDropModel::
        spawn_holy_symbol_reward_15050(
            reward_world,reward_session,definitions,tables,
            reject_forward);
    assert(spawned.status==rloft::InventoryWorldDropStatus::Dropped);
    assert(spawned.loose_item_index==0x12BU);
    assert(reward_world.loose_items.size()==0x12CU);
    assert(reward_world.loose_items.front().object_type_id==2U);
    assert(reward_world.loose_items.back().object_type_id==0x73U);
    assert(reward_inv.held.object_id==
        actual_held_before.object_id);

    // Failing original map coordinates is a pure rejection:
    // do not evict any original record to make room and never
    // mutate a party item or the held cursor.
    const auto world_before_reject=reward_world.loose_items.size();
    auto invalid_reward_session=reward_session;
    invalid_reward_session.level.cell_words.clear();
    spawned=rloft::InventoryWorldDropModel::
        spawn_holy_symbol_reward_15050(
            reward_world,invalid_reward_session,definitions,tables,
            reject_forward);
    assert(spawned.status==
        rloft::InventoryWorldDropStatus::InvalidWorldPosition);
    assert(reward_world.loose_items.size()==world_before_reject);
    assert(reward_world.loose_items.back().object_type_id==0x73U);
    assert(reward_inv.held.object_id==
        actual_held_before.object_id);

    // The integrated 15050 reward helper can now complete its
    // source-order family-C world fallback without an event stub
    // or cursor mutation. The event-8 callback remains the
    // separate prerequisite for any actual Level16 completion.
    auto integrated_party=reward_party;
    auto integrated_inv=reward_inv;
    auto integrated_world=reward_world;
    const auto integrated_party_before=integrated_party.bytes;
    const auto integrated_held_before=integrated_inv.held;
    const auto prior_world_front=integrated_world.loose_items.front().
        object_type_id;
    const auto combined=rloft::HolySymbolRewardModel::
        grant_with_world_15050(
            integrated_party,integrated_inv,integrated_world,
            reward_session,definitions,tables,reject_forward);
    assert(combined.status==RewardStatus::SpawnedInWorld);
    assert(combined.loose_item_index==0x12BU);
    assert(integrated_world.loose_items.size()==0x12CU);
    assert(integrated_world.loose_items.front().object_type_id!=
        prior_world_front);
    assert(integrated_world.loose_items.back().object_type_id==0x73U);
    assert(integrated_world.loose_items.back().packed_runtime_state==0U);
    assert(integrated_party.bytes==integrated_party_before);
    assert(integrated_inv.held.object_id==
        integrated_held_before.object_id);
    assert(integrated_inv.held.known==
        integrated_held_before.known);
    assert(reward_session.persistent_event_flags[0x44U/8U]==0U);

    // A missing level-cell backing rejects only the world
    // fallback; it may not clear party items, evict a loose
    // entry or consume the held cursor.
    const auto combined_before=integrated_world.loose_items.size();
    const auto combined_front=integrated_world.loose_items.front().
        object_type_id;
    const auto refused=rloft::HolySymbolRewardModel::
        grant_with_world_15050(
            integrated_party,integrated_inv,integrated_world,
            invalid_reward_session,definitions,tables,reject_forward);
    assert(refused.status==RewardStatus::WorldPlacementRejected);
    assert(integrated_party.bytes==integrated_party_before);
    assert(integrated_inv.held.object_id==
        integrated_held_before.object_id);
    assert(integrated_world.loose_items.size()==combined_before);
    assert(integrated_world.loose_items.front().object_type_id==
        combined_front);

    // When the cursor is empty, even an invalid world geometry
    // must NOT prevent original 15050's cursor-first grant.
    integrated_inv.held={-1,1U};
    const auto cursor_first=rloft::HolySymbolRewardModel::
        grant_with_world_15050(
            integrated_party,integrated_inv,integrated_world,
            invalid_reward_session,definitions,tables,reject_forward);
    assert(cursor_first.status==RewardStatus::GivenToCursor);
    assert(integrated_inv.held.object_id==0x73);
    assert(integrated_inv.held.known==0U);
    assert(integrated_world.loose_items.size()==combined_before);

    // Actor-slot reward priority also wins over invalid world
    // state, without a change to the original held object.
    integrated_inv.held={9,1U};
    put_item(integrated_party,2U,5U,-1);
    const auto party_first=rloft::HolySymbolRewardModel::
        grant_with_world_15050(
            integrated_party,integrated_inv,integrated_world,
            invalid_reward_session,definitions,tables,reject_forward);
    assert(party_first.status==RewardStatus::GivenToPartySlot);
    assert(party_first.actor==2U && party_first.slot==5U);
    assert(integrated_inv.held.object_id==9);
    assert(integrated_world.loose_items.size()==combined_before);

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
