#ifdef NDEBUG
#undef NDEBUG
#endif
#include "rloft/enhanced_game_save.hpp"
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
