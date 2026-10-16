#ifdef NDEBUG
#undef NDEBUG
#endif
#include "rloft/enhanced_game_save.hpp"
#include "rloft/assassin_relic_transition.hpp"
#include <cassert>
#include <stdexcept>
#include <vector>

int main(){
    rloft::EnhancedGameSnapshot s{};
    s.original_media_id=0x0123456789ABCDEFULL;
    s.session.level_id=22;s.session.coordinate_a=0x00048000U;
    s.session.coordinate_b=0x00048000U;s.session.facing=270U;
    s.session.time_of_day_16_16_hours=0x000E0000U;
    s.session.party.bytes[0]='R';
    s.session.persistent_event_flags[31]=17;
    s.session.level.width=8;s.session.level.height=9;
    s.session.level.cell_words.assign(72U,0x0440U);
    s.session.level.cell_words[37]=0x0581U;
    rloft::CellControlRecord door{};
    door.raw[0]=1;door.raw[4]=4;door.raw[5]=5;
    door.raw[6]=8;s.session.level.group2.push_back(door);
    s.session.level.event_scripts[0]=33;
    s.session.level.automap_cache.assign(72,0x2FU);
    rloft::LiveNpcRecord npc{};
    npc.current_hp=7;npc.x_position_16_16=0x58000U;
    npc.y_position_16_16=0x68000U;s.world.npcs.push_back(npc);
    rloft::LiveObjectRecord obj{};
    obj.object_type_id=86;obj.x_position_16_16=0x38000;
    s.world.placed_objects.push_back(obj);
    s.world.loose_items.push_back(obj);
    s.inventory.held.object_id=35;s.inventory.held.known=1;
    s.inventory.containers[1].slot_count=2;
    s.inventory.containers[1].items={5,39};
    s.inventory.party_container_ui_contexts[0].raw={{1,2,3,4}};
    // Verify an actual multi-level SAVE/Continue snapshot, not only the
    // first active Level22. Previously visited worlds retain independent
    // doors, NPC HP, dropped objects and direct-save precedence.
    rloft::LevelContinuityEntry visited{};
    visited.level.width=3;
    visited.level.height=2;
    visited.level.cell_words={0U,0x0080U,0U,1U,0U,0U};
    visited.level.group2.push_back(door);
    visited.level.group2[0].raw[6]=0U;
    visited.level.event_scripts[13]=0xA5U;
    visited.level.automap_annotations={9U,8U,7U};
    visited.world.npcs.push_back(npc);
    visited.world.npcs[0].current_hp=2;
    visited.world.loose_items.push_back(obj);
    visited.world.loose_items[0].object_type_id=0x74;
    visited.from_direct_save=true;
    s.visited_levels.emplace(7,std::move(visited));
    s.random_state=0xABCD4321;
    s.original_story_selector=2;
    auto b=rloft::EnhancedGameSaveCodec::encode(s);
    auto out=rloft::EnhancedGameSaveCodec::decode(b);
    assert(out.original_media_id==s.original_media_id);
    assert(out.session.level_id==22U && out.session.facing==270U);
    assert(out.session.party.bytes[0]=='R');
    assert(out.session.persistent_event_flags[31]==17);
    assert(out.session.level.cell_words[37]==0x0581U);
    assert(out.session.level.group2[0].raw[6]==8);
    assert(out.session.level.event_scripts[0]==33);
    assert(out.world.npcs[0].current_hp==7);
    assert(out.world.placed_objects[0].object_type_id==86);
    assert(out.world.loose_items.size()==1);
    assert(out.inventory.held.object_id==35);
    assert(out.inventory.containers[1].items[1]==39);
    assert(out.inventory.party_container_ui_contexts[0].raw[2]==3);
    assert(out.random_state==s.random_state);
    assert(out.original_story_selector==2);
    assert(out.visited_levels.size()==1U);
    const auto& old=out.visited_levels.at(7);
    assert(old.from_direct_save);
    assert(old.level.cell_words[1]==0x0080U);
    assert(old.level.group2[0].raw[6]==0U);
    assert(old.level.event_scripts[13]==0xA5U);
    assert(old.level.automap_annotations[0]==9U);
    assert(old.world.npcs[0].current_hp==2);
    assert(old.world.loose_items[0].object_type_id==0x74U);
    // Save files must be fully round-trippable, not merely decodable.
    // Re-encoding an intact load preserves all persisted bytes.
    assert(rloft::EnhancedGameSaveCodec::encode(out)==b);

    // Original Level22 quest continuity through native Save/Continue:
    // defeated Assassin, source-authored drops, and story flag persist.
    // This must also work after one relic has moved into the player's
    // cursor. Do not restore the enemy or regenerate unique drops.
    auto quest=s;
    quest.session.level.width=30;
    quest.session.level.height=15;
    quest.session.level.cell_words.assign(30U*15U,0U);
    quest.session.level.automap_cache.assign(30U*15U,0x2FU);
    quest.world.npcs[0].npc_type_id=9U;
    quest.world.npcs[0].current_hp=0;
    quest.world.npcs[0].runtime_flags=0x02000000U;
    quest.world.loose_items.clear();
    quest.inventory.held.object_id=-1;
    quest.inventory.held.known=0U;
    quest.original_story_selector=1U;
    // The native snapshot already owns all original party bytes.
    // Prove that DDA4's class-track XP DWORD at actor+26 survives
    // both Assassin death Save/Continue and the Level23 warp.
    quest.session.party.bytes[0x26U]=0xC8U;
    quest.session.party.bytes[0x27U]=0U;
    quest.session.party.bytes[0x28U]=0U;
    quest.session.party.bytes[0x29U]=0U;
    for(const auto relic:rloft::AssassinRelicTransition::kDrops){
        rloft::LiveObjectRecord record{};
        record.object_type_id=relic.object_id;
        record.x_position_16_16=0x000D8000U;
        record.y_position_16_16=0x00078000U;
        quest.world.loose_items.push_back(record);
    }
    auto quest_bytes=rloft::EnhancedGameSaveCodec::encode(quest);
    auto quest_loaded=rloft::EnhancedGameSaveCodec::decode(quest_bytes);
    assert(quest_loaded.world.npcs.size()==1U);
    assert(quest_loaded.world.npcs[0].current_hp==0);
    assert(quest_loaded.world.npcs[0].npc_type_id==9U);
    assert(quest_loaded.world.loose_items.size()==4U);
    assert(quest_loaded.original_story_selector==1U);
    assert(quest_loaded.session.party.bytes[0x26U]==0xC8U);
    assert(quest_loaded.session.party.bytes[0x27U]==0U);
    for(std::size_t i=0;i<4U;++i)
        assert(quest_loaded.world.loose_items[i].object_type_id==
            rloft::AssassinRelicTransition::kDrops[i].object_id);
    assert(rloft::EnhancedGameSaveCodec::encode(quest_loaded)==quest_bytes);

    quest_loaded.inventory.held.object_id=static_cast<std::int16_t>(
        quest_loaded.world.loose_items.front().object_type_id);
    quest_loaded.world.loose_items.erase(
        quest_loaded.world.loose_items.begin());
    quest_bytes=rloft::EnhancedGameSaveCodec::encode(quest_loaded);
    auto after_pickup=rloft::EnhancedGameSaveCodec::decode(quest_bytes);
    assert(after_pickup.inventory.held.object_id==0x88);
    assert(after_pickup.world.npcs[0].current_hp==0);
    assert(after_pickup.world.loose_items.size()==3U);
    assert(after_pickup.world.loose_items[0].object_type_id==0x87U);
    assert(after_pickup.world.loose_items[1].object_type_id==0xE9U);
    assert(after_pickup.world.loose_items[2].object_type_id==0x74U);
    assert(rloft::EnhancedGameSaveCodec::encode(after_pickup)==quest_bytes);

    // Native continuation after original Level22 mist event: preserve the
    // real Level23 destination, the surviving Assassin parchment and the
    // saved mutable corpse world in Level22 continuity. No original
    // SAVE.DAT bytes or invented exit topology enter this test.
    auto level23=quest_loaded;
    rloft::LevelContinuityEntry level22_before_warp{};
    level22_before_warp.level=level23.session.level;
    level22_before_warp.world=level23.world;
    level22_before_warp.from_direct_save=true;
    level23.visited_levels.emplace(22,std::move(level22_before_warp));
    level23.session.level_id=
        rloft::AssassinRelicTransition::kDestinationLevel;
    level23.session.coordinate_a=
        rloft::AssassinRelicTransition::kDestinationCoordinateA;
    level23.session.coordinate_b=
        rloft::AssassinRelicTransition::kDestinationCoordinateB;
    level23.session.facing=
        rloft::AssassinRelicTransition::kDestinationFacing;
    level23.session.level.width=64;
    level23.session.level.height=64;
    level23.session.level.cell_words.assign(64U*64U,0U);
    level23.session.level.automap_cache.assign(64U*64U,0xFFU);
    level23.session.level.group2.clear();
    level23.world.npcs.clear();
    level23.world.loose_items.clear();
    level23.inventory.held.object_id=0xE9;
    const auto warped_bytes=rloft::EnhancedGameSaveCodec::encode(level23);
    const auto warped=rloft::EnhancedGameSaveCodec::decode(warped_bytes);
    assert(warped.session.level_id==23U);
    assert(warped.session.coordinate_a==0x00098000U);
    assert(warped.session.coordinate_b==0x003A8000U);
    assert(warped.session.facing==0xB4U);
    assert(warped.inventory.held.object_id==0xE9);
    assert(warped.session.party.bytes[0x26U]==0xC8U);
    assert(warped.session.party.bytes[0x27U]==0U);
    assert(warped.visited_levels.count(22)==1U);
    const auto& old22=warped.visited_levels.at(22);
    assert(old22.world.npcs[0].npc_type_id==9U);
    assert(old22.world.npcs[0].current_hp==0);
    assert(old22.world.loose_items.size()==4U);
    assert(rloft::EnhancedGameSaveCodec::encode(warped)==warped_bytes);

    b[b.size()-2U]^=1U;
    bool bad=false;
    try{(void)rloft::EnhancedGameSaveCodec::decode(b);}
    catch(const std::invalid_argument&){bad=true;}
    assert(bad);
    b.pop_back();bad=false;
    try{(void)rloft::EnhancedGameSaveCodec::decode(b);}
    catch(const std::invalid_argument&){bad=true;}
    assert(bad);
    return 0;
}
