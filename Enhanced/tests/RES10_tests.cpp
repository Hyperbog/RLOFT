#include "rloft/RES10.hpp"

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <fstream>
#include <iterator>
#include <stdexcept>
#include <vector>

namespace {
void require(bool condition, const char* description) {
    if (!condition) throw std::runtime_error(description);
}
template<class Action> void rejects(Action action) {
    bool failed = false;
    try { action(); } catch (const std::exception&) { failed = true; }
    require(failed, "RES10 accepted invalid or stale original data");
}
void put16(std::vector<std::uint8_t>& b, std::size_t at, unsigned value) {
    b.at(at) = static_cast<std::uint8_t>(value);
    b.at(at+1U) = static_cast<std::uint8_t>(value >> 8U);
}
void put24(std::vector<std::uint8_t>& b, std::size_t at, unsigned value) {
    for (unsigned j=0U; j<3U; ++j)
        b.at(at+j) = static_cast<std::uint8_t>(value >> (8U*j));
}
void put32(std::vector<std::uint8_t>& b, std::size_t at, unsigned value) {
    for (unsigned j=0U; j<4U; ++j)
        b.at(at+j) = static_cast<std::uint8_t>(value >> (8U*j));
}
std::vector<std::uint8_t> eh4_pcm(
    std::uint8_t time_constant, const std::vector<std::uint8_t>& samples) {
    std::vector<std::uint8_t> voc(26U, 0U);
    constexpr char signature[] = "Creative Voice File";
    for (std::size_t i=0U; i<sizeof(signature)-1U; ++i)
        voc[i] = static_cast<std::uint8_t>(signature[i]);
    voc[19U] = 0x1AU;
    put16(voc, 20U, 26U);
    put16(voc, 22U, 0x010AU);
    put16(voc, 24U, 0x1129U);
    voc.insert(voc.end(), {1U,0U,0U,0U,time_constant,0U});
    put24(voc, 27U, static_cast<unsigned>(samples.size()+2U));
    voc.insert(voc.end(), samples.begin(), samples.end());
    voc.push_back(0U);
    std::vector<std::uint8_t> entry{'E','H',4U,0U,0U,0U,0U,0U,0U};
    put32(entry, 5U, static_cast<unsigned>(voc.size()));
    entry.insert(entry.end(), voc.begin(), voc.end());
    return entry;
}
std::vector<std::uint8_t> archive_with(
    const std::vector<std::vector<std::uint8_t>>& entries) {
    std::vector<std::uint8_t> out(4U+4U*entries.size(), 0U);
    put32(out,0U,static_cast<unsigned>(entries.size()));
    for(std::size_t i=0U;i<entries.size();++i) {
        put32(out,4U+4U*i,static_cast<unsigned>(out.size()));
        out.insert(out.end(),entries[i].begin(),entries[i].end());
    }
    return out;
}
void synthetic() {
    const auto first = eh4_pcm(0xA6U,{0U,128U,255U});
    const auto second = eh4_pcm(0xA5U,{80U,128U});
    const auto original = archive_with({first,second});
    rloft::Res10Archive file(original);
    const auto stats=file.stats();
    require(file.entry_count()==2 && stats.entries==2U &&
            stats.pcm_samples==5U && stats.voc_bytes==71U &&
            stats.duration_microseconds==452U &&
            stats.shortest_sample_count==2U &&
            stats.longest_sample_count==3U && file.encode()==original,
            "RES10 synthetic structure and byte identity changed");
    const auto a=file.voice_info(0),b=file.voice_info(1);
    require(a.voc_version==0x10AU && a.time_constant==0xA6U &&
            a.nominal_sample_rate_hz==11111U && a.duration_microseconds==270U &&
            b.time_constant==0xA5U && b.duration_microseconds==182U &&
            file.pcm8(0)==std::vector<std::uint8_t>({0U,128U,255U}),
            "RES10 synthetic decoded sample timing or data changed");
    const auto audio=file.pcm16(0);
    require(audio.sample_rate==11111U && audio.channels==1U &&
            audio.samples==std::vector<std::int16_t>({-32768,0,32512}),
            "RES10 reused VOC PCM16 converter differs");
    const auto edited=std::vector<std::uint8_t>{0U,255U,128U,64U};
    file.apply({{1,second,edited}});
    require(file.pcm8(1)==edited && file.entry(0)==first &&
            file.entry(1)!=second && file.encode().size()==original.size()+2U &&
            file.stats().pcm_samples==7U &&
            file.voice_info(1).time_constant==b.time_constant &&
            file.voice_info(1).voc_version==b.voc_version,
            "RES10 edited VOC round-trip broke sample data or headers");
    const auto committed=file.encode();
    rejects([&]{file.apply({{1,second,edited}});});
    rejects([&]{file.apply({{2,{}, {5U}}});});
    rejects([&]{file.apply({{-1,{}, {5U}}});});
    rejects([&]{file.apply({{0,first,{9U}},{0,first,{8U}}});});
    rejects([&]{file.apply({{0,first,{5U}},{1,second,{6U}}});});
    rejects([&]{file.apply({{0,first,{}}});});
    require(file.encode()==committed,"RES10 failed edit changed live archive");
    const auto new_second=file.entry(1);
    file.apply({{1,new_second,{80U,128U}}});
    require(file.encode()==original,"RES10 inverse failed byte-exact restoration");
    file.apply({});
    require(file.encode()==original,"empty RES10 edit changed source");
    rejects([&]{(void)file.voice_info(2);});
    rejects([&]{(void)file.pcm8(-1);});
    auto broken=first;
    broken[5U]^=1U; // EH declared VOC length mismatch.
    rejects([&]{(void)rloft::Res10Archive(archive_with({broken}));});
    broken=first;
    broken[9U]^=1U; // Corrupt Creative Voice File signature.
    rejects([&]{(void)rloft::Res10Archive(archive_with({broken}));});
    broken=first;
    broken[9U+24U]^=1U; // VOC version checksum.
    rejects([&]{(void)rloft::Res10Archive(archive_with({broken}));});
    broken=first;
    broken[9U+31U]=1U; // Unsupported ADPCM codec.
    rejects([&]{(void)rloft::Res10Archive(archive_with({broken}));});
    broken=first;
    broken[9U+27U]=0xFFU; // VOC block extends past entry.
    rejects([&]{(void)rloft::Res10Archive(archive_with({broken}));});
    broken=first;
    broken.back()=0x20U; // Terminator missing.
    rejects([&]{(void)rloft::Res10Archive(archive_with({broken}));});
}
void original(const std::vector<std::uint8_t>& source) {
    require(source.size()==46060354U,"uploaded RES10 size differs");
    rloft::Res10Archive file(source);
    const auto s=file.stats();
    require(file.entry_count()==219 && file.encode()==source &&
            s.entries==219U && s.voc_bytes==46057503ULL &&
            s.pcm_samples==46050276ULL &&
            s.duration_microseconds==4144524840ULL &&
            s.shortest_sample_count==36616U && s.longest_sample_count==400490U,
            "RES10 original declared sizes, durations or source identity differ");
    for(int i=0; i<219; ++i) {
        const auto info=file.voice_info(i);
        require(info.voc_version==0x10AU && info.time_constant==0xA6U &&
                info.codec==0U && info.nominal_sample_rate_hz==11111U &&
                info.voc_bytes==info.pcm_samples+33U &&
                file.pcm8(i).size()==info.pcm_samples &&
                file.voc(i).size()==info.voc_bytes,
                "RES10 original clip signature, format or decoder differs");
    }
    require(file.voice_info(0).pcm_samples==45138U &&
            file.voice_info(218).pcm_samples==308565U,
            "RES10 first/last samples changed");
    const auto audio=file.pcm16(0);
    require(audio.sample_rate==11111U && audio.channels==1U &&
            audio.samples.size()==45138U && audio.samples[0]==-256 &&
            audio.samples[1]==0,
            "RES10 native PCM16 decode differs");
    const auto before0=file.entry(0),before218=file.entry(218);
    auto samples0=file.pcm8(0),samples218=file.pcm8(218);
    auto modified0=samples0,modified218=samples218;
    modified0[0U]^=0x5AU;
    modified0.insert(modified0.end(),{0U,0x80U,0xFFU});
    modified218[0U]^=0x33U;
    modified218.pop_back();
    file.apply({{0,before0,modified0},{218,before218,modified218}});
    const auto updated=file.encode();
    require(updated!=source && updated.size()==source.size()+2U &&
            file.stats().pcm_samples==s.pcm_samples+2U &&
            file.pcm8(0)==modified0 && file.pcm8(218)==modified218,
            "RES10 original-source batch edit did not reserialize");
    rloft::Res10Archive reloaded(updated);
    rloft::Res10Archive pristine(source);
    require(reloaded.pcm8(0)==modified0 && reloaded.pcm8(218)==modified218,
            "RES10 modified original archive failed reload");
    for(int i=1;i<218;++i)
        require(file.entry(i)==reloaded.entry(i) && file.entry(i)==
            pristine.entry(i),
            "RES10 modified bytes from an untouched original voice");
    const auto checkpoint=file.encode();
    rejects([&]{file.apply({{0,before0,modified0}});});
    require(file.encode()==checkpoint,"stale RES10 batch was not atomic");
    file.apply({{0,file.entry(0),samples0},{218,file.entry(218),samples218}});
    require(file.encode()==source,"RES10 original restoration is not byte-exact");
}
} // namespace

int main(int argc,char** argv) {
    synthetic();
    if(argc>2)throw std::invalid_argument("usage: RES10_tests [RES10]");
    if(argc==2) {
        std::ifstream f(argv[1],std::ios::binary);
        if(!f)throw std::runtime_error("cannot open supplied RES10 file");
        const std::vector<std::uint8_t> bytes{
            std::istreambuf_iterator<char>(f), std::istreambuf_iterator<char>()};
        original(bytes);
    }
}
