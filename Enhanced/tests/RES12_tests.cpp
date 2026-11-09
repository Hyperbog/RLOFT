#include "rloft/RES12.hpp"
#include "rloft/resource_assets.hpp"

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <fstream>
#include <iterator>
#include <stdexcept>
#include <vector>

namespace {
void need(bool yes,const char* why) { if(!yes) throw std::runtime_error(why); }
template<class Fn> void rejects(Fn fn) {
    bool caught=false;try{fn();}catch(const std::exception&){caught=true;}
    need(caught,"RES12 invalid operation accepted");
}
void put16(std::vector<std::uint8_t>& b,std::size_t at,unsigned x) {
    b.at(at)=static_cast<std::uint8_t>(x);b.at(at+1U)=static_cast<std::uint8_t>(x>>8U);
}
void put32(std::vector<std::uint8_t>& b,std::size_t at,unsigned x) {
    for(unsigned i=0;i<4U;++i)b.at(at+i)=static_cast<std::uint8_t>(x>>(8U*i));
}
std::vector<std::uint8_t> test_voc() {
    std::vector<std::uint8_t> v(26U,0U);
    constexpr char sig[]="Creative Voice File";
    for(std::size_t i=0;i<19;++i)v[i]=static_cast<std::uint8_t>(sig[i]);
    v[19U]=0x1AU;put16(v,20U,26U);put16(v,22U,0x010AU);put16(v,24U,0x1129U);
    v.insert(v.end(),{1U,5U,0U,0U,0xA6U,0U,0x80U,0x81U,0x7FU,0U});
    return v;
}
std::vector<std::uint8_t> test_archive() {
    std::vector<std::uint8_t> raster{'E','H',1U,8U,0U,1U,0U,2U,0U,
                                     1U,0xFEU,0U,0U};
    for(unsigned i=0;i<16U;++i)raster.push_back(static_cast<std::uint8_t>(i));
    // opcode 19(text-base), opcode 05(141 inline template words),
    // opcode 18 (jump to terminate at word 146), opcode 1B (terminate).
    std::vector<std::uint16_t> words{0x19U,239U,5U};
    for(unsigned i=0;i<141U;++i)words.push_back(static_cast<std::uint16_t>(i));
    words.insert(words.end(),{0x18U,146U,0x1BU});
    need(words.size()==147U,"test wordcode layout");
    std::vector<std::uint8_t> program(2U+4U*words.size(),0U);
    put16(program,0U,static_cast<unsigned>(words.size()));
    for(std::size_t i=0;i<words.size();++i)put16(program,2U+2U*i,words[i]);
    auto voc=test_voc();
    std::vector<std::uint8_t> voice{'E','H',4U,0U,0U,0U,0U,0U,0U};
    put32(voice,5U,static_cast<unsigned>(voc.size()));
    voice.insert(voice.end(),voc.begin(),voc.end());
    const std::vector<std::vector<std::uint8_t>> entries{raster,program,voice};
    std::vector<std::uint8_t> b(4U+4U*entries.size(),0U);
    put32(b,0U,static_cast<unsigned>(entries.size()));
    for(std::size_t i=0;i<entries.size();++i) {
        put32(b,4U+4U*i,static_cast<unsigned>(b.size()));
        b.insert(b.end(),entries[i].begin(),entries[i].end());
    }
    return b;
}
void synthetic() {
    const auto original=test_archive();
    rloft::Res12Archive archive(original);
    const auto s=archive.stats();
    need(archive.encode()==original && s.entries==3U && s.rasters==1U &&
         s.programs==1U && s.program_words==147U && s.instructions==4U &&
         s.inline_templates==1U && s.nonzero_companion_words==0U &&
         s.voices==1U && s.valid_voices==1U && !s.truncated_voices,
         "synthetic RES12 inventory or grammar incorrect");
    need(archive.range(rloft::Res12Family::Raster).first==0 &&
         archive.range(rloft::Res12Family::Wordcode).first==1 &&
         archive.range(rloft::Res12Family::Voc).first==2 &&
         archive.instructions(0)[1U].operands.size()==141U &&
         archive.instructions(0)[2U].word_ip==144U &&
         archive.pcm16(2).samples.size()==3U,
         "RES12 legacy inline template or typed resources invalid");
    const auto e0=archive.entry(0);
    const auto old_program=archive.program(0);
    const auto e2=archive.entry(2);
    auto pixels=archive.raster(0).pixels;pixels[0U]^=0x31U;
    auto words=old_program.words;words[1U]^=1U;
    auto voc=archive.voc(2);voc[32U]^=1U;
    archive.apply({{{0,e0,pixels,std::nullopt}},
                   {{0,old_program.words,words}},{{2,e2,voc}}});
    need(archive.raster(0).pixels==pixels && archive.program(0).words==words &&
         archive.voc(2)==voc && archive.encode()!=original &&
         archive.program(0).companion_words==old_program.companion_words,
         "combined RES12 image/program/voice change failed");
    const auto committed=archive.encode();
    rejects([&]{archive.apply({{{0,e0,pixels,std::nullopt}}, {}, {}});});
    rejects([&]{archive.apply({{{0,archive.entry(0),pixels,std::nullopt},
                               {0,archive.entry(0),pixels,std::nullopt}}, {}, {}});});
    auto wrong=words;wrong[145U]=145U; // Points into jump's operand.
    rejects([&]{archive.apply({{},{{0,words,wrong}}, {}});});
    wrong=words;wrong[2U]=0x2AU; // Invalid opcode.
    rejects([&]{archive.apply({{},{{0,words,wrong}}, {}});});
    wrong=words;wrong.pop_back(); // Would misalign the companion bank.
    rejects([&]{archive.apply({{},{{0,words,wrong}}, {}});});
    rejects([&]{archive.apply({{}, {}, {{2,archive.entry(2),{}}}});});
    need(archive.encode()==committed,"failed RES12 operation partially committed");
    archive.apply({{{0,archive.entry(0),{},e0}},
                   {{0,words,old_program.words}},{{2,archive.entry(2),test_voc()}}});
    need(archive.encode()==original,"RES12 inverse not byte-exact");
    archive.apply({});need(archive.encode()==original,"RES12 empty edit changed source");
    rejects([&]{(void)archive.entry(-1);});
    rejects([&]{(void)archive.program(1);});
    rejects([&]{(void)archive.raster(1);});
    rejects([&]{(void)archive.voc(1);});
    rejects([&]{(void)rloft::disassemble_res9_wordcode({5U,1U,0x1BU},141U);});
    rejects([&]{(void)rloft::disassemble_res9_wordcode({0x2AU},141U);});
}
void retail(const std::vector<std::uint8_t>& original) {
    need(original.size()==12150551U,"RES12 original size mismatch");
    rloft::Res12Archive archive(original);
    const auto s=archive.stats();
    need(archive.encode()==original && archive.entry_count()==667 &&
         s.rasters==370U && s.raw_rasters==19U && s.lzss_rasters==351U &&
         s.programs==37U && s.program_words==4820U && s.instructions==1079U &&
         s.inline_templates==16U && s.nonzero_companion_words==0U &&
         s.voices==260U && s.valid_voices==259U && s.truncated_voices==1U &&
         archive.range(rloft::Res12Family::Wordcode).first==370 &&
         archive.range(rloft::Res12Family::Voc).first==407,
         "RES12 original archive/wordcode inventory differs");
    std::size_t decoded_pixels=0U;
    for(int i=0;i<370;++i) {
        auto info=archive.raster_info(i);auto image=archive.raster(i);
        need(image.pixels.size()==info.decoded_bytes,"RES12 raster decode wrong size");
        decoded_pixels+=image.pixels.size();
    }
    need(decoded_pixels==s.decoded_pixels,"RES12 decoded pixel checksum total mismatch");
    std::size_t instr=0U,templates=0U;
    for(int i=0;i<37;++i) {
        const auto p=archive.program(i);const auto ins=archive.instructions(i);
        instr+=ins.size();
        templates+=std::count_if(ins.begin(),ins.end(),
            [](const rloft::Res9Instruction& x){return x.opcode==5U;});
        need(std::all_of(p.companion_words.begin(),p.companion_words.end(),
            [](std::uint16_t x){return x==0U;}),"RES12 companion bank changed");
    }
    need(instr==1079U && templates==16U,"RES12 legacy VM targets/instructions differ");
    for(int i=407;i<667;++i) {
        if(i==430) {
            need(archive.is_truncated_voc(i) && archive.entry(i).size()==5746U &&
                 archive.voc(i).size()==5737U,"original RES12:430 anomaly modified");
            rejects([&]{(void)archive.pcm16(i);});
            rejects([&]{archive.apply({{}, {}, {{i,archive.entry(i),test_voc()}}});});
        } else {
            need(!archive.is_truncated_voc(i) && !archive.voc(i).empty(),
                 "RES12 VOC payload invalid");
        }
    }
    const auto raster0=archive.entry(0);
    const auto program0=archive.program(0);
    const auto voc407=archive.entry(407);
    auto pixels=archive.raster(0).pixels;pixels[0U]^=1U;
    auto words=program0.words;words[1U]^=1U;
    auto voice=archive.voc(407);voice[32U]^=1U;
    archive.apply({{{0,raster0,pixels,std::nullopt}},
                   {{0,program0.words,words}},{{407,voc407,voice}}});
    need(archive.raster(0).pixels==pixels && archive.program(0).words==words &&
         archive.voc(407)==voice && archive.entry(430).size()==5746U &&
         archive.entry(430)==rloft::ResArchive(original).entry(430) &&
         archive.program(0).companion_words==program0.companion_words &&
         archive.entry(369)==rloft::ResArchive(original).entry(369) &&
         archive.entry(666)==rloft::ResArchive(original).entry(666),
         "RES12 original combined edit changed untouched assets");
    const auto committed=archive.encode();
    rejects([&]{archive.apply({{},{{0,program0.words,words}}, {}});});
    rejects([&]{archive.apply({{}, {}, {{430,archive.entry(430),test_voc()}}});});
    need(archive.encode()==committed,"RES12 invalid edit changed live archive");
    archive.apply({{{0,archive.entry(0),{},raster0}},
                   {{0,words,program0.words}},{{407,archive.entry(407),
                   std::vector<std::uint8_t>(voc407.begin()+9,voc407.end())}}});
    need(archive.encode()==original,"RES12 original compressed bytes not restored");
}
} // namespace
int main(int argc,char** argv) {
    synthetic();
    if(argc>2)throw std::invalid_argument("usage: RES12_tests [RES12]");
    if(argc==2) {
        std::ifstream in(argv[1],std::ios::binary);
        if(!in)throw std::runtime_error("cannot open RES12");
        const std::vector<std::uint8_t> data{std::istreambuf_iterator<char>(in),
                                             std::istreambuf_iterator<char>()};
        retail(data);
    }
}
