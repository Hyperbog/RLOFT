#include "rloft_native/native_world_runtime.hpp"

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <string>
#include <vector>

using namespace rloft_native;
namespace {
void p16(std::vector<std::uint8_t>& b,std::size_t p,unsigned value) {
    b.at(p)=static_cast<std::uint8_t>(value);
    b.at(p+1)=static_cast<std::uint8_t>(value>>8U);
}
void p32(std::vector<std::uint8_t>& b,std::size_t p,unsigned value) {
    for(unsigned k=0;k<4;++k)b.at(p+k)=
        static_cast<std::uint8_t>(value>>(8U*k));
}
template<typename F> void rejects(F task) {
    bool rejected=false;
    try{task();}catch(const std::exception&){rejected=true;}
    assert(rejected);
}
std::vector<std::uint8_t> original_fixture(bool alternate=false) {
    constexpr unsigned b0=29,w=2,h=2;
    constexpr unsigned b1=b0+0x2A4+w*h*2;
    constexpr unsigned b2=b1+2*(2+10);
    constexpr unsigned b3=b2+11,b4=b3+11,b5=b4+11,b6=b5+50*26;
    std::vector<std::uint8_t> record(b6+4+4+4,0);
    constexpr unsigned offsets[]{b0,b1,b2,b3,b4,b5,b6};
    for(unsigned n=0;n<7;++n)p32(record,1U+4U*n,offsets[n]);
    p16(record,b0+1,2);
    p16(record,b0+3,2);
    p16(record,b0+0x28A,1);
    p16(record,b0+0x28C,1);
    p16(record,b0+0x28E,1);
    p16(record,b0+0x2A4,(alternate?0x0480:0x0080) | 0x0007);
    p16(record,b0+0x2A6,0x1003);
    p16(record,b0+0x2A8,0x0000);
    p16(record,b0+0x2AA,0x2000);
    p16(record,b1,1); // column 0, one kind-1 record
    p16(record,b1+12,1); // column 1, one kind-2 record
    record[b1+2]=1;       // kind 1
    record[b1+3]=0x05;    // control flags: mode1, eligible
    record[b1+2+4]=0;     // row
    record[b1+2+5]=0;     // column
    record[b1+2+6]=8;     // original closed endpoint
    record[b1+2+7]=0;     // stationary
    record[b1+14]=2;      // kind 2
    record[b1+15]=0x06;   // low two bits = 2, + eligibility
    record[b1+14+4]=0;
    record[b1+14+5]=1;
    record[b5]=1;
    record[b5+1]=5;
    p16(record,b6,2);
    p16(record,b6+2,2);
    record[b6+4]=7;
    record[b6+5]=8;
    record[b6+6]=9;
    record[b6+7]=10;
    std::vector<std::uint8_t> archive(132,0);
    p32(archive,0,32);
    for(unsigned level=0;level<32;++level) {
        p32(archive,4U+4U*level,static_cast<unsigned>(archive.size()));
        archive.insert(archive.end(),record.begin(),record.end());
    }
    return archive;
}

void run() {
    const auto original=original_fixture();
    NativeWorldRuntime world(decode_native_world_payload(original));
    assert(world.active_level()==0);
    assert(world.original().levels.size()==32);
    const auto before=world.evidence_json();
    assert(before.find("\"total_cells\":128")!=std::string::npos);
    assert(before.find("\"kind1_dynamic_records\":32")!=std::string::npos);
    assert(before.find("\"gameplay_parity_verified\":false")!=std::string::npos);

    const auto barrier=world.cell(22,0,0);
    assert(barrier.code==7);
    assert(barrier.special_center);
    assert(barrier.dynamic_position==8);
    assert(barrier.normal_collision==NativeCollision::SpecialBlocked);
    assert(world.cell(22,1,0).resource_variant_offset==2);
    assert(world.cell(22,1,0).normal_collision==
           NativeCollision::OrdinaryBlocked);
    assert(world.cell(22,0,1).normal_collision==NativeCollision::Passable);
    assert(world.cell(22,1,1).authoring_marker);
    assert(world.cell(22,1,1).normal_collision==NativeCollision::Passable);

    world.select_level(22);
    auto baseline=world.snapshot();
    assert(baseline.active_level==22);
    assert(baseline.source_fingerprint_sha256.size()==64);
    world.replace_cell_code(22,1,1,31);
    assert(world.cell(22,1,1).code==31);
    assert(world.cell(22,1,1).authoring_marker);
    rejects([&]{world.replace_cell_code(22,1,1,64);});
    rejects([&]{world.replace_cell_code(22,2,1,1);});
    rejects([&]{world.select_level(32);});

    world.mark_reached_by_view(22,0,1);
    world.mark_reached_by_view(22,1,1);
    assert(world.cell(22,0,1).transient_visible);
    assert(world.cell(22,0,1).explored);
    assert(world.marked_scene_cells(22,2).size()==2);
    rejects([&]{static_cast<void>(world.marked_scene_cells(22,1));});
    world.clear_transient_visibility(22);
    assert(!world.cell(22,0,1).transient_visible);
    assert(world.cell(22,0,1).explored);
    assert(world.marked_scene_cells(22,0).empty());
    assert(!world.cell(21,0,1).explored);

    // Proven original animation +6/+7 state update, no in-game interaction.
    world.debug_begin_dynamic_transition(22,0);
    rejects([&]{world.debug_begin_dynamic_transition(22,0);});
    for(int remaining=7;remaining>=0;--remaining) {
        assert(world.advance_dynamic_animations(22)==
               (remaining==0?0U:1U));
    }
    assert(world.cell(22,0,0).dynamic_position==0);
    assert(world.normal_collision(22,0,0)==NativeCollision::Passable);
    world.debug_begin_dynamic_transition(22,0);
    for(int remaining=7;remaining>=0;--remaining)
        static_cast<void>(world.advance_dynamic_animations(22));
    assert(world.cell(22,0,0).dynamic_position==8);
    assert(world.normal_collision(22,0,0)==NativeCollision::SpecialBlocked);
    rejects([&]{world.debug_begin_dynamic_transition(22,1);});
    rejects([&]{world.debug_begin_dynamic_transition(22,22);});

    world.restore(baseline);
    assert(world.active_level()==22);
    assert(world.cell(22,1,1).code==0);
    assert(world.cell(22,0,0).dynamic_position==8);
    assert(!world.cell(22,0,1).explored);
    assert(world.original().level(22).cell(1,1)==0x2000);

    // Source-bound snapshot restore and atomic failure.
    NativeWorldRuntime other(decode_native_world_payload(original_fixture(true)));
    rejects([&]{other.restore(baseline);});
    auto corrupted=baseline;
    corrupted.controls[22][0].bytes[5]=1;
    rejects([&]{world.restore(corrupted);});
    corrupted=baseline;
    corrupted.controls[22][0].bytes[6]=9;
    rejects([&]{world.restore(corrupted);});
    corrupted=baseline;
    corrupted.maps.levels[22].cell_words.pop_back();
    rejects([&]{world.restore(corrupted);});
    assert(world.cell(22,0,0).dynamic_position==8);
    assert(world.cell(22,1,1).code==0);
}
}
int main() {run();}
