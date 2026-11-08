#include "rloft/LEVELS.hpp"

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <fstream>
#include <iterator>
#include <stdexcept>
#include <string>
#include <vector>

namespace {
void require(bool ok, const char* reason) {
    if (!ok) throw std::runtime_error(reason);
}
int reject_index = 0;
template <class F> void rejects(F action) {
    ++reject_index;
    bool failed = false;
    try { action(); } catch (const std::exception&) { failed = true; }
    if (!failed) throw std::runtime_error("invalid LEVELS.DAT accepted at rejection #" + std::to_string(reject_index));
}
void put16(std::vector<std::uint8_t>& b, std::size_t p, unsigned value) {
    b.at(p) = static_cast<std::uint8_t>(value);
    b.at(p + 1U) = static_cast<std::uint8_t>(value >> 8U);
}
void put32(std::vector<std::uint8_t>& b, std::size_t p, unsigned value) {
    for (unsigned shift = 0U; shift < 32U; shift += 8U)
        b.at(p + shift / 8U) = static_cast<std::uint8_t>(value >> shift);
}
std::vector<std::uint8_t> synthetic_level() {
    constexpr unsigned width = 2, height = 2;
    constexpr unsigned b0 = 0x1DU;
    constexpr unsigned b1 = b0 + 0x2A4U + 2U * width * height;
    constexpr unsigned b2 = b1 + 2U * width + 10U;
    constexpr unsigned b3 = b2;
    constexpr unsigned b4 = b3 + 11U;
    constexpr unsigned b5 = b4;
    constexpr unsigned b6 = b5 + 0x514U;
    std::vector<std::uint8_t> r(b6 + 4U + width * height + 4U, 0);
    r[0] = 0xDFU; // Archive decoder does not own this byte.
    for (unsigned i = 0U, bounds[] = {b0,b1,b2,b3,b4,b5,b6}; i < 7U; ++i)
        put32(r, 1U + 4U * i, bounds[i]);
    put16(r,b0+1U,width); put16(r,b0+3U,height);
    r[b0+0x290U]=0xFFU; r[b0+0x29CU]=12U;
    put16(r,b0+0x28EU,1U);
    put16(r,b1,1U); // 1 control in column 0
    r[b1+2U+4U]=1U; r[b1+2U+5U]=0U;
    put16(r,b1+12U,0U); // no control in column 1
    r[b5] = 1U; r[b5 + 1U] = 7U; r[b5 + 2U] = 9U;
    put16(r,b6,width); put16(r,b6+2U,height);
    put32(r,b6+4U+width*height,0U);
    return r;
}
void synthetic() {
    const auto r = synthetic_level();
    std::vector<std::uint8_t> archive(8U+r.size(),0U);
    put32(archive,0U,1U); put32(archive,4U,8U);
    std::copy(r.begin(),r.end(),archive.begin()+8);
    rloft::LevelsArchive levels(archive);
    require(levels.level_count()==1U && levels.encode()==archive,
            "synthetic archive roundtrip");
    auto metadata=levels.summary(0U);
    require(metadata.width==2 && metadata.height==2 &&
            metadata.control_records==1U && metadata.family_b_objects==1U &&
            metadata.active_event_commands==1U && metadata.music_sequence==12U,
            "synthetic level counts");
    levels.set_cell_word(0U,1,1,0x2400U);
    levels.set_music_sequence(0U,7U);
    require(levels.record(0U)[0]==0xDFU && levels.summary(0U).music_sequence==7U,
            "raw preamble or music changed incorrectly");
    auto reloaded=rloft::LevelsArchive(levels.encode());
    require(reloaded.record(0U)[0x1DU+0x2A4U+6U]==0U &&
            reloaded.record(0U)[0x1DU+0x2A4U+7U]==0x24U,
            "cell word was not edited at original grid offset");
    rejects([&] { levels.set_cell_word(0U,2,0,1U); });
    rejects([&] { levels.set_cell_word(0U,0,-1,1U); });
    rejects([&] { (void)levels.record(1U); });
    rejects([&] { levels.replace_record(1U,r); });
    auto bad=archive; put32(bad,4U,9U);
    rejects([&] { (void)rloft::LevelsArchive(bad); });
    bad=archive; put16(bad,8U+0x1DU+1U,0U);
    rejects([&] { (void)rloft::LevelsArchive(bad); });
    bad=archive; bad[8U+0x1DU+0x2A4U+8U+2U+5U]=1U;
    rejects([&] { (void)rloft::LevelsArchive(bad); });
    bad=archive; const auto event_at=8U+0x1DU+0x2A4U+8U+14U+11U;
    bad[event_at]=6U;
    rejects([&] { (void)rloft::LevelsArchive(bad); });
    bad=archive; put32(bad,bad.size()-4U,4U);
    rejects([&] { (void)rloft::LevelsArchive(bad); });
    rejects([] { (void)rloft::LevelsArchive({}); });
}
void original(const std::vector<std::uint8_t>& b) {
    require(b.size()==499878U,"original LEVELS.DAT file size");
    rloft::LevelsArchive levels(b);
    require(levels.level_count()==32U && levels.encode()==b,
            "original LEVELS.DAT directory/roundtrip mismatch");
    std::size_t cells=0,controls=0,world_a=0,world_b=0,world_c=0,
                event_slots=0,commands=0;
    for(std::size_t i=0;i<levels.level_count();++i) {
        const auto& s=levels.summary(i);
        cells+=s.cell_count; controls+=s.control_records;
        world_a+=s.family_a_npcs;world_b+=s.family_b_objects;
        world_c+=s.family_c_loose_items;
        event_slots+=s.active_event_slots;
        commands+=s.active_event_commands;
    }
    require(cells==115584U && controls==1659U &&
            world_a==1258U && world_b==4324U && world_c==622U &&
            event_slots==394U && commands==555U,
            "original archive corpus statistics mismatch");
    const auto original22=levels.record(22U);
    require(original22.size()==0x1288U &&
            levels.summary(22U).width==30 && levels.summary(22U).height==15 &&
            levels.summary(22U).family_a_npcs==1U &&
            levels.summary(22U).family_b_objects==110U &&
            levels.summary(22U).family_c_loose_items==0U,
            "level-22 compact layout differs");
    levels.set_music_sequence(22U,0x2FU);
    levels.set_cell_word(22U,3,2,0x2400U);
    const auto changed=levels.encode();
    require(changed.size()==b.size() && changed!=b,
            "fixed-size mod did not persist");
    auto reopened=rloft::LevelsArchive(changed);
    require(reopened.summary(22U).music_sequence==0x2FU,
            "level-22 music modification lost");
    for (std::size_t i=0;i<levels.level_count();++i)
        if(i!=22U) require(reopened.record(i)==levels.record(i),
                         "unmodified level changed");
    auto longer=original22;
    put32(longer,longer.size()-4U,5U);
    longer.insert(longer.end(),{0x11U,0x22U,0x33U,0x44U,0x55U});
    levels.replace_record(22U,longer);
    const auto enlarged=levels.encode();
    reopened=rloft::LevelsArchive(enlarged);
    require(enlarged.size()==b.size()+5U &&
            reopened.summary(22U).annotation_bytes==5U &&
            reopened.record(22U)==longer,
            "variable-size level record not re-indexed");
    for(std::size_t i=0;i<32U;++i)
        if(i!=22U) require(reopened.record(i)==levels.record(i),
                         "re-indexing damaged a different level");
    auto invalid=longer; invalid.pop_back();
    rejects([&] { levels.replace_record(22U,invalid); });
    require(levels.encode()==enlarged,"failed edit changed live archive");
}
} // namespace

int main(int argc, char** argv) {
    synthetic();
    if(argc>2) throw std::runtime_error("usage: LEVELS_tests [LEVELS.DAT]");
    if(argc==2) {
        std::ifstream file(argv[1],std::ios::binary);
        if(!file) throw std::runtime_error("could not open original LEVELS.DAT");
        const std::vector<std::uint8_t> data(
            (std::istreambuf_iterator<char>(file)),
            std::istreambuf_iterator<char>());
        original(data);
    }
}
