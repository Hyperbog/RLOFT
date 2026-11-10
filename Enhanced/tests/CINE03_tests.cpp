#include "rloft/CINE03.hpp"

#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <fstream>
#include <iterator>
#include <set>
#include <stdexcept>
#include <utility>
#include <vector>

namespace {
using rloft::Cine03File;
using rloft::Cine03Edits;
using rloft::Cine03PixelPatch;

void require(bool ok, const char* why) {
    if (!ok) throw std::runtime_error(why);
}
template<class F> void rejects(F&& operation) {
    bool failed = false;
    try { operation(); } catch (const std::exception&) { failed = true; }
    require(failed, "CINE03 accepted invalid, duplicate, or stale input");
}
void append16(std::vector<std::uint8_t>& out, std::uint16_t n) {
    out.push_back(static_cast<std::uint8_t>(n));
    out.push_back(static_cast<std::uint8_t>(n >> 8U));
}
void append32(std::vector<std::uint8_t>& out, std::uint32_t n) {
    for (unsigned i = 0U; i < 4U; ++i)
        out.push_back(static_cast<std::uint8_t>(n >> (i*8U)));
}
void chunk(std::vector<std::uint8_t>& out, std::uint16_t id,
           const std::vector<std::uint8_t>& bytes) {
    append16(out, id);
    append32(out, static_cast<std::uint32_t>(bytes.size()));
    out.insert(out.end(), bytes.begin(), bytes.end());
}
std::vector<std::uint8_t> rle() {
    std::vector<std::uint8_t> result;
    for (unsigned row = 0U; row < 400U; ++row)
        result.insert(result.end(),{0x81U,9U,0x81U,9U,0xC1U,9U});
    return result;
}
std::vector<std::uint8_t> fixture() {
    std::vector<std::uint8_t> result;
    append16(result,3U);append16(result,1U);
    append16(result,3U);
    chunk(result,1U,{});
    chunk(result,0U,std::vector<std::uint8_t>(768U,5U));
    chunk(result,2U,rle());
    append16(result,2U);
    std::vector<std::uint8_t> pixels(320U*256U,17U);
    pixels[0U]=11U; pixels[320U+3U]=22U;
    chunk(result,8U,rloft::Cine00File::encode_viewport(pixels));
    chunk(result,5U,{19U,0U,0U,0U});
    append16(result,3U);
    std::vector<std::uint8_t> pixels2(320U*256U,42U);
    chunk(result,8U,rloft::Cine00File::encode_viewport(pixels2));
    chunk(result,3U,{0U,0U});
    chunk(result,7U,{34U,0U,6U,0U});
    return result;
}

void synthetic_test() {
    const auto original = fixture();
    Cine03File file(original), target(original);
    require(file.encode()==original && file.stats().frames==3U &&
            file.stats().chunks==8U && file.stats().by_id[8U]==2U &&
            file.viewport_pixel(1U,0U,0U,0U)==11U &&
            file.viewport_pixel(1U,0U,3U,1U)==22U &&
            file.viewport_pixel(2U,0U,0U,0U)==42U &&
            file.snapshot(2U).pixels[51U*320U]==42U,
            "CINE03 synthetic image layout or original round-trip differs");
    const std::vector<Cine03PixelPatch> expected_variation{
        {1U,0U,0U,0U,11U,63U},
        {1U,0U,3U,1U,22U,24U},
        {2U,0U,319U,255U,42U,4U}
    };
    Cine03Edits change{};
    change.pixels=expected_variation;
    target.apply(change);
    const auto changes=file.pixel_differences(target);
    require(changes.size()==3U && target.pixel_differences(file).size()==3U &&
            target.viewport_pixel(1U,0U,0U,0U)==63U &&
            target.viewport_pixel(2U,0U,319U,255U)==4U,
            "CINE03 synthetic planar differences not recovered");
    change.pixels=changes;
    change.base.palette={{0U,1U,2U,5U,6U}};
    change.base.base.cues={{1U,1U,rloft::Cine01CueKind::MusicSequence,
                            19U,0U,20U,0U}};
    change.base.base.packets.expected_wait=1U;
    change.base.base.packets.replacement_wait=2U;
    file.apply(change);
    require(file.payload(1U,0U)==target.payload(1U,0U) &&
            file.payload(2U,0U)==target.payload(2U,0U) &&
            file.snapshot(2U).pixels[306U*320U+319U]==4U &&
            file.frame_info(0U).wait_ticks==2U &&
            file.cues()[0U].first==20U &&
            file.payload(0U,1U)[2U]==6U,
            "CINE03 combined image, palette, sound and wait edit failed");
    const auto checkpoint=file.encode();
    rejects([&]{file.apply(change);});
    Cine03Edits invalid{};
    invalid.pixels={{1U,0U,0U,0U,11U,99U}}; // Stale value.
    rejects([&]{file.apply(invalid);});
    invalid.pixels={{1U,0U,320U,0U,63U,99U}};
    rejects([&]{file.apply(invalid);});
    invalid.pixels={{1U,0U,0U,256U,63U,99U}};
    rejects([&]{file.apply(invalid);});
    invalid.pixels={{0U,1U,0U,0U,6U,99U}}; // Palette is not planar.
    rejects([&]{file.apply(invalid);});
    invalid.pixels={{99U,0U,0U,0U,63U,99U}};
    rejects([&]{file.apply(invalid);});
    invalid.pixels={{1U,0U,0U,0U,63U,12U},
                    {1U,0U,0U,0U,63U,13U}};
    rejects([&]{file.apply(invalid);});
    invalid.pixels={{1U,0U,0U,0U,63U,12U}};
    invalid.base.base.packets.chunks={{1U,0U,file.payload(1U,0U),target.payload(1U,0U)}};
    rejects([&]{file.apply(invalid);}); // Raw packet/typed pixel collision.
    invalid.base.base.packets.chunks.clear();
    invalid.base.palette={{0U,1U,2U,5U,65U}};
    rejects([&]{file.apply(invalid);}); // Invalid combined palette edit.
    require(file.encode()==checkpoint,"failed CINE03 transaction changed live bytes");

    Cine03Edits inverse{};
    for (const auto& p : changes)
        inverse.pixels.push_back({p.frame,p.chunk,p.x,p.y,p.replacement,p.expected});
    inverse.base.palette={{0U,1U,2U,6U,5U}};
    inverse.base.base.cues={{1U,1U,rloft::Cine01CueKind::MusicSequence,
                             20U,0U,19U,0U}};
    inverse.base.base.packets.expected_wait=2U;
    inverse.base.base.packets.replacement_wait=1U;
    file.apply(inverse);
    require(file.encode()==original && file.pixel_differences(Cine03File(original)).empty(),
            "CINE03 synthetic inverse did not restore every source byte");
    file.apply({});
    require(file.encode()==original,"empty CINE03 edit mutated source");
    auto incompatible=original;
    // Program command differs: pixel-only comparisons must refuse migration.
    Cine03Edits audio{};
    audio.base.base.cues={{1U,1U,rloft::Cine01CueKind::MusicSequence,
                           19U,0U,20U,0U}};
    Cine03File changed_audio(incompatible);
    changed_audio.apply(audio);
    rejects([&]{(void)file.pixel_differences(changed_audio);});
    auto truncated=original; truncated.pop_back();
    rejects([&]{(void)Cine03File(std::move(truncated));});
}

void original_test(const std::vector<std::uint8_t>& bytes) {
    Cine03File file(bytes);
    require(bytes.size()==7498347U && file.encode()==bytes &&
            file.stats().frames==128U && file.stats().chunks==131U &&
            file.stats().default_wait==1U &&
            file.stats().by_id==std::array<std::size_t,10>{
                {24U,1U,1U,9U,0U,1U,0U,2U,91U,2U}},
            "CINE03 original archive identity/structure mismatch");
    const auto cues=file.cues();
    require(cues.size()==5U && cues[0U].frame==1U &&
            cues[0U].first==19U && cues[1U].frame==7U &&
            cues[1U].first==34U && cues[1U].second==6U &&
            cues[2U].frame==8U && cues[2U].first==34U &&
            cues[3U].frame==76U && cues[3U].first==35U &&
            cues[4U].frame==82U && cues[4U].first==35U,
            "CINE03 original audio/subtitle commands mismatch");
    require(file.info(75U,1U).id==8U &&
            file.viewport_pixel(75U,1U,228U,156U)==0U &&
            file.info(83U,0U).id==8U &&
            file.viewport_pixel(83U,0U,224U,171U)==0U,
            "CINE03 distinct original viewport pixels mismatch");
    std::size_t alternate=0U;
    for (std::size_t frame=0U;frame<128U;++frame)
        alternate+=file.frame_info(frame).uses_image_wait ? 1U:0U;
    require(alternate==100U && file.snapshot(127U).last_palette_frame==127U &&
            file.snapshot(127U).last_image_frame==116U,
            "CINE03 original frame provenance or timing mismatch");

    const auto pixel=file.viewport_pixel(9U,0U,0U,0U);
    const auto before_palette=file.payload(0U,1U)[5U];
    Cine03Edits edit{};
    edit.pixels={{9U,0U,0U,0U,pixel,static_cast<std::uint8_t>(pixel^1U)}};
    edit.base.palette={{0U,1U,5U,before_palette,
                        static_cast<std::uint8_t>(before_palette^1U)}};
    edit.base.base.cues={{cues[0U].frame,cues[0U].chunk,cues[0U].kind,
                          cues[0U].first,cues[0U].second,20U,0U}};
    edit.base.base.packets.expected_wait=1U;
    edit.base.base.packets.replacement_wait=2U;
    file.apply(edit);
    require(file.viewport_pixel(9U,0U,0U,0U)==(pixel^1U) &&
            file.payload(0U,1U)[5U]==(before_palette^1U) &&
            file.cues()[0U].first==20U && file.stats().default_wait==2U,
            "CINE03 original-file atomic edits not applied");
    const auto checkpoint=file.encode();
    rejects([&]{file.apply(edit);});
    require(file.encode()==checkpoint,"CINE03 original stale edit was not atomic");
    Cine03Edits inverse{};
    inverse.pixels={{9U,0U,0U,0U,static_cast<std::uint8_t>(pixel^1U),pixel}};
    inverse.base.palette={{0U,1U,5U,static_cast<std::uint8_t>(before_palette^1U),before_palette}};
    inverse.base.base.cues={{cues[0U].frame,cues[0U].chunk,cues[0U].kind,
                             20U,0U,cues[0U].first,cues[0U].second}};
    inverse.base.base.packets.expected_wait=2U;
    inverse.base.base.packets.replacement_wait=1U;
    file.apply(inverse);
    require(file.encode()==bytes,"CINE03 original bytes were not restored exactly");
}

void sibling_test(const std::vector<std::uint8_t>& cine02,
                  const std::vector<std::uint8_t>& cine03) {
    Cine03File source(cine02),target(cine03);
    const auto changes=source.pixel_differences(target);
    std::set<std::pair<std::size_t,std::size_t>> changed_packets;
    for(const auto& p:changes){
        changed_packets.emplace(p.frame,p.chunk);
        require(source.info(p.frame,p.chunk).id==8U &&
                source.viewport_pixel(p.frame,p.chunk,p.x,p.y)==p.expected &&
                target.viewport_pixel(p.frame,p.chunk,p.x,p.y)==p.replacement,
                "CINE03 variant change is not a real planar pixel");
    }
    require(changes.size()==2534U && changed_packets.size()==34U &&
            changed_packets.begin()->first==75U &&
            changed_packets.rbegin()->first==115U,
            "CINE02/CINE03 original 2534-byte, 34-packet delta mismatch");
    Cine03Edits edits{}; edits.pixels=changes;
    source.apply(edits);
    require(source.encode()==cine03 && source.pixel_differences(target).empty(),
            "CINE03 complete sibling overlay did not reproduce target bytes");
    Cine03Edits inverse{};
    for(const auto& p:changes)
        inverse.pixels.push_back({p.frame,p.chunk,p.x,p.y,p.replacement,p.expected});
    source.apply(inverse);
    require(source.encode()==cine02,
            "CINE03 sibling overlay undo did not reproduce original bytes");
}

std::vector<std::uint8_t> read(const char* path) {
    std::ifstream in(path,std::ios::binary);
    if(!in)throw std::runtime_error("cannot open cinematic test fixture");
    return {std::istreambuf_iterator<char>(in),std::istreambuf_iterator<char>()};
}
} // namespace

int main(int argc,char** argv) {
    synthetic_test();
    if(argc>3)throw std::invalid_argument("usage: CINE03_tests [CINE03.ANM [CINE02.ANM]]");
    if(argc>=2) {
        const auto cine03=read(argv[1]);
        original_test(cine03);
        if(argc==3)sibling_test(read(argv[2]),cine03);
    }
}
