#include "rloft/LEV018.hpp"

#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <fstream>
#include <iterator>
#include <stdexcept>
#include <vector>

namespace {
void need(bool result, const char* problem) {
    if (!result) throw std::runtime_error(problem);
}
template <typename Callable> void rejects(Callable&& test) {
    bool failed=false;
    try {test();} catch (const std::exception&) {failed=true;}
    need(failed,"LEV018 invalid edit was accepted");
}
void put16(std::vector<std::uint8_t>& b,std::size_t at,std::uint16_t value) {
    b.at(at)=static_cast<std::uint8_t>(value);
    b.at(at+1U)=static_cast<std::uint8_t>(value>>8U);
}
void put32(std::vector<std::uint8_t>& b,std::size_t at,std::uint32_t value) {
    for(unsigned i=0U;i<4U;++i)b.at(at+i)=static_cast<std::uint8_t>(value>>(8U*i));
}
std::vector<std::uint8_t> synthetic() {
    constexpr std::size_t first=0x1DU,grid=first+0x2A4U;
    constexpr unsigned width=3U,height=2U;
    constexpr std::size_t b1=grid+width*height*2U;
    constexpr std::size_t b2=b1+width*2U+1U+width*2U;
    constexpr std::size_t b3=b2;
    constexpr std::size_t b4=b3+2U+2U*22U;
    constexpr std::size_t b5=b4,b6=b5+0x514U;
    std::vector<std::uint8_t> data(b6+4U+width*height+4U,0U);
    data[0]=0xDFU;
    const std::array<std::size_t,7> bounds{{first,b1,b2,b3,b4,b5,b6}};
    for(std::size_t i=0;i<7U;++i)put32(data,1U+i*4U,static_cast<unsigned>(bounds[i]));
    put16(data,first+1U,width);put16(data,first+3U,height);
    put16(data,first+0x28EU,2U);data[first+0x29CU]=45U;
    put16(data,b3,2U);
    // Two independent 22-byte original direct-save placed objects.
    // 0: x=2.5,y=1.25,cell=5,type=60, packed=0x30.
    put32(data,b3+2U,0x00014000U);put32(data,b3+6U,0x00028000U);
    put16(data,b3+14U,5U);put16(data,b3+16U,60U);
    put16(data,b3+22U,0x30U);
    // 1: x=1.25,y=0.75,cell=1,type=61, packed=0x10.
    put32(data,b3+24U,0x0000C000U);put32(data,b3+28U,0x00014000U);
    put16(data,b3+36U,1U);put16(data,b3+38U,61U);
    put16(data,b3+44U,0x10U);
    put16(data,b6,width);put16(data,b6+2U,height);
    put32(data,b6+4U+width*height,0U);
    return data;
}
void exercise(const std::vector<std::uint8_t>& original,bool retail) {
    rloft::Lev018File file(original);
    need(file.encode()==original &&
         file.decode().storage==rloft::LevelStorage::DirectSave,
         "original LEV018 not lossless");
    const auto summary=file.summary();
    if(retail) {
        need(original.size()==19722U && summary.width==80 &&
             summary.height==64 && summary.group1_controls==18U &&
             summary.group2_controls==2U && summary.active_animation_count==0U &&
             summary.world_counts==std::array<std::size_t,3>{{8U,75U,0U}} &&
             summary.active_event_slots==2U &&
             summary.active_event_commands==3U &&
             summary.annotation_bytes==0U && summary.music_sequence==45U,
             "uploaded Level18 metadata mismatch");
        need(file.placed_object(0).object_type_id()==60U &&
             file.placed_object(40).object_type_id()==61U &&
             file.placed_object(74).cached_cell_index()==1432U,
             "original placed-object source identity mismatch");
    } else {
        need(summary.world_counts[1]==2U &&
             file.placed_object(0).object_type_id()==60U &&
             file.placed_object(1).object_type_id()==61U &&
             file.placed_object(0).cached_cell_index()==5U,
             "synthetic placed-object source mismatch");
    }
    const auto first=file.placed_object(0U);
    auto modified=first;
    modified.set_runtime_flags(first.runtime_flags()^0x00120000U);
    modified.set_state_word(static_cast<std::uint16_t>(first.state_word()^0x20U));
    modified.set_auxiliary_word(static_cast<std::uint16_t>(first.auxiliary_word()+7U));
    modified.set_packed_runtime_state(static_cast<std::uint16_t>(first.packed_runtime_state()^0x08U));
    const std::size_t other=retail?40U:1U;
    const auto second=file.placed_object(other);
    auto second_modified=second;
    second_modified.set_state_word(static_cast<std::uint16_t>(second.state_word()+1U));
    file.apply_placed_patches({{0U,first,modified},{other,second,second_modified}});
    need(file.placed_object(0).raw==modified.raw &&
         file.placed_object(other).raw==second_modified.raw,
         "atomic placed-object edit did not commit");
    const auto changed=file.encode();
    const rloft::Lev018File reopened(changed);
    need(reopened.placed_object(other).raw==second_modified.raw,
         "modified placed object did not reload");
    const auto offset=static_cast<std::size_t>(summary.boundaries[3])+2U;
    for(std::size_t i=0U;i<original.size();++i) {
        const auto first_changed=i>=offset&&i<offset+22U;
        const auto second_changed=i>=offset+other*22U&&i<offset+(other+1U)*22U;
        if(!first_changed&&!second_changed)
            need(original[i]==changed[i],"edit touched unrelated original bytes");
    }
    const auto committed=file.encode();
    rejects([&]{file.apply_placed_patches({{0U,first,modified}});});
    rejects([&]{file.apply_placed_patches({{0U,modified,first},{0U,modified,first}});});
    rejects([&]{file.apply_placed_patches({{summary.world_counts[1],modified,first}});});
    auto bad=modified;
    bad.set_position_16_16(0xFFFF0000U,0U,0U);
    rejects([&]{file.apply_placed_patches({{0U,modified,bad}});});
    bad=modified;
    bad.set_position_16_16(modified.x_position_16_16(),
                           modified.y_position_16_16(),0U);
    rejects([&]{file.apply_placed_patches({{0U,modified,bad}});});
    need(file.encode()==committed,"rejected edit partially committed");
    file.apply_placed_patches({{0U,modified,first},{other,second_modified,second}});
    need(file.encode()==original,"reverted placed-object edits are not byte-exact");
    // Changing 16.16 position requires changing the cached cell together.
    const auto x=first.x_position_16_16();
    const auto y=first.y_position_16_16();
    const auto moved_x=x-0x10000U;
    auto relocated=first;
    relocated.set_position_16_16(moved_x,y,static_cast<std::uint16_t>(
        (y>>16U)*static_cast<unsigned>(summary.width)+(moved_x>>16U)));
    file.apply_placed_patches({{0U,first,relocated}});
    need(file.placed_object(0).raw==relocated.raw,"valid object relocation failed");
    file.apply_placed_patches({{0U,relocated,first}});
    need(file.encode()==original,"object relocation did not restore original");
    if(retail) {
        for(std::size_t i=0;i<75U;++i) {
            const auto ob=file.placed_object(i);
            const auto ix=ob.x_position_16_16()>>16U;
            const auto iy=ob.y_position_16_16()>>16U;
            need(ix<80U&&iy<64U&&ob.cached_cell_index()==iy*80U+ix,
                 "retail placed-object position/cache mismatch");
        }
        need(file.npc(0).npc_type_id()==0U,"existing NPC codec changed");
    }
    file.set_music_sequence(18U);
    file.set_music_sequence(summary.music_sequence);
    auto event=file.event_slot(0U);
    auto edit_event=event;edit_event.raw[0]=1U;edit_event.raw[1]=3U;
    file.replace_event_slot(0U,edit_event);
    file.replace_event_slot(0U,event);
    need(file.encode()==original,"delegated music/event edits not reversible");
    file.apply_placed_patches({});
    need(file.encode()==original,"empty patch changed source");
    auto invalid=original;invalid[1]=0U;
    rejects([&]{(void)rloft::Lev018File(invalid);});
}
}
int main(int argc,char** argv) {
    exercise(synthetic(),false);
    if(argc>2)throw std::invalid_argument("usage: LEV018_tests [LEV018.DAT]");
    if(argc==2) {
        std::ifstream in(argv[1],std::ios::binary);
        if(!in)throw std::runtime_error("Cannot open original LEV018.DAT");
        const std::vector<std::uint8_t> source(
            (std::istreambuf_iterator<char>(in)),std::istreambuf_iterator<char>());
        exercise(source,true);
    }
}
