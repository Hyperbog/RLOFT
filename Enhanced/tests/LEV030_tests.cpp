#include "rloft/LEV030.hpp"

#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <fstream>
#include <iterator>
#include <stdexcept>
#include <vector>

namespace {
using Group = rloft::Lev019ControlGroup;

void require(bool ok, const char* detail) {
    if (!ok) throw std::runtime_error(detail);
}
template <class Action> void rejects(Action action) {
    bool failed = false;
    try { action(); } catch (const std::exception&) { failed = true; }
    require(failed, "LEV030 accepted invalid or stale change");
}
void put16(std::vector<std::uint8_t>& b, std::size_t at, unsigned v) {
    b.at(at) = static_cast<std::uint8_t>(v);
    b.at(at+1U) = static_cast<std::uint8_t>(v >> 8U);
}
void put32(std::vector<std::uint8_t>& b, std::size_t at, unsigned v) {
    for (unsigned i=0; i<4; ++i)
        b.at(at+i) = static_cast<std::uint8_t>(v >> (8U*i));
}
std::vector<std::uint8_t> synthetic() {
    constexpr unsigned w=4U,h=3U;
    constexpr std::size_t header=0x1DU,grid=header+0x2A4U;
    constexpr std::size_t b1=grid+2U*w*h;
    constexpr std::size_t b2=b1+2U*w+10U+1U+2U*w;
    constexpr std::size_t b3=b2+22U,b4=b3+2U,b5=b4+2U*22U;
    constexpr std::size_t b6=b5+0x514U;
    constexpr std::array<std::size_t,7> bounds{{header,b1,b2,b3,b4,b5,b6}};
    std::vector<std::uint8_t> b(b6+4U+w*h+4U,0U);
    b[0]=0x9AU; // Writer-unowned marker.
    for (std::size_t i=0;i<bounds.size();++i)
        put32(b,1U+4U*i,static_cast<unsigned>(bounds[i]));
    put16(b,header+1U,w);put16(b,header+3U,h);
    put16(b,header+0x28AU,1U); // NPC family.
    put16(b,header+0x28CU,2U); // Loose-item family.
    put16(b,header+0x28EU,0U); // Placed family.
    put16(b,header+0x29AU,1U); // Saved render threshold.
    b[header+0x29CU]=41U; // Music.
    std::size_t cur=b1;
    for(unsigned c=0;c<w;++c){
        put16(b,cur,c==1U ? 1U:0U);cur+=2U;
        if(c==1U){b[cur]=8U;b[cur+4U]=1U;b[cur+5U]=1U;cur+=10U;}
    }
    b[cur++]=0U; // Active animation count.
    for(unsigned c=0;c<w;++c){put16(b,cur,0U);cur+=2U;}
    require(cur==b2,"synthetic controls invalid");
    put32(b,b2,(1U<<16U)|0x8000U); // NPC y=1.5
    put32(b,b2+4U,(1U<<16U)|0x6000U); // NPC x=1.375
    put16(b,b2+12U,w+1U);put16(b,b2+14U,55U);
    put16(b,b2+18U,0U);put16(b,b2+20U,16U);
    put16(b,b3,0U); // Placed family inline count.
    for(unsigned i=0;i<2U;++i){
        const auto at=b4+22U*i;
        put32(b,at,(1U<<16U)|0x4000U); // y=1.25
        put32(b,at+4U,((1U+i)<<16U)|0x5000U); // x=1.3125/2.3125
        put16(b,at+12U,w+1U+i);put16(b,at+14U,164U);
    }
    put16(b,b6,w);put16(b,b6+2U,h);
    std::fill(b.begin()+static_cast<std::ptrdiff_t>(b6+4U),
              b.begin()+static_cast<std::ptrdiff_t>(b6+4U+w*h),0xFFU);
    put32(b,b6+4U+w*h,0U); // No annotations.
    return b;
}
void exercise(const std::vector<std::uint8_t>& original,bool retail) {
    rloft::Lev030File file(original);
    const auto summary=file.summary();
    require(file.encode()==original && summary.world_counts[0]==1U &&
            summary.world_counts[1]==0U && summary.group1_controls==1U &&
            summary.group2_controls==0U && summary.annotation_bytes==0U &&
            summary.active_event_slots==0U && summary.active_event_commands==0U &&
            summary.music_sequence==41U && file.serialized_render_floor()==1,
            "LEV030 file structure or lossless decode differs");
    if (retail) {
        require(original.size()==4246U && summary.width==24 &&
                summary.height==24 && summary.world_counts[2]==17U &&
                file.npc(0U).npc_type_id()==55U &&
                file.npc(0U).x_position_16_16()==0x00016000U &&
                file.npc(0U).y_position_16_16()==0x00098000U &&
                file.npc(0U).cached_cell_index()==217U &&
                file.loose_item(0U).x_position_16_16()==0x0005F400U &&
                file.loose_item(0U).y_position_16_16()==0x00027800U,
                "LEV030 uploaded original identity does not match");
        const auto state=file.decode();
        require(std::count_if(state.cell_words.begin(),state.cell_words.end(),
            [](std::uint16_t x){return (x&0x0400U)!=0U;})==361 &&
            std::count_if(state.cell_words.begin(),state.cell_words.end(),
            [](std::uint16_t x){return (x&0x0040U)!=0U;})==2,
            "LEV030 exploration state differs");
        for(std::size_t i=0;i<17;++i)
            require(file.loose_item(i).object_type_id()==164U,
                    "LEV030 loose-item type or order differs");
    } else {
        require(summary.width==4 && summary.height==3 &&
                summary.world_counts[2]==2U,"synthetic LEV030 invalid");
    }
    const auto control_column=retail ? 23 : 1;
    auto controls=file.controls(Group::Primary,control_column);
    require(controls.size()==1U && controls[0].raw[0]==8U &&
            controls[0].column()==control_column,
            "LEV030 original control mismatch");
    const auto original_npc=file.npc(0U);
    const auto loose0=file.loose_item(0U),loose1=file.loose_item(1U);
    const auto cache=file.decode().automap_cache;
    const auto script=file.decode().event_scripts;
    const auto old_x=original_npc.x_position_16_16();
    const auto old_y=original_npc.y_position_16_16();
    const auto pos0x=loose0.x_position_16_16(),pos0y=loose0.y_position_16_16();
    const auto pos1x=loose1.x_position_16_16(),pos1y=loose1.y_position_16_16();
    auto newer_controls=controls;
    newer_controls[0].raw[7U]^=1U;

    rloft::Lev030Edits update{};
    update.state.state.render_floor=rloft::Lev026RenderFloorPatch{1,0};
    update.state.state.level.world.controls={
        {Group::Primary,control_column,controls,newer_controls}};
    update.npc_positions={{0U,original_npc,old_x+0x10000U,old_y+0x10000U}};
    update.loose_positions={{0U,loose0,pos0x+0x0400U,pos0y+0x0800U},
                            {1U,loose1,pos1x+0x10000U,pos1y+0x10000U}};
    file.apply(update);
    const auto changed_npc=file.npc(0U);
    const auto changed0=file.loose_item(0U),changed1=file.loose_item(1U);
    require(file.serialized_render_floor()==0 &&
            changed_npc.x_position_16_16()==old_x+0x10000U &&
            changed_npc.y_position_16_16()==old_y+0x10000U &&
            changed_npc.cached_cell_index()==original_npc.cached_cell_index()+summary.width+1U &&
            changed0.x_position_16_16()==pos0x+0x0400U &&
            changed0.y_position_16_16()==pos0y+0x0800U &&
            changed0.cached_cell_index()==loose0.cached_cell_index() &&
            changed1.x_position_16_16()==pos1x+0x10000U &&
            changed1.cached_cell_index()==loose1.cached_cell_index()+summary.width+1U &&
            file.controls(Group::Primary,control_column)[0].raw==newer_controls[0].raw &&
            file.encode().size()==original.size() &&
            file.summary().boundaries==summary.boundaries &&
            file.bytes()[0]==original[0] &&
            file.decode().automap_cache==cache &&
            file.decode().event_scripts==script,
            "LEV030 fixed-point transaction corrupted source");
    auto checkpoint=file.encode();
    rejects([&]{file.apply(update);}); // Stale source images and renderer.
    auto bad=rloft::Lev030Edits{};
    bad.npc_positions={{0U,changed_npc,0xFFFF0000U,old_y}};
    rejects([&]{file.apply(bad);}); // Off-map x.
    bad.npc_positions={{0U,changed_npc,old_x,0xFFFF0000U}};
    rejects([&]{file.apply(bad);}); // Off-map y.
    bad.npc_positions={{0U,changed_npc,old_x,old_y},
                       {0U,changed_npc,old_x,old_y}};
    rejects([&]{file.apply(bad);}); // Duplicate entity.
    bad={};
    bad.loose_positions={{0U,changed0,0xFFFF0000U,pos0y}};
    rejects([&]{file.apply(bad);});
    bad.loose_positions={{0U,changed0,pos0x,pos0y},
                         {0U,changed0,pos0x,pos0y}};
    rejects([&]{file.apply(bad);});
    bad={};
    bad.npc_positions={{0U,original_npc,old_x,old_y}};
    rejects([&]{file.apply(bad);}); // Stale pre-edit actor.
    bad={};
    bad.loose_positions={{0U,loose0,pos0x,pos0y}};
    rejects([&]{file.apply(bad);}); // Stale pre-edit instance.
    bad={};
    bad.npc_positions={{0U,changed_npc,old_x,old_y}};
    bad.state.state.render_floor=rloft::Lev026RenderFloorPatch{1,1};
    rejects([&]{file.apply(bad);}); // Valid position, stale header.
    require(file.encode()==checkpoint,"LEV030 rejected edits changed bytes");

    rloft::Lev030Edits undo{};
    undo.state.state.render_floor=rloft::Lev026RenderFloorPatch{0,1};
    undo.state.state.level.world.controls={
        {Group::Primary,control_column,newer_controls,controls}};
    undo.npc_positions={{0U,changed_npc,old_x,old_y}};
    undo.loose_positions={{0U,changed0,pos0x,pos0y},
                          {1U,changed1,pos1x,pos1y}};
    file.apply(undo);
    require(file.encode()==original,"LEV030 inverse did not restore exact source");
    file.apply({});
    require(file.encode()==original,"LEV030 empty edit changed file");
    rejects([&]{(void)file.loose_item(summary.world_counts[2]);});
    auto truncated=original;truncated.pop_back();
    rejects([&]{(void)rloft::Lev030File(truncated);});
}
} // namespace

int main(int argc,char** argv) {
    exercise(synthetic(),false);
    if(argc>2)throw std::invalid_argument("usage: LEV030_tests [LEV030.DAT]");
    if(argc==2){
        std::ifstream input(argv[1],std::ios::binary);
        if(!input)throw std::runtime_error("cannot open original LEV030.DAT");
        const std::vector<std::uint8_t> b{
            std::istreambuf_iterator<char>(input),std::istreambuf_iterator<char>()};
        exercise(b,true);
    }
}
