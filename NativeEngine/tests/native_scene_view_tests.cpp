#include "rloft_native/native_scene_view.hpp"

#include <cassert>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <string>
#include <vector>

using namespace rloft_native;
namespace {
template<class Fn> void rejects(Fn f) {
    bool failed=false;
    try{f();}catch(const std::exception&){failed=true;}
    assert(failed);
}
NativeWorldPayload synthetic_world() {
    NativeWorldPayload w;
    for(unsigned id=0;id<32;++id) {
        NativeLevelPayload level;
        level.id=id;level.width=6;level.height=6;
        level.cell_words.assign(36,0);
        level.automap.assign(36,0);
        for(unsigned i=0;i<6;++i) {
            level.cell_words[i]=12;
            level.cell_words[30+i]=12;
            level.cell_words[i*6]=12;
            level.cell_words[i*6+5]=12;
        }
        // A special-center dynamic collision on the eastward sightline.
        level.cell_words[3*6+4]=0x0080|7;
        // Frame cell is normally passable but has a group-1 state record.
        level.cell_words[3*6+3]=0x0100|0x1000;
        for(unsigned bank=0;bank<5;++bank)
            for(unsigned code=0;code<64;++code) {
                const auto value=unsigned(100*bank+2*code);
                const auto at=0x00A+128U*bank+2U*code;
                level.original_header[at]=static_cast<std::uint8_t>(value);
                level.original_header[at+1]=static_cast<std::uint8_t>(value>>8U);
            }
        level.original_header[0x290]=0xff;
        NativeOriginalControl state{};
        state.column=3;
        state.bytes[0]=2;
        state.bytes[1]=0x06; // state resource variant offset 2
        state.bytes[4]=3;state.bytes[5]=3;
        level.controls.push_back(state);
        NativeOriginalControl dynamic{};
        dynamic.column=4;
        dynamic.bytes[0]=1;
        dynamic.bytes[1]=0x05;
        dynamic.bytes[4]=3;dynamic.bytes[5]=4;
        dynamic.bytes[6]=8; // closed
        level.controls.push_back(dynamic);
        w.total_cells+=36;
        w.total_control_records+=2;
        w.levels.push_back(std::move(level));
    }
    return w;
}
void run() {
    NativeWorldRuntime world(synthetic_world());
    const auto& level=world.original().level(22);
    assert(level.resource_lookup(0,0)==0);
    assert(level.resource_lookup(1,5)==110);
    assert(level.resource_lookup(2,63)==326);
    assert(level.resource_lookup(4,15)==430);
    assert(level.original_header[0x290]==255);
    rejects([&]{static_cast<void>(level.resource_lookup(5,0));});
    rejects([&]{static_cast<void>(level.resource_lookup(0,64));});

    const auto simple=native_scene_resource_candidates(world,22,3,3);
    assert(simple.original_code==0 && simple.effective_code==0);
    assert(simple.state_record_found && !simple.direction_neighbor_substituted);
    assert(simple.selected_variant_offset==2);
    assert(simple.candidate_resource_ids[0]==2);
    assert(simple.candidate_resource_ids[3]==302);
    assert(!simple.selection_branch_verified);
    const auto selected=native_scene_resource_candidates(
        world,22,3,3,NativeCardinal::East);
    assert(selected.original_code==0 && selected.effective_code==7);
    assert(selected.direction_neighbor_substituted);
    assert(selected.selected_variant_offset==1);
    assert(selected.raw_lookup[0]==14);
    assert(selected.candidate_resource_ids[0]==15);
    assert(selected.candidate_resource_ids[2]==215);
    const auto no_override=native_scene_resource_candidates(
        world,22,3,3,NativeCardinal::North);
    assert(!no_override.direction_neighbor_substituted);
    assert(no_override.candidate_resource_ids[0]==2);
    const auto neutral=native_scene_resource_candidates(world,22,2,3);
    assert(neutral.selected_variant_offset==-1);
    assert(neutral.candidate_resource_ids[1]==100);

    NativeProbeCamera camera{};
    camera.level=22;camera.column_position=3.5;
    camera.row_position=3.5;camera.heading_radians=0;
    camera.field_of_view_radians=0.6;
    const auto before=world.snapshot();
    const auto view=trace_native_probe_view(world,camera,5,64);
    assert(view.columns.size()==5);
    assert(view.rays_that_hit==5);
    assert(!view.original_renderer_verified);
    assert(view.columns[2].hit && view.columns[2].side==0);
    assert(view.columns[2].hit_column==4 && view.columns[2].hit_row==3);
    assert(view.columns[2].hit_collision==NativeCollision::SpecialBlocked);
    assert(std::abs(view.columns[2].corrected_distance-0.5)<0.00001);
    assert(view.columns[2].reached.size()==1);
    const auto pixels=render_native_probe_rgb(view,5,16);
    assert(pixels.size()==5U*16U*3U);
    rejects([&]{static_cast<void>(render_native_probe_rgb(view,6,16));});

    apply_native_probe_visibility(world,view);
    assert(world.cell(22,4,3).transient_visible);
    assert(world.cell(22,4,3).explored);
    assert(world.original().level(22).cell(4,3)==(0x0080|7));
    world.clear_transient_visibility(22);
    assert(!world.cell(22,4,3).transient_visible);
    assert(world.cell(22,4,3).explored);
    assert(!world.cell(21,4,3).explored);

    // The interactive alpha requires an actual footprint: a blocked
    // movement must never change camera position or subsequently stretch
    // its projected first-person frame.
    auto protected_camera=camera;
    const auto near_wall=move_native_probe_camera_clearance(
        world,protected_camera,0.8,0.0);
    assert(near_wall.moved && near_wall.blocked);
    assert(protected_camera.column_position>=3.7);
    assert(protected_camera.column_position<=3.75);
    assert(std::abs(protected_camera.row_position-3.5)<1e-9);
    // A large step may stop partway through its sweep. Additional
    // attempts may advance the remaining free clearance, but once
    // contact is reached the final pose and its render must be stable.
    bool at_contact=false;
    for(unsigned attempt=0;attempt<12;++attempt) {
        const auto retry=move_native_probe_camera_clearance(
            world,protected_camera,0.18,0.0);
        assert(retry.blocked);
        if(!retry.moved) {at_contact=true;break;}
    }
    assert(at_contact);
    const auto contact_x=protected_camera.column_position;
    const auto contact_y=protected_camera.row_position;
    const auto contact_view=trace_native_probe_view(
        world,protected_camera,5,64);
    const auto contact_frame=render_native_probe_rgb(contact_view,5,16);
    const auto refused=move_native_probe_camera_clearance(
        world,protected_camera,0.18,0.0);
    assert(refused.blocked && !refused.moved);
    assert(protected_camera.column_position==contact_x);
    assert(protected_camera.row_position==contact_y);
    assert(render_native_probe_rgb(
        trace_native_probe_view(world,protected_camera,5,64),5,16)==
        contact_frame);
    const auto slide=move_native_probe_camera_clearance(
        world,protected_camera,0.18,0.12);
    assert(slide.moved && slide.blocked); // tangent slide along wall
    assert(protected_camera.column_position==contact_x);
    assert(protected_camera.row_position>contact_y);
    rejects([&] {static_cast<void>(move_native_probe_camera_clearance(
        world,protected_camera,0.2,0.0,0.49));});

    const auto right=native_relative_movement(0.0,0.0,1.0,2.0,0.05);
    assert(std::abs(right.delta_rows+0.1)<1e-9);
    auto steep_look=camera;
    steep_look.pitch_radians=1.40;
    const auto pitch_motion=move_native_probe_camera_clearance(
        world,steep_look,0.0,0.0);
    assert(!pitch_motion.moved);
    steep_look.pitch_radians=1.43;
    rejects([&]{static_cast<void>(move_native_probe_camera_clearance(
        world,steep_look,0.0,0.0));});
    const auto original_x=camera.column_position;
    assert(!move_native_probe_camera(world,camera,1.1,0.0));
    assert(camera.column_position==original_x); // all-or-nothing blocked
    rejects([&]{static_cast<void>(move_native_probe_camera(
        world,camera,999.0,0));});

    // Experimental host camera passes only after instruction-proven
    // kind-1 dynamic record reaches its passable endpoint.
    world.debug_begin_dynamic_transition(22,1);
    for(unsigned n=0;n<8;++n)
        static_cast<void>(world.advance_dynamic_animations(22));
    assert(world.cell(22,4,3).dynamic_position==0);
    const auto opened=trace_native_probe_view(world,camera,1,64);
    assert(opened.columns[0].hit_column==5);
    assert(opened.columns[0].hit_row==3);
    assert(opened.columns[0].hit_code==12);
    assert(opened.columns[0].corrected_distance>1.4 &&
           opened.columns[0].corrected_distance<1.6);
    assert(move_native_probe_camera(world,camera,1.0,0.0));
    assert(std::abs(camera.column_position-4.5)<0.00001);
    assert(!move_native_probe_camera(world,camera,1.0,0.0));
    world.restore(before);
    assert(world.cell(22,4,3).dynamic_position==8);
    assert(!world.cell(22,4,3).explored);

    rejects([&]{static_cast<void>(trace_native_probe_view(
        world,camera,0,64));});
    rejects([&]{static_cast<void>(trace_native_probe_view(
        world,camera,641,64));});
    rejects([&]{static_cast<void>(trace_native_probe_view(
        world,camera,3,0));});
    auto bad=camera;bad.column_position=-1;
    rejects([&]{static_cast<void>(trace_native_probe_view(world,bad,3,64));});
    bad=camera;bad.field_of_view_radians=3.14159265358979323846;
    rejects([&]{static_cast<void>(trace_native_probe_view(world,bad,3,64));});
    // Collision from source state must prevent starting *in* blocking cell.
    bad=camera;bad.column_position=4.5;
    rejects([&]{static_cast<void>(trace_native_probe_view(world,bad,3,64));});
}
}
int main() {run();}
