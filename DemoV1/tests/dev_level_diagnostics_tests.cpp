#ifdef NDEBUG
#undef NDEBUG
#endif

#include "rloft/dev_level_diagnostics.hpp"

#include <array>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <string>

int main() {
    rloft::ExplorationSessionState session{};
    session.level_id=22U;
    session.coordinate_a=0x00048000U;
    session.coordinate_b=0x00048000U;
    session.facing=270U;
    session.time_of_day_16_16_hours=0x000E0000U;
    session.level.width=3;
    session.level.height=3;
    session.level.cell_words.assign(9U,0U);
    session.party.bytes.fill(0xFFU);
    session.party.bytes[0x10U]=0U;
    session.party.bytes[0x11U]=0U;

    rloft::LevelWorldState world{};
    rloft::LiveNpcRecord npc{};
    npc.npc_type_id=7U;
    npc.current_hp=12;
    npc.x_position_16_16=0x00018000U;
    world.npcs.push_back(npc);
    rloft::LiveObjectRecord loose{};
    loose.object_type_id=0x89U;
    loose.x_position_16_16=0x00028000U;
    world.loose_items.push_back(loose);
    rloft::InventoryRuntimeState inventory{};
    inventory.containers[0].slot_count=4U;
    inventory.containers[0].ui_columns=2U;
    inventory.containers[0].items={-1,2,-1,3};
    inventory.held={-1,0U};

    const auto initial=rloft::DevLevelDiagnostics::capture(
        session,world,inventory);
    assert(initial.level_id==22U);
    assert(initial.coordinate_a==0x00048000U);
    assert(initial.active_actors==1U);
    assert(initial.npc_count==1U);
    assert(initial.loose_item_count==1U);
    assert(initial.placed_object_count==0U);
    assert(initial.active_container_views==0U);
    assert(initial.set_story_flags==0U);
    const auto exact_repeat=rloft::DevLevelDiagnostics::capture(
        session,world,inventory);
    assert(!rloft::DevLevelDiagnostics::compare(
        initial,exact_repeat).any());

    // A camera update must never look like an inventory,
    // actor, quest or world mutation.
    session.coordinate_a+=0x10000U;
    const auto moved=rloft::DevLevelDiagnostics::capture(
        session,world,inventory);
    auto delta=rloft::DevLevelDiagnostics::compare(initial,moved);
    assert(delta.camera && !delta.party && !delta.inventory &&
           !delta.world && !delta.story_flags);
    session.coordinate_a=initial.coordinate_a;

    // CODE.1/CHARS party payload is fingerprinted directly,
    // not normalized through an invented character structure.
    session.party.bytes[0x40U]=0x73U;
    const auto equipped=rloft::DevLevelDiagnostics::capture(
        session,world,inventory);
    delta=rloft::DevLevelDiagnostics::compare(initial,equipped);
    assert(delta.party && !delta.camera && !delta.inventory &&
           !delta.world && !delta.story_flags);
    session.party.bytes[0x40U]=0xFFU;

    // An ordinary inventory cursor / nested container change
    // stays in the inventory domain.
    inventory.held={0x73,1U};
    const auto held=rloft::DevLevelDiagnostics::capture(
        session,world,inventory);
    delta=rloft::DevLevelDiagnostics::compare(initial,held);
    assert(delta.inventory && !delta.camera && !delta.party &&
           !delta.world && !delta.story_flags);
    inventory.held={-1,0U};
    inventory.containers[0].items[0]=258;
    const auto nested=rloft::DevLevelDiagnostics::capture(
        session,world,inventory);
    assert(rloft::DevLevelDiagnostics::compare(
        initial,nested).inventory);
    inventory.containers[0].items[0]=-1;
    inventory.party_container_ui_contexts[0].
        mark_open_21693(0U,0U,2);
    const auto open=rloft::DevLevelDiagnostics::capture(
        session,world,inventory);
    assert(open.active_container_views==1U);
    assert(rloft::DevLevelDiagnostics::compare(
        initial,open).inventory);
    inventory.party_container_ui_contexts[0]=
        rloft::PartyContainerUiContext{};

    // Persistent event bit 44h changes only story domain.
    session.persistent_event_flags[0x44U/8U] |=
        static_cast<std::uint8_t>(1U<<(0x44U%8U));
    const auto quest=rloft::DevLevelDiagnostics::capture(
        session,world,inventory);
    assert(quest.set_story_flags==1U);
    delta=rloft::DevLevelDiagnostics::compare(initial,quest);
    assert(delta.story_flags && !delta.camera && !delta.party &&
           !delta.inventory && !delta.world);
    session.persistent_event_flags.fill(0U);

    // Original family-C loose item movements and NPC HP changes
    // are world changes; the inspector must not mutate records.
    world.loose_items[0].x_position_16_16+=0x10000U;
    const auto world_moved=rloft::DevLevelDiagnostics::capture(
        session,world,inventory);
    delta=rloft::DevLevelDiagnostics::compare(initial,world_moved);
    assert(delta.world && !delta.camera && !delta.party &&
           !delta.inventory && !delta.story_flags);
    assert(world.loose_items[0].object_type_id==0x89U);
    world.loose_items[0].x_position_16_16=0x00028000U;
    world.npcs[0].current_hp=11;
    assert(rloft::DevLevelDiagnostics::compare(
        initial,rloft::DevLevelDiagnostics::capture(
            session,world,inventory)).world);
    world.npcs[0].current_hp=12;
    session.level.cell_words[2]=0x2400U;
    assert(rloft::DevLevelDiagnostics::compare(
        initial,rloft::DevLevelDiagnostics::capture(
            session,world,inventory)).world);
    session.level.cell_words[2]=0U;

    const auto restored=rloft::DevLevelDiagnostics::capture(
        session,world,inventory);
    assert(!rloft::DevLevelDiagnostics::compare(
        initial,restored).any());

    // Cross-run digest is explicit big-endian HEX DISPLAY over
    // low32 bits of the FNV64, always exactly 8 characters.
    assert(rloft::DevLevelSnapshot::digest8(0U)=="00000000");
    assert(rloft::DevLevelSnapshot::digest8(0x12345678ABCDEF01ULL)==
           "ABCDEF01");
    assert(rloft::DevLevelSnapshot::digest8(initial.party_hash).
           size()==8U);

    // Read-only invariant, even for a fully populated world:
    // capturing may NOT reset HP, alter source known bits, or
    // change any of the canonical four actor 6FA0 bytes.
    assert(session.party.bytes[0x40U]==0xFFU);
    assert(session.persistent_event_flags[0x44U/8U]==0U);
    assert(inventory.party_container_ui_contexts[0].inactive());
    assert(world.npcs[0].current_hp==12);
}
