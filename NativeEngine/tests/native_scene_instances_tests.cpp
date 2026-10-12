#include "rloft_native/native_scene_instances.hpp"

#include <cassert>
#include <cstdint>
#include <stdexcept>
#include <vector>

using namespace rloft_native;
namespace {
void write16(std::array<std::uint8_t,11>& r,unsigned offset,unsigned value) {
    r[offset]=static_cast<std::uint8_t>(value);
    r[offset+1]=static_cast<std::uint8_t>(value>>8U);
}
template<class F> void rejects(F action) {
    bool rejected=false;
    try{action();}catch(const std::exception&){rejected=true;}
    assert(rejected);
}
NativeSceneDefinitionDirectory defs() {
    NativeSceneDefinitionDirectory defs;
    defs.banks.starts={0,2169,3316,4158};
    defs.banks.lengths={2169,1147,842,76};
    defs.banks.res3_entries=4234;
    defs.npc_definitions.assign(83,{0,0});
    defs.npc_definitions[9]={37,37};
    defs.object_definitions.assign(336,{0,3316+0});
    defs.object_definitions[0x58]={7,2,3316+7};
    defs.banks.init_descriptors[0].assign(175,{});
    defs.banks.init_descriptors[0][10]={12,0,1,1,0};
    defs.banks.init_descriptors[1].assign(12,{});
    return defs;
}
NativeWorldPayload world() {
    NativeWorldPayload w;
    for(unsigned level=0;level<32;++level) {
        NativeLevelPayload map;
        map.id=level;
        map.width=5;
        map.height=5;
        map.cell_words.assign(25,0);
        map.automap.assign(25,0);
        // Archive compact family A: type 9 NPC, x=1+0.5, y=2+0.25,
        // original orientation byte 5.
        std::array<std::uint8_t,11> npc{};
        write16(npc,0,9);
        write16(npc,2,1);
        write16(npc,4,0x8000);
        write16(npc,6,2);
        write16(npc,8,0x4000);
        npc[10]=5;
        map.families[0].push_back(npc);
        // Family B: placed object type 10, INIT source class 1.
        std::array<std::uint8_t,11> placed{};
        write16(placed,0,10);
        write16(placed,2,2);
        write16(placed,4,0x2000);
        write16(placed,6,3);
        placed[10]=3;
        map.families[1].push_back(placed);
        // Family C: archived type 0x54; original CODE.2 remaps to 0x58
        // before current OBJECTS.DAT namespace lookup.
        std::array<std::uint8_t,11> loose{};
        write16(loose,0,0x54);
        write16(loose,2,3);
        write16(loose,6,1);
        map.families[2].push_back(loose);
        w.total_cells+=25;
        w.levels.push_back(map);
    }
    return w;
}
void run() {
    const auto sources=world();
    const auto d=defs();
    auto ledger=derive_native_authored_scene_instances(sources,d);
    assert(ledger.levels.size()==32);
    assert(ledger.original_npc_instances==32);
    assert(ledger.original_placed_instances==32);
    assert(ledger.original_loose_instances==32);
    assert(ledger.original_legacy_object_aliases==32);
    assert(ledger.original_compact_families_sha256.size()==64);
    assert(!ledger.native_scene_frame_selection_verified);
    const auto& row=ledger.levels[22];
    assert(row.size()==3);
    assert(row[0].source_class==0 && row[0].original_type_id==9);
    assert(row[0].absolute_res3_base==37);
    assert(row[0].compact_facing_0_to_7==5);
    assert(row[0].cell_column==1 && row[0].cell_row==2);
    assert(row[0].x_position_16_16==0x18000);
    assert(row[0].y_position_16_16==0x24000);
    assert(row[1].source_class==1 && row[1].original_type_id==10);
    assert(row[1].absolute_res3_base==2169+12);
    assert(row[1].compact_flags==3);
    assert(row[1].x_position_16_16==0x22000);
    assert(row[2].source_class==2 && row[2].original_type_id==0x54);
    assert(row[2].runtime_type_id==0x58);
    assert(row[2].absolute_res3_base==3316+7);
    assert(!row[2].actual_view_subframe_verified);
    NativeWorldRuntime current(sources);
    assert(native_marked_scene_instance_candidates(current,ledger,22,0).empty());
    current.mark_reached_by_view(22,1,2);
    current.mark_reached_by_view(22,2,3);
    const auto visible=native_marked_scene_instance_candidates(
        current,ledger,22,2);
    assert(visible.size()==2);
    assert(visible[0].source_class==0);
    assert(visible[1].source_class==1);
    rejects([&]{static_cast<void>(native_marked_scene_instance_candidates(
        current,ledger,22,1));});
    current.mark_reached_by_view(22,3,1);
    assert(native_marked_scene_instance_candidates(current,ledger,22,3).size()==3);
    current.clear_transient_visibility(22);
    assert(native_marked_scene_instance_candidates(current,ledger,22,3).empty());
    assert(current.cell(22,1,2).explored);
    assert(!current.cell(21,1,2).explored);
    rejects([&]{static_cast<void>(native_marked_scene_instance_candidates(
        current,ledger,32,2));});

    auto foreign=world();
    foreign.levels[22].families[0][0][10]^=1;
    NativeWorldRuntime other(foreign);
    rejects([&]{static_cast<void>(
        native_marked_scene_instance_candidates(other,ledger,22,4));});
    auto bad_source=world();
    write16(bad_source.levels[0].families[0][0],0,83);
    rejects([&]{static_cast<void>(
        derive_native_authored_scene_instances(bad_source,d));});
    bad_source=world();
    write16(bad_source.levels[0].families[1][0],0,175);
    rejects([&]{static_cast<void>(
        derive_native_authored_scene_instances(bad_source,d));});
    bad_source=world();
    write16(bad_source.levels[0].families[2][0],0,336);
    rejects([&]{static_cast<void>(
        derive_native_authored_scene_instances(bad_source,d));});
    bad_source=world();
    write16(bad_source.levels[0].families[1][0],2,0xffff);
    rejects([&]{static_cast<void>(
        derive_native_authored_scene_instances(bad_source,d));});
}
}
int main() {run();}
