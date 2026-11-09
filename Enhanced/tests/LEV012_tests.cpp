#include "rloft/LEV012.hpp"

#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <fstream>
#include <iterator>
#include <stdexcept>
#include <vector>

namespace {
void require(bool good,const char* why) {if(!good)throw std::runtime_error(why);}
template<class Fn> void rejects(Fn&& f) {
    bool failed=false;try{f();}catch(const std::exception&){failed=true;}
    require(failed,"invalid LEV012 operation unexpectedly succeeded");
}
void put16(std::vector<std::uint8_t>& b,std::size_t at,std::uint16_t n) {
    b.at(at)=static_cast<std::uint8_t>(n);
    b.at(at+1U)=static_cast<std::uint8_t>(n>>8U);
}
void put32(std::vector<std::uint8_t>& b,std::size_t at,std::uint32_t n) {
    for(unsigned i=0;i<4U;++i)b.at(at+i)=static_cast<std::uint8_t>(n>>(8U*i));
}
std::vector<std::uint8_t> synthetic(){
    constexpr std::size_t begin=0x1DU,grid=begin+0x2A4U;
    constexpr std::size_t width=3U,height=2U;
    constexpr std::size_t b1=grid+width*height*2U;
    constexpr std::size_t b2=b1+2U*width+1U+2U*width;
    constexpr std::size_t b3=b2+22U,b4=b3+2U,b5=b4,b6=b5+0x514U;
    std::vector<std::uint8_t> data(b6+4U+width*height+4U,0U);
    data[0]=0xDFU; // Writer-unowned original byte must survive.
    const std::array<std::size_t,7> borders{{begin,b1,b2,b3,b4,b5,b6}};
    for(std::size_t i=0;i<7U;++i)put32(data,1U+4U*i,static_cast<unsigned>(borders[i]));
    put16(data,begin+1U,width);put16(data,begin+3U,height);
    put16(data,begin+0x28AU,1U);
    data[begin+0x29CU]=12U;
    // Original NPC: y=1.25, x=2.5, cache cell=5, type=31,
    // HP=-4, countdown=7, flags=0x10000000, state=0x18.
    put32(data,b2+0U,0x00014000U);put32(data,b2+4U,0x00028000U);
    put32(data,b2+8U,0x10000000U);put16(data,b2+12U,5U);
    put16(data,b2+14U,31U);put16(data,b2+16U,7U);
    put16(data,b2+18U,0xFFFCU);put16(data,b2+20U,0x18U);
    put16(data,b3,0U);
    put16(data,b6,width);put16(data,b6+2U,height);
    put32(data,b6+4U+width*height,0U);
    return data;
}
void exercise(const std::vector<std::uint8_t>& original,bool retail){
    rloft::Lev012File file(original);
    require(file.encode()==original,"source round trip differed");
    require(file.decode().storage==rloft::LevelStorage::DirectSave,
            "direct-save storage type changed");
    const auto s=file.summary();
    if(retail){
        require(original.size()==19848U && s.width==64 && s.height==64 &&
                s.world_counts==std::array<std::size_t,3>{{30U,150U,19U}} &&
                s.group1_controls==82U && s.group2_controls==9U &&
                s.active_event_slots==21U && s.active_event_commands==40U &&
                s.active_animation_count==1U && s.annotation_bytes==0U &&
                s.music_sequence==12U,"retail Level12 inventory changed");
        const auto first=file.npc(0U);
        require(first.npc_type_id()==78U && first.current_hp()==0 &&
                first.runtime_flags()==0x80000000U &&
                first.cached_cell_index()==1425U,"retail NPC0 changed");
    }else{
        require(s.world_counts[0]==1U && file.npc(0).npc_type_id()==31U &&
                file.npc(0).current_hp()==-4 &&
                file.npc(0).cached_cell_index()==5U &&
                file.npc(0).packed_runtime_state()==0x18U,
                "synthetic NPC image not decoded correctly");
    }
    const auto npc_count=s.world_counts[0];
    const auto before=file.npc(0U);
    auto modified=before;
    modified.set_current_hp(static_cast<std::int16_t>(before.current_hp()+1));
    modified.set_action_state_countdown(17U);
    modified.set_runtime_flags(before.runtime_flags() ^ 0x00020000U);
    // A second independent original NPC in the same atomic transaction.
    const std::size_t other=retail?8U:0U;
    const auto second=file.npc(other);
    auto other_modified=second;
    other_modified.set_current_hp(static_cast<std::int16_t>(second.current_hp()-1));
    if(retail){
        file.apply_npc_patches({{0U,before,modified},{other,second,other_modified}});
        require(file.npc(other).raw==other_modified.raw,"second NPC edit missing");
    }else{file.apply_npc_patches({{0U,before,modified}});}
    require(file.npc(0U).raw==modified.raw,"first NPC edit not applied");
    const auto encoded=file.encode();
    const rloft::Lev012File reopened(encoded);
    require(reopened.npc(0U).raw==modified.raw,"edited NPC did not reload");
    for(std::size_t i=0;i<original.size();++i){
        const auto first_at=static_cast<std::size_t>(s.boundaries[2]);
        const bool first_changed=i>=first_at && i<first_at+22U;
        const bool second_changed=retail && i>=first_at+other*22U &&
                                  i<first_at+(other+1U)*22U;
        if(!first_changed&&!second_changed)
            require(original[i]==encoded[i],"NPC patch changed unrelated bytes");
    }
    const auto unchanged=file.encode();
    rejects([&]{file.apply_npc_patches({{0U,before,modified}});});
    rejects([&]{file.apply_npc_patches({{0U,modified,before},
                                       {0U,modified,before}});});
    rejects([&]{file.apply_npc_patches({{npc_count,before,modified}});});
    auto bad=modified;
    // Change high x word without updating cached cell.
    bad.raw[6U]=0xFFU;
    rejects([&]{file.apply_npc_patches({{0U,modified,bad}});});
    require(file.encode()==unchanged,"rejected NPC batch modified live file");
    if(retail)file.apply_npc_patches({{0U,modified,before},
                                     {other,other_modified,second}});
    else file.apply_npc_patches({{0U,modified,before}});
    require(file.encode()==original,"NPC edits not reversible exactly");
    // A valid 16.16 movement updates the source cached-cell index with it.
    auto moved=before;
    const auto x=before.x_position_16_16();
    const auto y=before.y_position_16_16();
    const auto shifted_x=x-0x10000U; // source x is >=1 in both fixtures
    const auto next_cell=static_cast<std::uint16_t>(
        (y>>16U)*static_cast<unsigned>(s.width)+(shifted_x>>16U));
    moved.set_position_16_16(shifted_x,y,next_cell);
    file.apply_npc_patches({{0U,before,moved}});
    require(file.npc(0U).cached_cell_index()==next_cell &&
            file.npc(0U).x_position_16_16()==shifted_x,
            "position edit failed to update cached map cell");
    file.apply_npc_patches({{0U,moved,before}});
    require(file.encode()==original,"NPC position not reversible exactly");
    const auto original_event=file.event_slot(0);
    auto changed_event=original_event;
    changed_event.raw[0]=1U;changed_event.raw[1]=2U;
    file.replace_event_slot(0,changed_event);
    file.replace_event_slot(0,original_event);
    require(file.encode()==original,"event editor changed unrelated state");
    const auto cell=file.cell_word(0,0);
    file.apply_cell_patches({{0,0,cell,static_cast<std::uint16_t>(cell^0x0440U)}});
    file.apply_cell_patches({{0,0,static_cast<std::uint16_t>(cell^0x0440U),cell}});
    require(file.encode()==original,"map-cell edit not reversible");
    if(retail){
        // All original source NPC records decode without altering raw bytes.
        for(std::size_t i=0;i<npc_count;++i){
            const auto record=file.npc(i);
            const auto x=record.x_position_16_16()>>16U;
            const auto y=record.y_position_16_16()>>16U;
            require(x<64U&&y<64U&&record.cached_cell_index()==y*64U+x,
                    "source NPC cell cache differs from its 16.16 position");
        }
    }
    file.apply_npc_patches({});
    require(file.encode()==original,"empty patch unexpectedly changed data");
    auto corrupt=original;
    corrupt[1U]=0U;
    rejects([&]{(void)rloft::Lev012File(corrupt);});
}
} // namespace

int main(int argc,char** argv){
    exercise(synthetic(),false);
    if(argc>2)throw std::invalid_argument("usage: LEV012_tests [LEV012.DAT]");
    if(argc==2){
        std::ifstream source(argv[1],std::ios::binary);
        if(!source)throw std::runtime_error("cannot open LEV012.DAT");
        const std::vector<std::uint8_t> data(
            (std::istreambuf_iterator<char>(source)),
            std::istreambuf_iterator<char>());
        exercise(data,true);
    }
}
