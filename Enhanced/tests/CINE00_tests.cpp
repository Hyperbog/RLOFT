#include "rloft/CINE00.hpp"

#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <iterator>
#include <stdexcept>
#include <vector>

namespace {
using rloft::Cine00File;
using rloft::CineChunkPatch;
using rloft::CineEdits;

void need(bool condition, const char* message) {
    if (!condition) throw std::runtime_error(message);
}
template<class F> void rejects(F f) {
    bool failed = false;
    try { f(); } catch (const std::exception&) { failed = true; }
    need(failed, "invalid ANM edit was not rejected");
}
void put16(std::vector<std::uint8_t>& v, std::uint16_t n) {
    v.push_back(static_cast<std::uint8_t>(n));
    v.push_back(static_cast<std::uint8_t>(n >> 8U));
}
void put32(std::vector<std::uint8_t>& v, std::uint32_t n) {
    for (unsigned i = 0U; i < 4U; ++i)
        v.push_back(static_cast<std::uint8_t>(n >> (8U * i)));
}
void chunk(std::vector<std::uint8_t>& bytes, std::uint16_t type,
           const std::vector<std::uint8_t>& payload) {
    put16(bytes, type); put32(bytes, static_cast<std::uint32_t>(payload.size()));
    bytes.insert(bytes.end(), payload.begin(), payload.end());
}
std::vector<std::uint8_t> rle_solid(std::uint8_t pixel) {
    std::vector<std::uint8_t> encoded;
    encoded.reserve(400U * 6U);
    for (int y = 0; y < 400; ++y)
        encoded.insert(encoded.end(),{0x81U,pixel,0x81U,pixel,0xC1U,pixel});
    return encoded;
}
std::vector<std::uint8_t> synthetic() {
    std::vector<std::uint8_t> bytes;
    put16(bytes,3U); put16(bytes,1U);
    put16(bytes,3U);
    chunk(bytes,1U,{});
    chunk(bytes,0U,std::vector<std::uint8_t>(768U,0x13U));
    chunk(bytes,2U,rle_solid(0x2AU));
    put16(bytes,3U);
    std::vector<std::uint8_t> pixels(320U * 256U);
    for (std::size_t i=0U;i<pixels.size();++i)
        pixels[i]=static_cast<std::uint8_t>((i * 7U) & 0xFFU);
    chunk(bytes,8U,Cine00File::encode_viewport(pixels));
    chunk(bytes,3U,{0U,0U});
    chunk(bytes,7U,{1U,0U,5U,0U});
    put16(bytes,1U);
    chunk(bytes,9U,{27U,0U});
    return bytes;
}
std::pair<std::size_t,std::size_t> find_chunk(const Cine00File& file,
                                              std::uint16_t id) {
    for (std::size_t i=0U;i<file.stats().frames;++i)
        for (std::size_t j=0U;j<file.chunk_count(i);++j)
            if (file.info(i,j).id==id) return {i,j};
    throw std::runtime_error("ANM expected fixture chunk not found");
}
void exercise(const std::vector<std::uint8_t>& original,bool retail) {
    Cine00File file(original);
    const auto stats = file.stats();
    need(file.encode()==original && stats.default_wait==1U,
         "initial ANM byte-for-byte preservation failed");
    if (retail) {
        need(original.size()==13781936U && stats.frames==405U &&
            stats.chunks==411U &&
            stats.by_id==std::array<std::size_t,10>{{57,1,1,159,0,1,2,12,167,11}},
            "CINE00 original frame/chunk inventory changed");
        need(file.chunk_count(0U)==3U &&
             file.info(0U,0U).id==1U &&
             file.info(0U,1U).id==0U &&
             file.info(0U,2U).id==2U &&
             file.info(0U,2U).payload_bytes==53844U,
             "CINE00 opening frame changed");
    } else {
        need(stats.frames==3U && stats.chunks==7U &&
             stats.by_id==std::array<std::size_t,10>{{1,1,1,1,0,0,0,1,1,1}},
             "synthetic ANM structure changed");
    }
    const auto full = find_chunk(file,2U);
    const auto palette = find_chunk(file,0U);
    const auto planar = find_chunk(file,8U);
    const auto delta = find_chunk(file,3U);
    const auto frame = file.full_frame(full.first,full.second);
    need(frame.size()==320U*400U,"ANM full-frame RLE geometry changed");
    if (!retail) need(frame.front()==0x2AU && frame.back()==0x2AU,
                      "synthetic RLE pixel decode failed");
    const auto old_palette=file.payload(palette.first,palette.second);
    const auto old_rle=file.payload(full.first,full.second);
    const auto old_packed=file.payload(planar.first,planar.second);
    const auto old_pixels=file.viewport(planar.first,planar.second);
    need(old_pixels.size()==320U*256U &&
         Cine00File::encode_viewport(old_pixels)==old_packed,
         "ANM four-plane image decode/encode not lossless");
    auto pixels=old_pixels;
    pixels[0U]^=0x5AU;
    pixels[319U]^=0x1AU;
    pixels[320U*255U+100U]^=0x36U;
    auto new_palette=old_palette;
    new_palette[100U]^=0x2AU;
    const auto new_packed=Cine00File::encode_viewport(pixels);
    const auto new_rle=rle_solid(0x36U); // Alternate valid image coding.
    CineEdits changes{};
    changes.expected_wait=1U; changes.replacement_wait=2U;
    changes.chunks={{palette.first,palette.second,old_palette,new_palette},
                    {full.first,full.second,old_rle,new_rle},
                    {planar.first,planar.second,old_packed,new_packed}};
    file.apply(changes);
    const auto changed=file.encode();
    need(changed.size()==original.size()+new_rle.size()-old_rle.size() &&
         file.stats().default_wait==2U && file.stats().frames==stats.frames &&
         file.stats().by_id==stats.by_id &&
         file.payload(palette.first,palette.second)==new_palette &&
         file.payload(full.first,full.second)==new_rle &&
         file.payload(planar.first,planar.second)==new_packed &&
         file.viewport(planar.first,planar.second)==pixels &&
         file.full_frame(full.first,full.second).front()==0x36U,
         "ANM atomic multi-chunk replacement failed");
    need(file.payload(delta.first,delta.second)==
         Cine00File(original).payload(delta.first,delta.second),
         "unrelated original delta was modified");
    rejects([&]{file.apply(changes);}); // All expected originals stale.
    CineEdits bad{};
    bad.expected_wait=1U; bad.replacement_wait=3U;
    rejects([&]{file.apply(bad);});
    bad={}; bad.chunks={{palette.first,palette.second,new_palette,{0U}}};
    rejects([&]{file.apply(bad);}); // Palette must be 768 bytes.
    bad={}; bad.chunks={{full.first,full.second,new_rle,{0x81U}}};
    rejects([&]{file.apply(bad);}); // Truncated RLE.
    bad={}; bad.chunks={{delta.first,delta.second,
                         file.payload(delta.first,delta.second),{1U,0U}}};
    rejects([&]{file.apply(bad);}); // Truncated sparse delta.
    bad={}; bad.chunks={{planar.first,planar.second,new_packed,{0U}}};
    rejects([&]{file.apply(bad);}); // Incorrect planar byte count.
    bad={}; bad.chunks={{palette.first,palette.second,new_palette,old_palette},
                        {palette.first,palette.second,new_palette,old_palette}};
    rejects([&]{file.apply(bad);}); // Duplicates.
    bad={}; bad.chunks={{palette.first,palette.second,new_palette,old_palette},
                        {full.first,full.second,old_rle,new_rle}};
    rejects([&]{file.apply(bad);}); // One valid, one stale: atomic.
    need(file.encode()==changed,"failed ANM edit partially mutated file");
    CineEdits undo{};
    undo.expected_wait=2U; undo.replacement_wait=1U;
    undo.chunks={{palette.first,palette.second,new_palette,old_palette},
                 {full.first,full.second,new_rle,old_rle},
                 {planar.first,planar.second,new_packed,old_packed}};
    file.apply(undo);
    need(file.encode()==original,"ANM inverse edit was not byte-exact");
    file.apply({});
    need(file.encode()==original,"ANM no-op changed bytes");
    rejects([&]{(void)file.info(stats.frames,0U);});
    rejects([&]{(void)file.full_frame(palette.first,palette.second);});
    rejects([&]{(void)Cine00File::encode_viewport({0U});});
    auto corrupt=original;
    corrupt.pop_back();
    rejects([&]{(void)Cine00File(corrupt);});
    corrupt=original;
    corrupt.push_back(0U);
    rejects([&]{(void)Cine00File(corrupt);});
    corrupt=original;
    corrupt.at(6U)=10U; // First frame's chunk-ID low byte.
    rejects([&]{(void)Cine00File(corrupt);});
    corrupt=original;
    corrupt.at(11U)=0x80U; // Signed negative size in first chunk header.
    rejects([&]{(void)Cine00File(corrupt);});
}
} // namespace

int main(int argc,char** argv) {
    exercise(synthetic(),false);
    std::cout<<"CINE00 synthetic regression PASS\n";
    if (argc>2) throw std::invalid_argument("usage: CINE00_tests [CINE00.ANM]");
    if (argc==2) {
        std::ifstream input(argv[1],std::ios::binary);
        if (!input) throw std::runtime_error("cannot open CINE00.ANM");
        const std::vector<std::uint8_t> original{
            std::istreambuf_iterator<char>(input),std::istreambuf_iterator<char>()};
        exercise(original,true);
        std::cout<<"CINE00 original-file regression PASS\n";
    }
}
