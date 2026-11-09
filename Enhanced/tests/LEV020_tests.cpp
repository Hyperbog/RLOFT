#include "rloft/LEV020.hpp"

#include <array>
#include <cstddef>
#include <cstdint>
#include <fstream>
#include <iterator>
#include <stdexcept>
#include <vector>

namespace {
void require(bool ok, const char* why) {if (!ok) throw std::runtime_error(why);}
template<class F> void rejects(F f) {
    bool failed = false;
    try {f();} catch (const std::exception&) {failed = true;}
    require(failed, "invalid LEV020 edit accepted");
}
void put16(std::vector<std::uint8_t>& b, std::size_t p, unsigned v) {
    b.at(p)=static_cast<std::uint8_t>(v);
    b.at(p+1U)=static_cast<std::uint8_t>(v>>8U);
}
void put32(std::vector<std::uint8_t>& b, std::size_t p, std::uint32_t v) {
    for(unsigned i=0U;i<4U;++i)b.at(p+i)=static_cast<std::uint8_t>(v>>(8U*i));
}
std::vector<std::uint8_t> synthetic() {
    constexpr std::size_t h=0x1DU,g=h+0x2A4U;
    constexpr unsigned w=4U,ht=3U;
    constexpr std::size_t b1=g+2U*w*ht,b2=b1+(2U*w+10U)*2U+1U;
    constexpr std::size_t b3=b2+2U*22U,b4=b3+2U+2U*22U,b5=b4,b6=b5+0x514U;
    std::vector<std::uint8_t> b(b6+4U+w*ht+4U,0U);
    b[0]=0xD3U;
    constexpr std::array<std::size_t,7> bounds{{h,b1,b2,b3,b4,b5,b6}};
    for(std::size_t i=0;i<bounds.size();++i)put32(b,1U+i*4U,bounds[i]);
    put16(b,h+1U,w);put16(b,h+3U,ht);put16(b,h+0x28AU,2U);put16(b,h+0x28EU,2U);
    std::size_t at=b1;
    put16(b,at,1U);at+=2U;b[at]=2U;b[at+4U]=1U;at+=10U;
    for(unsigned c=1U;c<w;++c){put16(b,at,0U);at+=2U;}
    b[at++]=0U;
    for(unsigned c=0U;c<w;++c) {
        put16(b,at,c==1U?1U:0U);at+=2U;
        if(c==1U){b[at]=1U;b[at+4U]=1U;b[at+5U]=1U;b[at+6U]=8U;at+=10U;}
    }
    require(at==b2,"synthetic control layout");
    for(unsigned i=0U;i<2U;++i){
        const std::size_t p=b2+i*22U;
        const unsigned x=i==0U?2U:1U,y=i==0U?1U:0U;
        put32(b,p,y<<16U);put32(b,p+4U,x<<16U);
        put32(b,p+8U,0x08000000U);
        put16(b,p+12U,y*w+x);put16(b,p+14U,i==0U?47U:67U);
    }
    put16(b,b3,2U);
    for(unsigned i=0U;i<2U;++i){
        const std::size_t p=b3+2U+i*22U;
        const unsigned x=i+1U,y=i==0U?1U:0U;
        put32(b,p,y<<16U);put32(b,p+4U,x<<16U);
        put16(b,p+12U,y*w+x);put16(b,p+14U,34U+i);
        put16(b,p+20U,0x50U);
    }
    b[b5]=1U;b[b5+1U]=1U;b[b5+2U]=1U;b[b5+3U]=1U;
    put16(b,b6,w);put16(b,b6+2U,ht);
    b[b6+4U]=0x2FU;
    put32(b,b6+4U+w*ht,0U);
    return b;
}
void exercise(const std::vector<std::uint8_t>& original, bool retail) {
    using G = rloft::Lev019ControlGroup;
    rloft::Lev020File file(original);
    require(file.encode()==original &&
            file.decode().storage==rloft::LevelStorage::DirectSave,
            "source round trip");
    const auto base=file.summary();
    if(retail) {
        require(original.size()==7548U && base.width==20 && base.height==36 &&
                base.group1_controls==12U && base.group2_controls==7U &&
                base.world_counts==std::array<std::size_t,3>{{7U,134U,0U}} &&
                base.active_animation_count==0U &&
                base.active_event_slots==2U && base.active_event_commands==7U &&
                base.annotation_bytes==0U, "retail LEV020 identity");
        require(file.npc(2U).npc_type_id()==47U &&
                file.npc(2U).runtime_flags()==0x0A000000U &&
                file.npc(3U).npc_type_id()==67U &&
                file.placed_object(0U).object_type_id()==34U &&
                file.placed_object(0U).packed_runtime_state()==0x50U,
                "retail mutable NPC and placed-object states");
        std::size_t explored=0U;
        for(auto word:file.decode().cell_words)explored+=(word&0x0400U)?1U:0U;
        require(explored==381U,"original exploration marks changed");
    }
    const std::size_t actor=retail?2U:0U;
    const auto npc=file.npc(actor);
    auto new_npc=npc;new_npc.set_runtime_flags(npc.runtime_flags()^0x02000000U);
    const auto placed=file.placed_object(0U);
    auto new_placed=placed;
    new_placed.set_packed_runtime_state(placed.packed_runtime_state()^0x0040U);
    const auto old_word=file.cell_word(0,0);
    const auto old_cache=file.automap_cell(0,0);
    const auto old_event=file.event_slot(0U);
    auto new_event=old_event;
    new_event.raw[2U]^=1U;
    const auto first=file.decode().group1.front();
    const int col=first.column();
    const auto old_control=file.controls(G::Primary,col);
    auto new_control=old_control;
    auto added=old_control.front();added.raw[7U]^=0x21U;
    new_control.push_back(added);
    rloft::Lev020Edits edits;
    edits.npcs.push_back({actor,npc,new_npc});
    edits.placed.push_back({0U,placed,new_placed});
    edits.cells.push_back({0,0,old_word,static_cast<std::uint16_t>(old_word^0x0400U)});
    edits.cache.push_back({0,0,old_cache,static_cast<std::uint8_t>(old_cache^1U)});
    edits.events.push_back({0U,old_event,new_event});
    edits.controls.push_back({G::Primary,col,old_control,new_control});
    file.apply(edits);
    require(file.encode().size()==original.size()+10U &&
            file.summary().boundaries[6]==base.boundaries[6]+10U &&
            file.npc(actor).raw==new_npc.raw &&
            file.placed_object(0U).raw==new_placed.raw &&
            file.automap_cell(0,0)==edits.cache[0].replacement &&
            file.event_slot(0U).raw==new_event.raw &&
            file.controls(G::Primary,col).size()==new_control.size() &&
            file.encode()[0]==original[0],"mixed-domain edit failed");
    const auto committed=file.encode();
    rejects([&]{file.apply(edits);});
    auto invalid=edits;
    invalid.cache.push_back(invalid.cache.front());
    rejects([&]{file.apply(invalid);});
    require(file.encode()==committed,"rejected edit partially committed");
    rloft::Lev020Edits undo;
    undo.npcs.push_back({actor,new_npc,npc});
    undo.placed.push_back({0U,new_placed,placed});
    undo.cells.push_back({0,0,edits.cells[0].replacement,old_word});
    undo.cache.push_back({0,0,edits.cache[0].replacement,old_cache});
    undo.events.push_back({0U,new_event,old_event});
    undo.controls.push_back({G::Primary,col,new_control,old_control});
    file.apply(undo);
    require(file.encode()==original,"mixed-domain inverse not byte-exact");
    file.apply({});
    require(file.encode()==original,"empty patch changed source");
}
} // namespace

int main(int argc,char** argv) {
    exercise(synthetic(),false);
    if(argc>2)throw std::invalid_argument("usage: LEV020_tests [LEV020.DAT]");
    if(argc==2) {
        std::ifstream in(argv[1],std::ios::binary);
        if(!in)throw std::runtime_error("cannot open original LEV020.DAT");
        const std::vector<std::uint8_t> original{
            std::istreambuf_iterator<char>(in),std::istreambuf_iterator<char>()};
        exercise(original,true);
    }
}
