#include "rloft_native/native_scene_target.hpp"
#include <cassert>
#include <cmath>
#include <stdexcept>
#include <vector>

using namespace rloft_native;
namespace {
NativeWorldRuntime fixture() {
    NativeWorldPayload source;
    source.levels.reserve(32);
    for(unsigned level=0;level<32;++level) {
        NativeLevelPayload map;
        map.id=level;
        map.width=8;map.height=8;
        map.cell_words.assign(64,0);
        map.automap.assign(64,0);
        source.total_cells+=64;
        source.levels.push_back(std::move(map));
    }
    // Ordinary CODE.2 six-bit blocked wall.
    source.levels[22].cell_words[1U*8U+3U]=1;
    return NativeWorldRuntime(std::move(source));
}
NativeAuthoredSceneInstance at(unsigned level,unsigned type,
                              int x16,int y16,unsigned source_class=1) {
    NativeAuthoredSceneInstance object;
    object.level=level;
    object.source_class=source_class;
    object.original_type_id=static_cast<std::uint16_t>(type);
    object.runtime_type_id=object.original_type_id;
    object.x_position_16_16=x16;
    object.y_position_16_16=y16;
    object.cell_column=unsigned(x16/65536);
    object.cell_row=unsigned(y16/65536);
    return object;
}
template<class F> void rejects(F task) {
    bool rejected=false;
    try{task();}catch(const std::exception&){rejected=true;}
    assert(rejected);
}
} // namespace
int main() {
    auto world=fixture();
    world.select_level(22);
    const auto before=world.snapshot();
    // Two genuine-layout 16.16 placements; nearest crosshair angle
    // wins; ties select the closer placement.
    const auto straight=at(22,62,2*65536+32768,2*65536+32768);
    const auto other=at(22,63,2*65536+32768,3*65536+32768);
    const auto near=at(22,9,1*65536+32768,2*65536+32768,0);
    const std::vector<NativeAuthoredSceneInstance> source{other,straight,near};
    auto chosen=select_native_authored_scene_target(
        world,22,source,1.5,2.5,0.,2.5,0.5);
    assert(chosen && chosen->original.original_type_id==9U);
    assert(chosen->distance_cells>0.0 && chosen->distance_cells<0.1);
    // Do not allow targets outside the cone or radius.
    assert(!select_native_authored_scene_target(
        world,22,std::vector<NativeAuthoredSceneInstance>{other},
        1.5,2.5,0.,2.5,0.3));
    assert(!select_native_authored_scene_target(
        world,22,std::vector<NativeAuthoredSceneInstance>{straight},
        1.5,2.5,0.,0.5,0.5));
    // Source wall (3,1) obstructs a farther candidate on this ray.
    const auto far=at(22,64,4*65536+32768,1*65536+32768);
    assert(!select_native_authored_scene_target(
        world,22,{far},1.5,1.5,0.,8.,0.5));
    // One valid target at blocked endpoint is still an inspector candidate.
    const auto endpoint=at(22,64,3*65536+32768,1*65536+32768);
    assert(select_native_authored_scene_target(
        world,22,{endpoint},2.5,1.5,0.,2.5,0.5));
    assert(!select_native_authored_scene_target(
        world,22,{near},1.5,2.5,3.141592653589793,2.5,0.5));
    rejects([&]{static_cast<void>(select_native_authored_scene_target(
        world,22,{near},-1.,1.5,0.));});
    rejects([&]{static_cast<void>(select_native_authored_scene_target(
        world,22,{near},1.5,1.5,0.,99.));});
    rejects([&]{static_cast<void>(select_native_authored_scene_target(
        world,22,{at(21,9,98304,163840)},1.5,1.5,0.));});
    auto malformed=near;
    malformed.cell_column=3;
    rejects([&]{static_cast<void>(select_native_authored_scene_target(
        world,22,{malformed},1.5,1.5,0.));});
    assert(world.snapshot().source_fingerprint_sha256==
           before.source_fingerprint_sha256);
    assert(world.cell(22,3,1).code==1);
    assert(world.active_level()==22);
}
