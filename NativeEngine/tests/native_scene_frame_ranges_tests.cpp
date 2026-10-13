#include "rloft_native/native_scene_frame_ranges.hpp"

#include <cassert>
#include <cstdint>
#include <stdexcept>
#include <string>
#include <vector>
#include <utility>

using namespace rloft_native;
namespace {
template<typename F> void rejects(F call) {
    bool thrown=false;
    try{call();}catch(const std::exception&){thrown=true;}
    assert(thrown);
}
NativeSceneDefinitionDirectory defs() {
    NativeSceneDefinitionDirectory out;
    out.banks.starts={0,2169,3316,4158};
    out.banks.lengths={2169,1147,842,76};
    out.banks.res3_entries=4234;
    out.npc_definitions.assign(83,{});
    out.npc_definitions[9]={37,37,8,4,3};
    out.object_definitions.assign(336,{});
    out.object_definitions[0x58]={7,2,3323};
    out.object_definitions[9]={0,0,3316};
    out.banks.init_descriptors[0].assign(175,{});
    out.banks.init_descriptors[0][10]={12,0,2,3,0};
    out.banks.init_descriptors[1].assign(12,{});
    return out;
}
NativeAuthoredSceneDirectory authored(bool include_base_only=false) {
    NativeAuthoredSceneDirectory source;
    source.resolved_source_bases_sha256=std::string(64,'a');
    for(unsigned level=0;level<32;++level) {
        std::vector<NativeAuthoredSceneInstance> v;
        NativeAuthoredSceneInstance npc{};
        npc.level=level;npc.source_class=0;
        npc.original_type_id=9;npc.runtime_type_id=9;
        npc.absolute_res3_base=37;
        v.push_back(npc);
        auto placed=npc;
        placed.source_class=1;placed.original_type_id=10;
        placed.runtime_type_id=10;placed.absolute_res3_base=2169+12;
        v.push_back(placed);
        auto loose=npc;
        loose.source_class=2;loose.original_type_id=0x54;
        loose.runtime_type_id=0x58;loose.absolute_res3_base=3316+7;
        v.push_back(loose);
        if(include_base_only) {
            loose.original_type_id=9;loose.runtime_type_id=9;
            loose.absolute_res3_base=3316;loose.compact_record_index=1;
            v.push_back(loose);
        }
        source.levels.push_back(v);
    }
    return source;
}
// Tiny original-format RES archive with valid EH7 LZSS literal streams.
// Unit fixtures never substitute for original game image data.
NativeResArchive source_shape_fixture(bool corrupt=false) {
    std::vector<std::uint8_t> body(20,0);
    body[0]=64;body[8]=127;body[10]=12;
    body[12]=0;body[13]=1; // 0x0100 stop, entry 0
    std::vector<std::uint8_t> eh{'E','H',7,0,0,20,0};
    for(std::size_t at=0;at<body.size();) {
        eh.push_back(0xff);
        for(unsigned k=0;k<8&&at<body.size();++k)
            eh.push_back(body[at++]);
    }
    if(corrupt)eh[2]=6; // no longer original EH type 7
    std::vector<std::uint8_t> res(4+4*4,0);
    res[0]=4;
    for(unsigned id=0;id<4;++id) {
        const auto at=4U+id*4U;
        const auto offset=static_cast<std::uint32_t>(res.size());
        for(unsigned byte=0;byte<4;++byte)
            res[at+byte]=static_cast<std::uint8_t>(offset>>(8U*byte));
        res.insert(res.end(),eh.begin(),eh.end());
    }
    return NativeResArchive(std::move(res));
}
void run() {
    const auto definitions=defs();
    auto a=authored(true);
    auto frames=derive_native_authored_frame_ranges(a,definitions);
    assert(frames.levels.size()==32);
    assert(frames.instances==128);
    assert(frames.npc_source_metadata_records==32);
    assert(frames.init_state_subranges==32);
    assert(frames.loose_item_subranges==64);
    assert(frames.bounded_resource_slots==32*(1+6+2+1));
    assert(frames.frame_ranges_sha256.size()==64);
    assert(frames.per_level_frame_ranges_sha256.size()==32);
    assert(frames.per_level_frame_ranges_sha256[31].size()==64);
    assert(!frames.dos_depth_sort_parity_verified &&
           !frames.dos_view_subframe_parity_verified);

    const auto& row=frames.levels[22];
    assert(row.size()==4);
    assert(row[0].npc_animation_stride==8);
    assert(row[0].npc_cycle_divisor==4);
    assert(row[0].npc_animation_span==3);
    assert(row[0].source_shape_stride==8);
    assert(row[0].state_variant_count==1);
    assert(row[0].resource_slot_count==1);
    assert(row[1].state_variant_count==3);
    assert(row[1].source_shape_stride==2);
    assert(row[1].resource_slot_count==6);
    assert(row[2].source_shape_stride==2);
    assert(row[2].resource_slot_count==2);
    assert(row[2].original_type_id==0x54);
    assert(row[2].runtime_type_id==0x58);
    assert(row[3].resource_slot_count==1);
    assert(row[3].source_shape_stride==0);
    assert(select_native_authored_res3_frame(row[0],definitions.banks,0,0)==37);
    assert(select_native_authored_res3_frame(row[1],definitions.banks,0,0)==2181);
    assert(select_native_authored_res3_frame(row[1],definitions.banks,1,0)==2183);
    assert(select_native_authored_res3_frame(row[1],definitions.banks,2,1)==2186);
    assert(select_native_authored_res3_frame(row[2],definitions.banks,0,1)==3324);
    assert(select_native_authored_res3_frame(row[3],definitions.banks,0,0)==3316);
    rejects([&]{static_cast<void>(select_native_authored_res3_frame(
        row[0],definitions.banks,0,1));});
    rejects([&]{static_cast<void>(select_native_authored_res3_frame(
        row[1],definitions.banks,3,0));});
    rejects([&]{static_cast<void>(select_native_authored_res3_frame(
        row[1],definitions.banks,2,2));});
    rejects([&]{static_cast<void>(select_native_authored_res3_frame(
        row[2],definitions.banks,1,0));});
    rejects([&]{static_cast<void>(select_native_authored_res3_frame(
        row[3],definitions.banks,0,1));});
    // Base-slot evidence is an authentic EH7 *structure*, not a
    // source-live facing, animation variant, or DOS frame claim.
    auto miniature=source_shape_fixture();
    NativeRes3SceneBanks miniature_banks;
    miniature_banks.res3_entries=4;
    miniature_banks.starts={0,1,2,3};
    miniature_banks.lengths={1,1,1,1};
    NativeAuthoredFrameRange probe;
    probe.level=22;probe.source_class=1;
    probe.first_absolute_res3_id=1;
    probe.resource_slot_count=1;
    probe.source_shape_stride=1;
    probe.state_variant_count=1;
    const auto evidence=probe_native_authored_base_eh7(
        probe,miniature_banks,miniature);
    assert(evidence.absolute_res3_id==1U);
    assert(evidence.source_width==64U);
    assert(evidence.x_min==0 && evidence.x_max==0);
    assert(evidence.y_min==0 && evidence.y_max==127);
    assert(evidence.raster_descriptors==1);
    assert(evidence.decoded_body_bytes==20);
    assert(evidence.original_entry_sha256.size()==64);
    assert(evidence.original_body_sha256.size()==64);
    assert(evidence.original_entry_sha256!=evidence.original_body_sha256);
    assert(!evidence.live_view_subframe_verified);
    auto wrong_banks=miniature_banks;
    wrong_banks.res3_entries=5;
    rejects([&]{static_cast<void>(probe_native_authored_base_eh7(
        probe,wrong_banks,miniature));});
    auto wrong_source=probe;wrong_source.first_absolute_res3_id=2;
    rejects([&]{static_cast<void>(probe_native_authored_base_eh7(
        wrong_source,miniature_banks,miniature));});
    auto corrupt=source_shape_fixture(true);
    rejects([&]{static_cast<void>(probe_native_authored_base_eh7(
        probe,miniature_banks,corrupt));});
    const auto json=native_authored_frame_ranges_json(frames);
    assert(json.find("\"bounded_resource_slots\":320")!=std::string::npos);
    assert(json.find("\"original_npc_animation_frame_verified\":false")!=std::string::npos);
    assert(json.find("\"frame_ranges_sha256\":\"")!=std::string::npos);

    auto bad=a;
    bad.levels[3][1].absolute_res3_base+=1;
    rejects([&]{static_cast<void>(
        derive_native_authored_frame_ranges(bad,definitions));});
    bad=a;
    bad.levels[31][0].runtime_type_id=83;
    rejects([&]{static_cast<void>(
        derive_native_authored_frame_ranges(bad,definitions));});
    bad=a;bad.resolved_source_bases_sha256.clear();
    rejects([&]{static_cast<void>(
        derive_native_authored_frame_ranges(bad,definitions));});
    auto broken_defs=definitions;
    broken_defs.object_definitions[0x58].shape_stride=0;
    const auto narrow=derive_native_authored_frame_ranges(a,broken_defs);
    assert(narrow.levels[22][2].resource_slot_count==1);
    assert(narrow.frame_ranges_sha256!=frames.frame_ranges_sha256);
    broken_defs=definitions;
    broken_defs.banks.init_descriptors[0][10].shape_stride=255;
    broken_defs.banks.init_descriptors[0][10].variant_count=255;
    rejects([&]{static_cast<void>(
        derive_native_authored_frame_ranges(a,broken_defs));});
    broken_defs=definitions;
    broken_defs.npc_definitions[9].cycle_divisor=7;
    const auto altered=derive_native_authored_frame_ranges(a,broken_defs);
    assert(altered.frame_ranges_sha256!=frames.frame_ranges_sha256);
    assert(altered.per_level_frame_ranges_sha256[22]!=
           frames.per_level_frame_ranges_sha256[22]);
}
}
int main(){run();}
