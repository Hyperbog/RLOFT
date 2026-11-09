#include "rloft/LEV011.hpp"

#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <fstream>
#include <iterator>
#include <stdexcept>
#include <vector>

namespace {
void need(bool condition, const char* why) {
    if (!condition) throw std::runtime_error(why);
}
template <class F> void rejects(F&& action) {
    bool rejected = false;
    try { action(); } catch (const std::exception&) { rejected = true; }
    need(rejected, "invalid LEV011 edit was accepted");
}
void write16(std::vector<std::uint8_t>& b, std::size_t at, unsigned value) {
    b.at(at) = static_cast<std::uint8_t>(value);
    b.at(at + 1U) = static_cast<std::uint8_t>(value >> 8U);
}
void write32(std::vector<std::uint8_t>& b, std::size_t at, unsigned value) {
    for (unsigned i = 0; i < 4U; ++i)
        b.at(at+i) = static_cast<std::uint8_t>(value >> (8U*i));
}
std::vector<std::uint8_t> synthetic() {
    constexpr std::size_t start=0x1DU, grid=start+0x2A4U;
    constexpr unsigned width=3U, height=2U;
    constexpr std::size_t b1=grid+width*height*2U;
    constexpr std::size_t b2=b1+width*2U+1U+width*2U;
    constexpr std::size_t b3=b2, b4=b3+2U, b5=b4, b6=b5+0x514U;
    // Includes one negative-coordinate CR label and one literal marker.
    const std::vector<std::uint8_t> notes{
        0xFDU,0xFFU,0x00U,0x01U,0x09U,0x00U,
        'M','A','D','\r','W','O','O','D','S',
        0x22U,0x00U,0x33U,0x00U,0x02U,0x00U,'/','/'};
    std::vector<std::uint8_t> b(b6+4U+width*height+4U+notes.size(),0U);
    b[0]=0xDFU; // CODE.2 direct writer leaves this byte unowned.
    constexpr std::array<std::size_t,7> bounds{{start,b1,b2,b3,b4,b5,b6}};
    for(std::size_t i=0;i<bounds.size();++i)
        write32(b,1U+i*4U,static_cast<unsigned>(bounds[i]));
    write16(b,start+1U,width);write16(b,start+3U,height);
    b[start+0x29CU]=3U;
    write16(b,b3,0U); // placed object count.
    write16(b,b6,width);write16(b,b6+2U,height);
    for(unsigned i=0;i<width*height;++i)b[b6+4U+i]=static_cast<std::uint8_t>(i);
    write32(b,b6+4U+width*height,static_cast<unsigned>(notes.size()));
    std::copy(notes.begin(),notes.end(),b.end()-static_cast<std::ptrdiff_t>(notes.size()));
    return b;
}
void exercise(const std::vector<std::uint8_t>& source, bool original) {
    rloft::Lev011File file(source);
    need(file.encode()==source && file.decode().storage==rloft::LevelStorage::DirectSave,
         "LEV011 did not preserve its original bytes");
    const auto base=file.summary();
    const auto all=file.notes();
    if(original){
        need(source.size()==58696U && base.width==128 && base.height==128 &&
             base.group1_controls==184U && base.group2_controls==0U &&
             base.world_counts==std::array<std::size_t,3>{{90U,116U,1U}} &&
             base.active_event_slots==21U && base.active_event_commands==23U &&
             base.annotation_bytes==622U && base.music_sequence==3U &&
             all.size()==58U && all.front().x==1033 && all.front().y==243 &&
             all.front().label==std::vector<std::uint8_t>{'O','L','D',' ','S','V','A','L','I','C','H',' ','R','O','A','D'} &&
             all[1].x==-3 && all[7].label[5]=='\r' &&
             all.back().label==std::vector<std::uint8_t>{'B','L','O','O','D',' ','O','N',' ','T','H','E',' ','V','I','N','E'},
             "original LEV011 label and world inventory differs");
    } else {
        need(all.size()==2U && all[0].x==-3 && all[0].label[3]=='\r' &&
             all[1].label==std::vector<std::uint8_t>{'/','/'},
             "synthetic annotations not parsed");
    }
    // Update a signed-position label without rewriting other data.
    auto changed=all[0];changed.x=-17;changed.y=1033;
    changed.label={'E','D','I','T','\r','1'};
    file.set_note(0U,changed);
    const rloft::Lev011File decoded(file.encode());
    need(decoded.notes()[0].x==-17 && decoded.notes()[0].y==1033 &&
         decoded.notes()[0].label==changed.label,
         "LEV011 typed annotation edit did not reload");
    const auto after=file.encode();
    const auto text_begin=static_cast<std::size_t>(base.boundaries[6])+4U+
                          static_cast<std::size_t>(base.width*base.height)+4U;
    for(std::size_t i=0;i<text_begin-4U;++i)
        need(after[i]==source[i],"note edit modified map/world/event/cache section");
    need(file.summary().annotation_bytes==source.size()-text_begin-
         all[0].label.size()+changed.label.size(),"note edit length mismatch");
    // Add/erase, verifying original bytes of all other records survive.
    const rloft::Lev011MapNote injected{32767,-32768,{'/','/','\r',0xFEU}};
    file.insert_note(all.size(),injected);
    need(file.notes().size()==all.size()+1U &&
         file.notes().back().label==injected.label,
         "insertion did not preserve raw multibyte label");
    file.erase_note(all.size());
    need(file.notes().size()==all.size(),"erasure did not restore note count");
    file.replace_notes(all);
    need(file.encode()==source,"LEV011 note edits not byte-exactly reversible");
    file.replace_notes({});
    need(file.notes().empty() && file.summary().annotation_bytes==0U,
         "clear notes did not produce canonical empty tail");
    file.replace_notes(all);
    need(file.encode()==source,"LEV011 original annotations not restorable");
    // Every failure must leave the original file intact.
    const auto before=file.encode();
    rejects([&]{file.set_note(all.size(),injected);});
    rejects([&]{file.insert_note(all.size()+1U,injected);});
    rejects([&]{file.erase_note(all.size());});
    auto bad=all;bad[0].label.clear();
    rejects([&]{file.replace_notes(bad);});
    bad=all;bad[0].label.assign(2047U,0x41U);
    rejects([&]{file.replace_notes(bad);});
    need(file.encode()==before,"invalid note edit was partially applied");
    const auto cell=file.cell_word(0,0);
    file.apply_cell_patches({{0,0,cell,static_cast<std::uint16_t>(cell^0x0440U)}});
    need(file.cell_word(0,0)==(cell^0x0440U),"shared grid editor failed");
    file.apply_cell_patches({{0,0,static_cast<std::uint16_t>(cell^0x0440U),cell}});
    const auto slot=file.event_slot(0U);
    auto edited=slot;edited.raw[0]=1U;edited.raw[1]=4U;
    file.replace_event_slot(0U,edited);
    need(file.event_slot(0U).raw==edited.raw,"shared event editor failed");
    file.replace_event_slot(0U,slot);
    auto cache=file.decode().automap_cache;
    const auto cache_copy=cache;cache.back()^=0x55U;
    file.replace_automap_cache(cache);
    need(file.automap_cell(base.width-1,base.height-1)==cache.back(),
         "shared renderer cache editor failed");
    file.replace_automap_cache(cache_copy);
    const auto loose=file.decode().family_c_raw;
    auto extended=loose;extended.insert(extended.end(),22U,0U);
    file.replace_world_family(rloft::LevWorldFamily::Loose,extended);
    need(file.summary().world_counts[2]==base.world_counts[2]+1U &&
         file.notes().size()==all.size(),"family resizing corrupted note offset");
    file.replace_world_family(rloft::LevWorldFamily::Loose,loose);
    file.set_music_sequence(37U);
    file.set_music_sequence(base.music_sequence);
    need(file.encode()==source,"combined edits did not restore exact LEV011");
    auto malformed=source;malformed.pop_back();
    rejects([&]{(void)rloft::Lev011File(malformed);});
    malformed=source;
    malformed[text_begin-4U]=0xFFU; // annotation length dword invalid.
    rejects([&]{(void)rloft::Lev011File(malformed);});
    malformed=source;
    malformed[text_begin+4U]=0U;malformed[text_begin+5U]=0U; // 0-length note.
    rejects([&]{(void)rloft::Lev011File(malformed);});
}
} // namespace

int main(int argc, char** argv) {
    exercise(synthetic(),false); // Runs without original commercial media.
    if(argc>2)throw std::invalid_argument("usage: LEV011_tests [LEV011.DAT]");
    if(argc==2){
        std::ifstream f(argv[1],std::ios::binary);
        if(!f)throw std::runtime_error("cannot open original LEV011.DAT");
        const std::vector<std::uint8_t> bytes{
            std::istreambuf_iterator<char>(f),std::istreambuf_iterator<char>()};
        exercise(bytes,true);
    }
}
